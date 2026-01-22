


#pragma once 

#include <memory>
#include "sinks/ILogSink.hpp"
#include "enums/SinksType.hpp"
#include "sinks/SinkConfig.hpp"   


class LogSinkFactory{
    public:
        LogSinkFactory() = default;
        LogSinkFactory(const LogSinkFactory& other)=delete;
        LogSinkFactory(LogSinkFactory&& other) = default;
        LogSinkFactory& operator=(const LogSinkFactory& other) = delete;
        LogSinkFactory& operator=(LogSinkFactory&& other) = default;
        
        std::unique_ptr<ILogSink> CreateSink(const SinkConfig& SinkConfigRef);

        ~LogSinkFactory()=default;
};