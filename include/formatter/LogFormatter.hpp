# pragma once 

#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <optional>
#include <iostream>
#include "../third_party/magic_enum.hpp"
#include "logger/LogMessage.hpp"
#include "formatter/LogFormatterHelper.hpp"

template <typename _PolicyType>
class LogFormatter {
    private:
        std::string AppName;
        
        std::string GetContext(){
            return (std::string(magic_enum::enum_name(_PolicyType::context)));
        }

    public :
        LogFormatter() = delete;
        LogFormatter(const std::string &CopyAppName):AppName{CopyAppName}{}
        LogFormatter(const LogFormatter & other ) = delete;
        LogFormatter& operator=(const LogFormatter& other) = delete;
        LogFormatter(LogFormatter &&other) = default;
        LogFormatter& operator=(LogFormatter && other) = default;
        ~LogFormatter() = default;

        std::optional<LogMessage> formatDataToLogMsg(const std::string& raw){
            try {
                float value = std::stof(raw); // Converts the string to a float

                return LogMessage(
                    AppName,
                    GetContext(),
                    LogFormatterHelper::GetSeverity(value,_PolicyType::CRITICAL,_PolicyType::WARNING),
                    LogFormatterHelper::GetCurrentTimeStamp(),
                    LogFormatterHelper::GetDescription(value,GetContext(),_PolicyType::unit)
                );
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid argument - " << e.what() << std::endl;
                return std::nullopt;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Out of range - " << e.what() << std::endl;
                return std::nullopt;
            }
        }
};