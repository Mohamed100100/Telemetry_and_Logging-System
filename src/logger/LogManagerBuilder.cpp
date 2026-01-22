#include "logger/LogManagerBuilder.hpp"

LogManagerBuilder& LogManagerBuilder::setBufferSize(size_t size) {
    bufferSize_ = size;
    return *this;
}

LogManagerBuilder& LogManagerBuilder::addSink(ILogSink* SinkPtr) {
    if (!PtrToLogManager) {
        PtrToLogManager = std::make_unique<LogManager>(bufferSize_);
    }
    PtrToLogManager->addSink(SinkPtr);
    return *this;
}

LogManagerBuilder& LogManagerBuilder::addLogMessage(LogMessage& LogMessageRef) {
    if (!PtrToLogManager) {
        PtrToLogManager = std::make_unique<LogManager>(bufferSize_);
    }
    PtrToLogManager->log(LogMessageRef);
    return *this;
}

std::unique_ptr<LogManager> LogManagerBuilder::build() {
    if (!PtrToLogManager) {
        PtrToLogManager = std::make_unique<LogManager>(bufferSize_);
    }
    return std::move(PtrToLogManager);
}