#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>

#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "sinks/SinkConfig.hpp"
#include "logger/LogManager.hpp"

// ============================================================
// Helper: Create sample telemetry data file
// ============================================================
void createSampleTelemetryFile(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create telemetry file\n";
        return;
    }
    
    // Format: TYPE,VALUE
    file << "CPU,45.2\n";
    file << "GPU,50.0\n";
    file << "RAM,40.0\n";
    file << "CPU,67.8\n";
    file << "GPU,75.5\n";
    file << "RAM,65.0\n";
    file << "CPU,78.5\n";
    file << "GPU,88.0\n";
    file << "RAM,72.5\n";
    file << "CPU,92.1\n";
    file << "GPU,96.5\n";
    file << "RAM,88.0\n";
    file << "CPU,55.0\n";
    file << "GPU,60.0\n";
    file << "RAM,50.0\n";
    
    file.close();
    std::cout << "[Setup] Created telemetry file: " << filePath << "\n";
}

// ============================================================
// Helper: Read telemetry data from file
// ============================================================
struct TelemetryReading {
    std::string type;
    std::string value;
};

std::vector<TelemetryReading> readTelemetryFile(const std::string& filePath) {
    std::vector<TelemetryReading> readings;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open telemetry file\n";
        return readings;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            TelemetryReading reading;
            reading.type = line.substr(0, commaPos);
            reading.value = line.substr(commaPos + 1);
            readings.push_back(reading);
        }
    }
    
    file.close();
    return readings;
}

// ============================================================
// Main Demo
// ============================================================
int main() {
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║     PHASE 4: ASYNC LOGGING WITH THREAD POOL DEMO         ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n\n";

    // ──────────────────────────────────────────────────────────
    // STEP 1: Create sample telemetry data file
    // ──────────────────────────────────────────────────────────
    const std::string telemetryFilePath = "telemetry_data.txt";
    createSampleTelemetryFile(telemetryFilePath);
    std::cout << "\n";

    // ──────────────────────────────────────────────────────────
    // STEP 2: Create formatters for each telemetry type
    // ──────────────────────────────────────────────────────────
    std::cout << "[Setup] Creating formatters...\n";
    LogFormatter<CpuPolicy> cpuFormatter("TelemetryMonitor");
    LogFormatter<GpuPolicy> gpuFormatter("TelemetryMonitor");
    LogFormatter<RamPolicy> ramFormatter("TelemetryMonitor");
    std::cout << "[Setup] Formatters created for CPU, GPU, RAM\n\n";

    // ──────────────────────────────────────────────────────────
    // STEP 3: Create sinks using factory
    // ──────────────────────────────────────────────────────────
    std::cout << "[Setup] Creating sinks...\n";
    LogSinkFactory factory;

    // ──────────────────────────────────────────────────────────
    // STEP 4: Create LogManager with ThreadPool
    //         - Buffer capacity: 100 messages
    //         - ThreadPool size: 4 workers (for parallel sink writing)
    // ──────────────────────────────────────────────────────────
    std::cout << "[Setup] Creating LogManager with async processing...\n";
    
    // Note: LogManager constructor starts:
    //   1. RingBuffer for message queuing
    //   2. FlushingThread for consuming messages
    //   3. ThreadPool for parallel sink writing
    LogManager logManager(100, 4);  // 100 message buffer, 4 pool threads
    
    // Add multiple sinks - they will be written in PARALLEL!
    logManager.addSink(factory.CreateSink(SinkConfig::Console()).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("system.log")).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("telemetry_detailed.log")).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("errors_only.log")).release());
    
    std::cout << "[Setup] Added 4 sinks: Console + 3 Files\n";
    std::cout << "[Setup] LogManager ready! Worker thread and ThreadPool running.\n\n";

    // ──────────────────────────────────────────────────────────
    // STEP 5: Read telemetry from file and log
    // ──────────────────────────────────────────────────────────
    std::cout << "┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              PROCESSING TELEMETRY DATA                     │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";

    auto readings = readTelemetryFile(telemetryFilePath);
    std::cout << "[Main] Read " << readings.size() << " telemetry readings from file\n\n";

    auto startTime = std::chrono::high_resolution_clock::now();

    for (const auto& reading : readings) {
        std::optional<LogMessage> msg;
        
        // Format based on telemetry type
        if (reading.type == "CPU") {
            msg = cpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.type == "GPU") {
            msg = gpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.type == "RAM") {
            msg = ramFormatter.formatDataToLogMsg(reading.value);
        }
        
        // Log the message (NON-BLOCKING!)
        if (msg.has_value()) {
            logManager.log(msg.value());
            std::cout << "[Main] Pushed " << reading.type << " = " << reading.value << "\n";
        }
        
        // Small delay to simulate real telemetry interval
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "\n[Main] All " << readings.size() << " readings pushed in " 
              << duration.count() << "ms\n";

    // ──────────────────────────────────────────────────────────
    // STEP 6: Main thread can do other work!
    // ──────────────────────────────────────────────────────────
    std::cout << "\n┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              MAIN THREAD DOING OTHER WORK                  │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";

    std::cout << "[Main] While worker processes logs, main thread does other work...\n";
    
    for (int i = 1; i <= 5; i++) {
        std::cout << "[Main] Other work progress: " << (i * 20) << "%\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "[Main] Other work completed!\n";

    // ──────────────────────────────────────────────────────────
    // STEP 7: Graceful shutdown
    // ──────────────────────────────────────────────────────────
    std::cout << "\n┌────────────────────────────────────────────────────────────┐\n";
    std::cout << "│              GRACEFUL SHUTDOWN                             │\n";
    std::cout << "└────────────────────────────────────────────────────────────┘\n\n";

    std::cout << "[Main] Exiting scope - LogManager destructor will:\n";
    std::cout << "       1. Signal FlushingThread to stop\n";
    std::cout << "       2. Wait for remaining messages to be processed\n";
    std::cout << "       3. Stop ThreadPool (wait for pending sink writes)\n";
    std::cout << "       4. Join all threads\n";
    std::cout << "       5. Clean up resources\n\n";

    // LogManager destructor called automatically here
    return 0;
}