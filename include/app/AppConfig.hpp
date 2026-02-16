#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace telemetry {

/**
 * @brief Telemetry data type
 */
enum class TelemetryType {
    CPU,
    GPU,
    RAM
};

/**
 * @brief Source type
 */
enum class SourceType {
    FILE,
    SOCKET,
    SOMEIP
};

/**
 * @brief Sink type
 */
enum class SinkType {
    CONSOLE,
    FILE
};

/**
 * @brief Single source configuration
 */
struct SourceConfig {
    SourceType sourceType;
    std::string path;
    TelemetryType telemetryType;
    uint32_t rateMs;
};

/**
 * @brief Single sink configuration
 */
struct SinkConfigData {
    SinkType sinkType;
    std::string path;
};

/**
 * @brief Complete application configuration
 */
struct AppConfig {
    std::string appName;
    std::vector<SourceConfig> sources;
    std::vector<SinkConfigData> sinks;
};

/**
 * @brief Load configuration from JSON file
 */
AppConfig loadConfig(const std::string& filePath);

/**
 * @brief Convert string to TelemetryType
 */
TelemetryType stringToTelemetryType(const std::string& str);

/**
 * @brief Convert string to SourceType
 */
SourceType stringToSourceType(const std::string& str);

/**
 * @brief Convert string to SinkType
 */
SinkType stringToSinkType(const std::string& str);

} // namespace telemetry