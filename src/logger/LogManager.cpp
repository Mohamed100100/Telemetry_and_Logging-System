
#include <algorithm>
#include "logger/LogManager.hpp"



LogManager::LogManager(size_t LogBufferCapacity):LogMessagesBuffer{LogBufferCapacity}{}


void LogManager::addSink(ILogSink *SinkPtr){
    SinksBuffer.push_back(std::unique_ptr<ILogSink>(SinkPtr));
}
void LogManager::removeSink(ILogSink *SinkPtr){
    SinksBuffer.erase(
        std::remove_if( SinksBuffer.begin(), 
                              SinksBuffer.end(),
                              [SinkPtr](const std::unique_ptr<ILogSink>& ptr) { return ptr.get() == SinkPtr; }),
        SinksBuffer.end());
}
void LogManager::log(const LogMessage &log_message){
    LogMessagesBuffer.push_back(std::move(log_message));
}
void LogManager::flush(){
    for (size_t i = 0; i < LogMessagesBuffer.size(); i++) {
        for(auto& sink_ptr : SinksBuffer){
            sink_ptr->write(LogMessagesBuffer[i]);
        }
    }
    LogMessagesBuffer.clear();
}

void LogManager::DeleteAllSinks(){
    SinksBuffer.clear();
}

void LogManager::DeleteAllLogMessages(){
    LogMessagesBuffer.clear();
}


