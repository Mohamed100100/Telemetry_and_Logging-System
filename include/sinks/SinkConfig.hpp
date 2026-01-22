    



#pragma once 

#include <string>
#include "enums/SinksType.hpp"


struct SinkConfig {
    SinkType type;
    std::string filePath;      // Only used for File sink
    
    SinkConfig() = default;
    // Static factory methods for easy config creation
    static SinkConfig Console();
    
    static SinkConfig File(const std::string& path);
};