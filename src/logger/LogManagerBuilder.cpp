


#include "logger/LogManagerBuilder.hpp"


LogManagerBuilder::LogManagerBuilder(){
    PtrToLogManeger.reset(new LogManager());
}

LogManagerBuilder& LogManagerBuilder::addSink(ILogSink *SinkPtr){
    PtrToLogManeger->addSink(SinkPtr);
    return *this;
}
LogManagerBuilder& LogManagerBuilder::addLogMessage(LogMessage &LogMessageRef){
    PtrToLogManeger->log(LogMessageRef);
    return *this;
}
std::unique_ptr<LogManager> LogManagerBuilder::build(){
    return std::move(PtrToLogManeger);
}