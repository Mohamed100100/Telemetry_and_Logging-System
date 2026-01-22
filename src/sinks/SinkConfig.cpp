#include "sinks/SinkConfig.hpp"

SinkConfig SinkConfig::Console() {
    SinkConfig config;
    config.type = SinkType::CONSOLE;
    config.filePath = "";
    return config;
}

SinkConfig SinkConfig::File(const std::string& path) {
    SinkConfig config;
    config.type = SinkType::FILE;
    config.filePath = path;
    return config;
}