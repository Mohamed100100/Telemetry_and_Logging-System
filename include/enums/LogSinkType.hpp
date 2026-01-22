#pragma once 


// Types of log output destinations
enum class LogSinkType_enum {
    CONSOLE,    // Standard output
    FILE,       // File output
    SOCKET      // Socket output (future)
};
