


#include "core/LogMessage.hpp"

LogMessage::LogMessage(const std::string& appName,
                       const std::string& context,
                       const std::string& severity,
                       const std::string& time,
                       const std::string& message)
                     : appName(appName),
                       context(context),
                       severity(severity),
                       time(time),
                       message(message) {}

std::string LogMessage::ToString(){
    return "[" + time + "] " + "<" + severity + "> " + "(" + appName + " - " + context + ") : " + message;
}



                       