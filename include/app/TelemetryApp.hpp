#pragma once

#include "app/AppConfig.hpp"
#include "logger/LogManager.hpp"
#include "logger/LogMessage.hpp"
#include "sources/ITelemetrySource.hpp"
#include "sinks/ILogSink.hpp"
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"

#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>

namespace telemetry {

struct SourceEntry {
    std::unique_ptr<ITelemetrySource> source;
    TelemetryType type;
    uint32_t rateMs;
    std::chrono::steady_clock::time_point lastRead;
    std::string name;
};

class TelemetryApp {
public:
    explicit TelemetryApp(const std::string& configPath);
    explicit TelemetryApp(const AppConfig& config);
    ~TelemetryApp();

    TelemetryApp(const TelemetryApp&) = delete;
    TelemetryApp& operator=(const TelemetryApp&) = delete;

    void start();
    void stop();
    bool isRunning() const;

private:
    void initialize();
    void createSources();
    void createSinks();
    void openSources();
    void mainLoop();
    void processSource(SourceEntry& entry);
    std::optional<LogMessage> formatData(const std::string& data, TelemetryType type);
    void printBanner();

private:
    AppConfig config_;

    std::unique_ptr<LogFormatter<CpuPolicy>> cpuFormatter_;
    std::unique_ptr<LogFormatter<GpuPolicy>> gpuFormatter_;
    std::unique_ptr<LogFormatter<RamPolicy>> ramFormatter_;

    // LogManager owns the sinks - we just keep count
    std::unique_ptr<LogManager> logManager_;
    size_t sinkCount_ = 0;
    
    std::vector<SourceEntry> sources_;

    std::atomic<bool> running_{false};
};

} // namespace telemetry