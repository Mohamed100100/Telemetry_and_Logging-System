#pragma once

#include <vector>
#include <memory>
#include "logger/LogMessage.hpp"
#include "sinks/ILogSink.hpp"

class LogManager {
    private :
        std::vector<std::unique_ptr<ILogSink>> SinksBuffer;
        std::vector<LogMessage> LogMessagesBuffer;

    public:
        LogManager() = default;
        LogManager(const LogManager& other) = delete;
        LogManager(LogManager&& other) = delete;

        LogManager& operator =(const LogManager& other) = delete;
        LogManager& operator =(LogManager&& other) = default;

        void addSink(ILogSink *SinkPtr);
        void removeSink(ILogSink *SinkPtr);
        void log(const LogMessage &log_message);
        void flush();
        void DeleteAllSinks();
        void DeleteAllLogMessages();

        ~LogManager() = default;
};