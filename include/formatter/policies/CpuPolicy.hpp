#pragma once 

#include <string_view>
#include "enums/TelemetrySource.hpp"
#include "enums/SeverityLevel.hpp"

struct CpuPolicy {
    // What type of telemetry this policy handles
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
    
    // Unit of measurement
    static constexpr const char* unit = "%";
    
    // Threshold values
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
};