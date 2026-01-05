#pragma once

#include <string>

class LogMessage {
    private:
        std::string appName;
        std::string context;
        std::string severity;
        std::string time;
        std::string message;

    public:
        LogMessage()  = delete;
        LogMessage(const std::string& appName,
                   const std::string& context,
                   const std::string& severity,
                   const std::string& time,
                   const std::string& message);

        LogMessage(const LogMessage& other) = default;
        LogMessage & operator =(LogMessage& other) = default;
        
        LogMessage(LogMessage&& other) = default;
        LogMessage & operator =(LogMessage&& other) = default;
        
        std::string ToString();
        
        ~LogMessage() = default;
};