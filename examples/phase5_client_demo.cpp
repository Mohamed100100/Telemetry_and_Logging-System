// ═══════════════════════════════════════════════════════════════════
// TELEMETRY CLIENT DEMO
// Run this after starting the service
// ═══════════════════════════════════════════════════════════════════

#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>

// Your existing headers
#include "sources/SomeIPTelemetrySourceAdapter.hpp"
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "sinks/SinkConfig.hpp"
#include "logger/LogManager.hpp"

// ═══════════════════════════════════════════════════════════════════
// Global flag for graceful shutdown
// ═══════════════════════════════════════════════════════════════════

static bool running = true;

void signalHandler(int signal) {
    std::cout << "\n[Client] Received signal " << signal << ", shutting down...\n";
    running = false;
}

// ═══════════════════════════════════════════════════════════════════
// Main Demo
// ═══════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║     PHASE 5: SOMEIP TELEMETRY CLIENT DEMO                ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n\n";
    
    // Setup signal handler
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // ───────────────────────────────────────────────────────────────
    // STEP 1: Create the SomeIP Telemetry Source Adapter
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 1: CREATE ADAPTER                        │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    SomeIPTelemetrySourceAdapter telemetrySource;
    
    // ───────────────────────────────────────────────────────────────
    // STEP 2: Open connection (init + connect)
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 2: CONNECT TO SERVICE                    │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    if (!telemetrySource.openSource()) {
        std::cerr << "[Client] Failed to connect to telemetry service!\n";
        std::cerr << "[Client] Make sure the service is running.\n";
        return 1;
    }
    std::cout << "[Client] Connected to telemetry service!\n\n";
    
    // ───────────────────────────────────────────────────────────────
    // STEP 3: Create Formatter (using CpuPolicy for this demo)
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 3: CREATE FORMATTER                      │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    LogFormatter<CpuPolicy> cpuFormatter("TelemetryMonitor");
    std::cout << "[Client] CPU Formatter created\n\n";
    
    // ───────────────────────────────────────────────────────────────
    // STEP 4: Create LogManager with Sinks
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 4: CREATE LOG MANAGER                    │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    // Create LogManager with buffer size and thread pool
    LogManager logManager(100, 4);  // 100 buffer, 4 pool threads
    
    // Create sinks using factory
    LogSinkFactory factory;
    logManager.addSink(factory.CreateSink(SinkConfig::Console()).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("telemetry_someip.log")).release());
    
    std::cout << "[Client] LogManager created with Console and File sinks\n\n";
    
    // ───────────────────────────────────────────────────────────────
    // STEP 5: Request Telemetry and Log
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 5: REQUEST AND LOG TELEMETRY             │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    std::cout << "[Client] Starting telemetry collection... (Ctrl+C to stop)\n\n";
    
    int requestCount = 0;
    const int maxRequests = 20;  // Limit for demo, remove for continuous
    
    while (running) {
        // Read telemetry from SomeIP service via adapter
        std::string telemetryData;
        
        if (telemetrySource.readSource(telemetryData)) {
            std::cout << "[Client] Received raw data: " << telemetryData << "%\n";
            
            // Format the data using LogFormatter
            auto logMessage = cpuFormatter.formatDataToLogMsg(telemetryData);
            
            if (logMessage.has_value()) {
                // Log the message (non-blocking, async!)
                logManager.log(logMessage.value());
                std::cout << "[Client] Message logged (async)\n";
            } else {
                std::cerr << "[Client] Failed to format telemetry data\n";
            }
        } else {
            std::cerr << "[Client] Failed to read telemetry data\n";
            
            // Try to reconnect
            std::cout << "[Client] Attempting to reconnect...\n";
            // telemetrySource.clos();
            if (!telemetrySource.openSource()) {
                std::cerr << "[Client] Reconnection failed. Exiting.\n";
                break;
            }
        }
        
        std::cout << "[Client] Request " << requestCount << "/" << maxRequests << " completed\n";
        std::cout << "─────────────────────────────────────────────────────────────\n\n";
        
        // Wait before next request
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    // ───────────────────────────────────────────────────────────────
    // STEP 6: Cleanup
    // ───────────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              STEP 6: CLEANUP                               │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";
    
    // telemetrySource.close();
    std::cout << "[Client] Telemetry source closed\n";
    
    std::cout << "\n[Client] Demo completed. Check 'telemetry_someip.log' for logged data.\n";
    std::cout << "[Client] LogManager destructor will flush remaining messages.\n";
    
    return 0;
}