#pragma once 

#include <string_view>
#include "enums/TelemetrySource.hpp"
#include "enums/SeverityLevel.hpp"

struct RamPolicy {
    // What type of telemetry this policy handles
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::RAM;
    
    // Unit of measurement
    static constexpr const char* unit = "MB";
    
    // Threshold values
    static constexpr float WARNING = 70.0f;
    static constexpr float CRITICAL = 85.0f;
};