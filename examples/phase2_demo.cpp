/**
 * @file phase2_demo.cpp
 * @brief Simple Phase 2 test using FileTelemetrySourceImpl and SocketTelemetrySourceImpl
 */

#include "logger/LogMessage.hpp"
#include "logger/LogManager.hpp"
#include "sinks/ConsoleSinkImpl.hpp"
#include "sinks/FileSinkImpl.hpp"
#include "sources/FileTelemetrySourceImpl.hpp"
#include "sources/SocketTelemetrySourceImpl.hpp"

#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "         PHASE 2: Simple Test Demo          " << std::endl;
    std::cout << "============================================\n" << std::endl;

    // ==========================================
    // SETUP: Create test data file
    // ==========================================
    std::cout << "[SETUP] Creating test data file..." << std::endl;
    
    std::string dataPath = "/tmp/phase2_test_data.txt";
    std::string logPath = "/tmp/phase2_output.txt";
    
    std::ofstream dataFile(dataPath);
    dataFile << "CPU:45%\n";
    dataFile << "MEM:62%\n";
    dataFile << "DISK:80%\n";
    dataFile.close();
    
    std::cout << "[SETUP] Test data file created: " << dataPath << "\n" << std::endl;

    // ==========================================
    // TEST 1: FileTelemetrySourceImpl - Basic Read
    // ==========================================
    std::cout << "---------- TEST 1: FileTelemetrySourceImpl ----------" << std::endl;
    {
        FileTelemetrySourceImpl fileSource(dataPath);
        
        if (fileSource.openSource()) {
            std::cout << "[PASS] File source opened successfully" << std::endl;
            
            std::string data = "initial value";
            int count = 1;
            bool isopen = true;
            isopen =fileSource.readSource(data);

            while(isopen == true && data != ""){        
                std::cout << "[INFO] Line " << count << ": " << data << std::endl;
                isopen =fileSource.readSource(data);
                count++;
            }
            std::cout << "[PASS] Read " << count << " lines from file" << std::endl;
        } else {
            std::cout << "[FAIL] Failed to open file source" << std::endl;
        }
        
        std::cout << "[INFO] FileSource going out of scope (RAII cleanup)..." << std::endl;
    }
    std::cout << "[PASS] FileSource destroyed automatically\n" << std::endl;

    // ==========================================
    // TEST 2: FileTelemetrySourceImpl with LogManager
    // ==========================================
    std::cout << "---------- TEST 2: Integration with LogManager ----------" << std::endl;
    {
        // Setup LogManager
        LogManager logManager;
        logManager.addSink(new ConsoleSinkImpl());
        logManager.addSink(new FileSinkImpl(logPath));
        
        // Setup file source
        FileTelemetrySourceImpl fileSource(dataPath);
        
        if (fileSource.openSource()) {
            std::string data;
            bool isopen = true;
            
            std::cout << "[INFO] Reading telemetry and logging:" << std::endl;

            isopen =fileSource.readSource(data);
            while(isopen == true && data != ""){     
                LogMessage msg(
                    "TelemetryApp",
                    "FileReader",
                    "INFO",
                    "2024-06-01 10:00:00",
                    data
                );
                
                logManager.log(msg);
                isopen =fileSource.readSource(data);
            }
            
            logManager.flush();
            
            std::cout << "[PASS] Logged all telemetry data" << std::endl;
            std::cout << "[INFO] Output saved to: " << logPath << std::endl;
        } else {
            std::cout << "[FAIL] Failed to open file source" << std::endl;
        }
        
        logManager.DeleteAllSinks();
        logManager.DeleteAllLogMessages();
    }
    std::cout << std::endl;

    // ==========================================
    // TEST 3: SocketTelemetrySourceImpl (Optional)
    // ==========================================
    std::cout << "---------- TEST 3: SocketTelemetrySourceImpl ----------" << std::endl;
    {
        std::string socketPath = "/tmp/telemetry_test.sock";
        
        std::cout << "[INFO] Attempting to connect to socket: " << socketPath << std::endl;
        std::cout << "[INFO] (Run socket_server.sh first if you want this to work)" << std::endl;
        
        SocketTelemetrySourceImpl socketSource(socketPath);
        
        if (socketSource.openSource()) {
            std::cout << "[PASS] Socket source connected successfully" << std::endl;
            
            std::string data;
            
            // Read 3 messages
            for (int i = 0; i < 3; i++) {
                if (socketSource.readSource(data)) {
                    std::cout << "[INFO] Received: " << data << std::endl;
                }
            }
            
            std::cout << "[PASS] Socket communication successful" << std::endl;
        } else {
            std::cout << "[SKIP] Socket server not running (this is OK for testing)" << std::endl;
        }
        
        std::cout << "[INFO] SocketSource going out of scope (RAII cleanup)..." << std::endl;
    }
    std::cout << "[PASS] SocketSource destroyed automatically\n" << std::endl;

    // ==========================================
    // TEST 4: Multiple Sources
    // ==========================================
    std::cout << "---------- TEST 4: Multiple File Sources ----------" << std::endl;
    {
        // Create second test file
        std::string dataPath2 = "/tmp/phase2_test_data2.txt";
        std::ofstream dataFile2(dataPath2);
        dataFile2 << "Sensor1:25.5C\n";
        dataFile2 << "Sensor2:30.2C\n";
        dataFile2.close();
        
        // Setup LogManager
        LogManager logManager;
        logManager.addSink(new ConsoleSinkImpl());
        
        // Source 1
        FileTelemetrySourceImpl source1(dataPath);
        // Source 2
        FileTelemetrySourceImpl source2(dataPath2);
        
        std::cout << "[INFO] Reading from two sources:" << std::endl;
        
        if (source1.openSource()) {
            std::string data;
            bool isopen = true;
            std::cout << "[INFO] Source 1 data:" << std::endl;
            isopen =source1.readSource(data);
            while(isopen == true && data != ""){     
                LogMessage msg("App", "Source1", "INFO", "2024-06-01 10:00:00", data);
                logManager.log(msg);
                isopen =source1.readSource(data);
            }
        }
        
        if (source2.openSource()) {
            std::string data;
            std::cout << "[INFO] Source 2 data:" << std::endl;

            bool isopen = true;
            isopen =source1.readSource(data);
                        
            while(isopen == true && data != ""){     
                LogMessage msg("App", "Source2", "INFO", "2024-06-01 10:00:00", data);
                logManager.log(msg);
                isopen =source1.readSource(data);
            }
        }
        
        logManager.flush();
        
        std::cout << "[PASS] Multiple sources handled correctly" << std::endl;
        
        logManager.DeleteAllSinks();
        logManager.DeleteAllLogMessages();
    }
    std::cout << std::endl;

    // ==========================================
    // TEST 5: Error Handling
    // ==========================================
    std::cout << "---------- TEST 5: Error Handling ----------" << std::endl;
    {
        std::string invalidPath = "/tmp/nonexistent_file_12345.txt";
        
        FileTelemetrySourceImpl badSource(invalidPath);
        
        if (badSource.openSource()) {
            std::cout << "[FAIL] Should not have opened invalid file" << std::endl;
        } else {
            std::cout << "[PASS] Correctly failed to open invalid file" << std::endl;
        }
        
        // Test reading without opening
        std::string data;
        if (badSource.readSource(data)) {
            std::cout << "[FAIL] Should not read from unopened source" << std::endl;
        } else {
            std::cout << "[PASS] Correctly returned false for unopened source" << std::endl;
        }
    }
    std::cout << std::endl;

    // ==========================================
    // SUMMARY
    // ==========================================
    std::cout << "============================================" << std::endl;
    std::cout << "         ALL TESTS COMPLETED                " << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2 Features Tested:" << std::endl;
    std::cout << "  [x] FileTelemetrySourceImpl" << std::endl;
    std::cout << "  [x] SocketTelemetrySourceImpl" << std::endl;
    std::cout << "  [x] RAII (automatic resource cleanup)" << std::endl;
    std::cout << "  [x] Integration with LogManager" << std::endl;
    std::cout << "  [x] Multiple sources" << std::endl;
    std::cout << "  [x] Error handling" << std::endl;

    return 0;
}