#include "sinks/SinkConfig.hpp"
#include "sinks/FileSinkImpl.hpp"
#include "sinks/ConsoleSinkImpl.hpp"

#include "sinks/LogSinkFactory.hpp"

std::unique_ptr<ILogSink> LogSinkFactory::CreateSink(const SinkConfig& SinkConfigRef) {
    switch(SinkConfigRef.type) {
        case SinkType::CONSOLE: 
            return std::make_unique<ConsoleSinkImpl>();  
            
        case SinkType::FILE:    
            return std::make_unique<FileSinkImpl>(
                const_cast<std::string&>(SinkConfigRef.filePath)
            );  
            
        default:                
            return nullptr;
    }
}