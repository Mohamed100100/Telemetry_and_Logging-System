#pragma once 

#include "core/ILogSink.hpp"

class ConsoleSinkImpl : public ILogSink{
    public:
        ConsoleSinkImpl() = default;
        ConsoleSinkImpl(const ConsoleSinkImpl & other) = delete;
        ConsoleSinkImpl(ConsoleSinkImpl && other) = default;

        ConsoleSinkImpl &operator = (const ConsoleSinkImpl& other) = delete;
        ConsoleSinkImpl & operator =(ConsoleSinkImpl && other) = default;

        virtual void write(LogMessage log_message);

        virtual ~ConsoleSinkImpl() = default;
};