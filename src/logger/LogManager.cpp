
#include <algorithm>
#include "logger/LogManager.hpp"


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
    for(auto& log_message : LogMessagesBuffer){
        for(auto& sink_ptr : SinksBuffer){
            sink_ptr->write(log_message);
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


