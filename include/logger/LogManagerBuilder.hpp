

#pragma once 

#include <memory>
#include "logger/LogManager.hpp"

class LogManagerBuilder{
    private:
        std::unique_ptr<LogManager> PtrToLogManeger;
    public:
        LogManagerBuilder();
        LogManagerBuilder(const LogManagerBuilder& other) = delete;
        LogManagerBuilder(LogManagerBuilder &&other) = default;
        LogManagerBuilder& operator=(LogManagerBuilder &&other) = default;
        LogManagerBuilder& operator=(const LogManagerBuilder &other) = delete;

        ~LogManagerBuilder()=default;


        LogManagerBuilder& addSink(ILogSink *SinkRef);
        LogManagerBuilder& addLogMessage(LogMessage &LogMessageRef);
        std::unique_ptr<LogManager> build();
};