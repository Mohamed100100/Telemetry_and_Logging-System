#include <algorithm>
#include "logger/LogManager.hpp"

// ============================================
// Constructor
// ============================================
LogManager::LogManager(size_t LogBufferCapacity, size_t threadPoolSize)
    : LogMessagesBuffer{LogBufferCapacity}
    , stopFlushing{false}
    , threadPool{threadPoolSize}
{
    FlushingThread = std::thread(&LogManager::workLoop, this); // Then start thread
}

// ============================================
// Sink Management
// ============================================
void LogManager::addSink(ILogSink *SinkPtr){
    SinksBuffer.push_back(std::unique_ptr<ILogSink>(SinkPtr));
}

void LogManager::removeSink(ILogSink *SinkPtr){
    SinksBuffer.erase(
        std::remove_if(SinksBuffer.begin(), 
                       SinksBuffer.end(),
                       [SinkPtr](const std::unique_ptr<ILogSink>& ptr) { 
                           return ptr.get() == SinkPtr; 
                       }),
        SinksBuffer.end());
}

void LogManager::DeleteAllSinks(){
    SinksBuffer.clear();
}

void LogManager::DeleteAllLogMessages(){
    LogMessagesBuffer.clear();
}

// ============================================
// Logging (Called by Main Thread)
// ============================================
void LogManager::log(const LogMessage &log_message){
    {
        std::lock_guard<std::mutex> lock(mx);   // Simpler than unique_lock here
        LogMessagesBuffer.try_push(std::move(log_message));
    }
    cv.notify_one();
}

// ============================================
// Worker Loop (Runs in Background Thread)
// ============================================
void LogManager::workLoop(){
    while (true) {
        std::unique_lock<std::mutex> lock(mx);
        
        // Wait until: have data OR stopping
        cv.wait(lock, [this]{ 
            return stopFlushing.load() || !LogMessagesBuffer.empty(); 
        });
        
        // Exit condition: stopping AND buffer empty
        if (stopFlushing.load() && LogMessagesBuffer.empty()) {
            return;  // Exit the loop and end thread
        }
        
        // Unlock before processing (so main thread can push more)
        lock.unlock();
        
        // Pop and write ONE message
        auto msg = LogMessagesBuffer.try_pop();
        if (msg.has_value()) {
            for (auto& sink_ptr : SinksBuffer) {
                // the arguments are copied here, but that's fine since LogMessage is small and we want to avoid dangling references
                threadPool.submit([sink_ptr = sink_ptr.get(), msg = *msg] {
                    sink_ptr->write(msg);
                });
            }
        }
    }
}

// ============================================
// Destructor
// ============================================
LogManager::~LogManager(){
    // Signal worker to stop
    {
        std::lock_guard<std::mutex> lock(mx);
        stopFlushing = true;
    }
    cv.notify_all();
    
    // Wait for worker to finish
    if (FlushingThread.joinable()) {
        FlushingThread.join();
    }
    
    // Clean up
    SinksBuffer.clear();
}