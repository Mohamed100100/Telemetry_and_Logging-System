#pragma once

#include <memory>
#include "logger/LogManager.hpp"

class LogManagerBuilder {
private:
    std::unique_ptr<LogManager> PtrToLogManager;
    size_t bufferSize_ = 1000;

public:
    // FIXED: Allow default construction for Builder pattern
    LogManagerBuilder() = default;  // ✅ Changed from = delete

    LogManagerBuilder(const LogManagerBuilder& other) = delete;
    LogManagerBuilder& operator=(const LogManagerBuilder& other) = delete;

    LogManagerBuilder(LogManagerBuilder&& other) = default;
    LogManagerBuilder& operator=(LogManagerBuilder&& other) = default;

    ~LogManagerBuilder() = default;

    LogManagerBuilder& setBufferSize(size_t size);
    LogManagerBuilder& addSink(ILogSink* SinkPtr);
    LogManagerBuilder& addLogMessage(LogMessage& LogMessageRef);

    std::unique_ptr<LogManager> build();
};