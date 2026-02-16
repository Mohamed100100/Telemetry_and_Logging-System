/**
 * @file phase6_demo.cpp
 * @brief Phase 6 Demo - Simple Telemetry Application
 */

#include "app/TelemetryApp.hpp"
#include <iostream>
#include <fstream>

using namespace telemetry;

int main(int argc, char* argv[]) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════╗" << std::endl;
    std::cout << "║      PHASE 6 DEMO: TelemetryApp       ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    std::string configPath;

    if (argc == 2) {
        configPath = argv[1];
        std::cout << "[Demo] Using config: " << configPath << std::endl;
    } else {
        std::cerr << "[Demo] No config file provided, using default 'config.json'\n";
        std::cerr << "[Demo] Usage: " << argv[0] << " <config_file.json>\n";
        return 1;
    }

    try {
        // THE ENTIRE APPLICATION:
        TelemetryApp app(configPath);
        app.start();        // This will block until Ctrl+C or termination signal
        
    } catch (const std::exception& e) {
        std::cerr << "[Demo] Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "[Demo] Done!" << std::endl;
    return 0;
}