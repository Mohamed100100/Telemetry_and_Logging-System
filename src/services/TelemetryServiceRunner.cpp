#include "services/TelemetryServiceImpl.hpp"
#include <thread>
#include <iostream>
#include <csignal>

// Include SOME/IP headers - they auto-register on load
#include "src-gen/v1/log/TelemetryServiceSomeIPStubAdapter.hpp"
#include <v1/log/TelemetryServiceSomeIPDeployment.hpp>

volatile bool running = true;

void signalHandler(int signal) {
    std::cout << "\n[Server] Shutting down..." << std::endl;
    running = false;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Telemetry Service Server            " << std::endl;
    std::cout << "========================================" << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    
    if (!runtime) {
        std::cerr << "[Server] Failed to get CommonAPI runtime!" << std::endl;
        return 1;
    }

    std::shared_ptr<TelemetryServiceImpl> serverImpl = std::make_shared<TelemetryServiceImpl>();
    
    bool success = runtime->registerService("local", "TelemetryService", serverImpl);

    if (!success) {
        std::cerr << "[Server] Failed to register service" << std::endl;
        return 1;
    }

    std::cout << "[Server] ✓ Service registered successfully!" << std::endl;
    std::cout << "[Server] Waiting for clients... (Ctrl+C to stop)" << std::endl;

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "[Server] Stopped" << std::endl;
    return 0;
}