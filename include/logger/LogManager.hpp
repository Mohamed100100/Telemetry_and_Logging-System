#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../utils/RingBuffer.hpp"
#include "logger/LogMessage.hpp"
#include "sinks/ILogSink.hpp"
#include "utils/ThreadPool.hpp"


class LogManager {
    private :
        std::vector<std::unique_ptr<ILogSink>> SinksBuffer;
        RingBuffer<LogMessage> LogMessagesBuffer;
        std::thread FlushingThread;
        std::atomic<bool> stopFlushing;
        std::mutex mx;
        std::condition_variable cv;
        ThreadPool threadPool;



    public:
        LogManager() = delete;
        LogManager(size_t LogBufferCapacity=100, size_t threadPoolSize=5);
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

        void workLoop();

        ~LogManager();
};