

#include <chrono>
#include <iomanip>
#include <string_view>
#include "formatter/LogFormatterHelper.hpp"


std::string LogFormatterHelper::GetDescription(float value,
                                               const std::string &context,
                                               const std::string_view& unit){
    std::string msg;
    msg+= context;
    msg+= "usage : ";
    msg+= std::to_string(value); 
    msg += std::string(unit);
    return msg;
}

std::string LogFormatterHelper::GetSeverity(float value,float criticalThreshold,float warningThreshold){
    if(value > criticalThreshold){
        return "CRITICAL";
    }else if(value > warningThreshold){
        return "WARNING";
    }else{
        return "INFO";
    }
}

std::string LogFormatterHelper::GetCurrentTimeStamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return oss.str();
}

