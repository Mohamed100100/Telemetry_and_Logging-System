#pragma once

#include <vector>
#include <memory>

#include "../utils/RingBuffer.hpp"
#include "logger/LogMessage.hpp"
#include "sinks/ILogSink.hpp"

class LogManager {
    private :
        std::vector<std::unique_ptr<ILogSink>> SinksBuffer;
        RingBuffer<LogMessage> LogMessagesBuffer;

    public:
        LogManager() = delete;
        explicit LogManager(size_t LogBufferCapacity = 1000); 
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