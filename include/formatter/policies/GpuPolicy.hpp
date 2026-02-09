#pragma once 

#include <string_view>
#include "enums/TelemetrySource.hpp"
#include "enums/SeverityLevel.hpp"

struct GpuPolicy {
    // What type of telemetry this policy handles
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::GPU;
    
    // Unit of measurement
    static constexpr std::string_view unit = "%";
    
    // Threshold values
    static constexpr float WARNING = 80.0f;
    static constexpr float CRITICAL = 95.0f;
};