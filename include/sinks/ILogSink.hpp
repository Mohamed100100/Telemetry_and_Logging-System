#pragma once
#include "logger/LogMessage.hpp"

class ILogSink{
public:
    virtual void write(const LogMessage &log_message) = 0;
    virtual ~ILogSink() = default;
};