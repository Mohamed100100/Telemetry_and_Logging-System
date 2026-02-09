


#pragma once 

#include <string>  
#include <string_view> 

class LogFormatterHelper{

    public:
        LogFormatterHelper() = default;
        LogFormatterHelper(const LogFormatterHelper& other) = delete;
        LogFormatterHelper(LogFormatterHelper &&other) = default;
        LogFormatterHelper& operator=(const LogFormatterHelper& other) = delete;
        LogFormatterHelper& operator=(LogFormatterHelper&& other) = default;
        ~LogFormatterHelper() = default;

        static std::string GetDescription(float value,const std::string &context,const std::string_view& unit);
        static std::string GetSeverity(float value,float criticalThreshold,float warningThreshold);
        static std::string GetCurrentTimeStamp();

};