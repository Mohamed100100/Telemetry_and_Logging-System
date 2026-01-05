#pragma once
#include "LogMessage.hpp"

class ILogSink{
public:
    virtual void write(LogMessage log_message) = 0;
    virtual ~ILogSink() = default;
};