#include "app/TelemetryApp.hpp"
#include "sources/FileTelemetrySourceImpl.hpp"
#include "sources/SocketTelemetrySourceImpl.hpp"
#include "sinks/ConsoleSinkImpl.hpp"
#include "sinks/FileSinkImpl.hpp"

#include <iostream>
#include <csignal>

#ifdef SOMEIP_ENABLED
#include "sources/SomeIPTelemetrySourceAdapter.hpp"
#endif

namespace telemetry {

static volatile std::sig_atomic_t g_stopRequested = 0;

void handleSignal(int sig) {
    (void)sig;
    g_stopRequested = 1;
}

TelemetryApp::TelemetryApp(const std::string& configPath) 
    : running_(false) {
    config_ = loadConfig(configPath);
    initialize();
}

TelemetryApp::TelemetryApp(const AppConfig& config) 
    : config_(config), running_(false) {
    initialize();
}

TelemetryApp::~TelemetryApp() {
    std::cout << "[App] Cleaning up..." << std::endl;
    
    // 1. Stop the main loop
    running_.store(false);
    g_stopRequested = 1;
    
    // 2. Wait for mainLoop to exit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 3. Clear sources
    sources_.clear();
    
    // 4. Clear formatters
    cpuFormatter_.reset();
    gpuFormatter_.reset();
    ramFormatter_.reset();
    
    // 5. Destroy LogManager - it owns and will delete the sinks
    logManager_.reset();
    
    std::cout << "[App] Cleanup complete" << std::endl;
}

void TelemetryApp::initialize() {
    std::cout << "[App] Initializing..." << std::endl;

    // create the log formatters with the names(GPU,CPU,RAM) from json file
    cpuFormatter_ = std::make_unique<LogFormatter<CpuPolicy>>(config_.appName);
    gpuFormatter_ = std::make_unique<LogFormatter<GpuPolicy>>(config_.appName);
    ramFormatter_ = std::make_unique<LogFormatter<RamPolicy>>(config_.appName);

    // Create LogManager first
    // 100 message buffer, 4 threads for parallel sink writing
    logManager_ = std::make_unique<LogManager>(100, 4);
    
    // Create sinks and add to LogManager (LogManager takes ownership)
    createSinks();

    // Create sources
    createSources();

    std::cout << "[App] Initialized" << std::endl;
}

// TelemetryApp.cpp
void TelemetryApp::createSinks() {
    for (auto& sinkCfg : config_.sinks) {
        ILogSink* sink = nullptr;
        
        switch (sinkCfg.sinkType) {
            case SinkType::CONSOLE:
                sink = new ConsoleSinkImpl();
                break;
            case SinkType::FILE:
                sink = new FileSinkImpl(sinkCfg.path);
                break;
        }
        
        if (sink) {
            logManager_->addSink(sink);  // LogManager takes ownership
            sinkCount_++;
        }
    }
}

void TelemetryApp::createSources() {
    for (auto& srcCfg : config_.sources) {
        SourceEntry entry;
        entry.type = srcCfg.telemetryType;
        entry.rateMs = srcCfg.rateMs;
        entry.lastRead = std::chrono::steady_clock::now();

        switch (srcCfg.sourceType) {
            case SourceType::FILE:
                entry.source = std::make_unique<FileTelemetrySourceImpl>(srcCfg.path);
                entry.name = "File[" + srcCfg.path + "]";
                std::cout << "[App] + File source: " << srcCfg.path << std::endl;
                break;

            case SourceType::SOCKET:
                entry.source = std::make_unique<SocketTelemetrySourceImpl>(srcCfg.path);
                entry.name = "Socket[" + srcCfg.path + "]";
                std::cout << "[App] + Socket source: " << srcCfg.path << std::endl;
                break;

            case SourceType::SOMEIP:
#ifdef SOMEIP_ENABLED
                entry.source = std::make_unique<SomeIPTelemetrySourceAdapter>();
                entry.name = "SOME/IP";
                std::cout << "[App] + SOME/IP source" << std::endl;
#else
                std::cout << "[App] ! SOME/IP not enabled, skipping" << std::endl;
                continue;
#endif
                break;
        }

        sources_.push_back(std::move(entry));
    }

    std::cout << "[App] Total sources: " << sources_.size() << std::endl;
}

void TelemetryApp::openSources() {
    for (auto& entry : sources_) {
        if (entry.source && entry.source->openSource()) {
            std::cout << "[App] ✓ Opened: " << entry.name << std::endl;
        } else {
            std::cout << "[App] ✗ Failed: " << entry.name << std::endl;
        }
    }
}

void TelemetryApp::start() {
    if (running_.load()) return;

    printBanner();
    
    g_stopRequested = 0;
    
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    openSources();

    running_.store(true);

    std::cout << "[App] Running... (Ctrl+C to stop)" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    mainLoop();
    
    std::cout << "\n[App] Stopped" << std::endl;
}

void TelemetryApp::stop() {
    running_.store(false);
    g_stopRequested = 1;
}

bool TelemetryApp::isRunning() const {
    return running_.load() && (g_stopRequested == 0);
}

void TelemetryApp::mainLoop() {
    while (running_.load() && g_stopRequested == 0) {
        auto now = std::chrono::steady_clock::now();

        for (auto& entry : sources_) {
            if (!running_.load() || g_stopRequested != 0) break;
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - entry.lastRead).count();

            if (elapsed >= entry.rateMs) {
                processSource(entry);
                entry.lastRead = now;
            }
        }

        for (int i = 0; i < 5 && running_.load() && g_stopRequested == 0; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
    
    running_.store(false);
}

void TelemetryApp::processSource(SourceEntry& entry) {
    if (!running_.load() || g_stopRequested != 0) return;
    if (!entry.source) return;
    
    std::string data;
    
    if (entry.source->readSource(data) && !data.empty()) {
        auto msg = formatData(data, entry.type);
        if (msg.has_value() && running_.load() && logManager_) {
            logManager_->log(msg.value());
        }
    }
}

std::optional<LogMessage> TelemetryApp::formatData(const std::string& data, TelemetryType type) {
    if (data.empty()) {
        return std::nullopt;
    }
    
    try {
        switch (type) {
            case TelemetryType::CPU:
                return cpuFormatter_->formatDataToLogMsg(data);
            case TelemetryType::GPU:
                return gpuFormatter_->formatDataToLogMsg(data);
            case TelemetryType::RAM:
                return ramFormatter_->formatDataToLogMsg(data);
        }
    } catch (...) {
        // Ignore format errors
    }
    return std::nullopt;
}

void TelemetryApp::printBanner() {
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  " << config_.appName << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Sources: " << sources_.size() << std::endl;
    std::cout << "  Sinks:   " << sinkCount_ << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

} // namespace telemetry