



#include "core/LogMessage.hpp"
#include "core/LogManager.hpp"
#include "sinks/ConsoleSinkImpl.hpp"
#include "sinks/FileSinkImpl.hpp"
#include <string>

int main() {
    LogMessage log1("MyApp", "Initialization", "INFO", "2024-06-01 10:00:00", "Application started successfully.");
    LogMessage log2("MyApp", "Database", "ERROR", "2024-06-01 10:05:00", "Failed to connect to the database.");
    LogMessage log3("MyApp", "UserModule", "WARN", "2024-06-01 10:10:00", "User profile is incomplete.");   

    LogManager logManager;

    logManager.addSink(new ConsoleSinkImpl());
    std::string filePath = "logs.txt";
    logManager.addSink(new FileSinkImpl(filePath));

    logManager.log(log1);
    logManager.log(log2);
    logManager.log(log3);

    logManager.flush();

    logManager.DeleteAllSinks();
    logManager.DeleteAllLogMessages();

    return 0;
}