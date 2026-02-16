#include "app/AppConfig.hpp"
#include <json.hpp>
#include <fstream>
#include <stdexcept>

using json = nlohmann::json;

namespace telemetry {

TelemetryType stringToTelemetryType(const std::string& str) {
    if (str == "GPU") return TelemetryType::GPU;
    if (str == "RAM") return TelemetryType::RAM;
    return TelemetryType::CPU;
}

SourceType stringToSourceType(const std::string& str) {
    if (str == "socket") return SourceType::SOCKET;
    if (str == "someip") return SourceType::SOMEIP;
    return SourceType::FILE;
}

SinkType stringToSinkType(const std::string& str) {
    if (str == "file") return SinkType::FILE;
    return SinkType::CONSOLE;
}

AppConfig loadConfig(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filePath);
    }

    json j;
    file >> j;
    
    AppConfig config;

    // Application name
    if (j.contains("application") && j["application"].contains("name")) {
        config.appName = j["application"]["name"].get<std::string>();
    } else {
        config.appName = "TelemetryApp";
    }

    // Sources
    if (j.contains("sources")) {
        for (auto& src : j["sources"]) {
            SourceConfig sc;
            
            sc.sourceType = stringToSourceType(src["type"].get<std::string>());
            
            if (src.contains("path")) {
                sc.path = src["path"].get<std::string>();
            }
            
            sc.telemetryType = stringToTelemetryType(src["telemetryType"].get<std::string>());
            sc.rateMs = src["rateMs"].get<uint32_t>();
            
            config.sources.push_back(sc);
        }
    }

    // Sinks
    if (j.contains("sinks")) {
        for (auto& snk : j["sinks"]) {
            SinkConfigData sc;
            
            sc.sinkType = stringToSinkType(snk["type"].get<std::string>());
            
            if (snk.contains("path")) {
                sc.path = snk["path"].get<std::string>();
            }
            
            config.sinks.push_back(sc);
        }
    }

    return config;
}

} // namespace telemetry