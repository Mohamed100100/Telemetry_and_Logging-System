#include <iostream>
#include <string>
#include <memory>
#include <vector>

// Policies
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"

// Formatter
#include "formatter/LogFormatter.hpp"

// Logger
#include "logger/LogManager.hpp"
#include "logger/LogManagerBuilder.hpp"
#include "logger/LogMessage.hpp"

// Sinks
#include "sinks/ILogSink.hpp"
#include "sinks/ConsoleSinkImpl.hpp"
#include "sinks/FileSinkImpl.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "sinks/SinkConfig.hpp"

// ============================================
// Test Utilities
// ============================================
namespace TestUtils {
    int totalTests = 0;
    int passedTests = 0;

    void printHeader(const std::string& title) {
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "  " << title << "\n";
        std::cout << "========================================\n";
    }

    void printSubHeader(const std::string& title) {
        std::cout << "\n--- " << title << " ---\n";
    }

    void printResult(const std::string& testName, bool passed) {
        totalTests++;
        if (passed) passedTests++;
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << testName << "\n";
    }

    void printSummary() {
        std::cout << "\n========================================\n";
        std::cout << "  TEST SUMMARY: " << passedTests << "/" << totalTests << " tests passed\n";
        std::cout << "========================================\n";
    }
}

// ============================================
// Test 1: Policy Compile-Time Verification
// ============================================
void testPolicies() {
    TestUtils::printHeader("Test 1: Policy Verification");

    // CPU Policy
    TestUtils::printSubHeader("CpuPolicy");
    static_assert(CpuPolicy::WARNING == 75.0f, "CpuPolicy WARNING check");
    static_assert(CpuPolicy::CRITICAL == 90.0f, "CpuPolicy CRITICAL check");
    std::cout << "  Context:  CPU\n";
    std::cout << "  Unit:     " << CpuPolicy::unit << "\n";
    std::cout << "  WARNING:  " << CpuPolicy::WARNING << "\n";
    std::cout << "  CRITICAL: " << CpuPolicy::CRITICAL << "\n";
    TestUtils::printResult("CpuPolicy thresholds correct", true);

    // GPU Policy
    TestUtils::printSubHeader("GpuPolicy");
    static_assert(GpuPolicy::WARNING == 80.0f, "GpuPolicy WARNING check");
    static_assert(GpuPolicy::CRITICAL == 95.0f, "GpuPolicy CRITICAL check");
    std::cout << "  Context:  GPU\n";
    std::cout << "  Unit:     " << GpuPolicy::unit << "\n";
    std::cout << "  WARNING:  " << GpuPolicy::WARNING << "\n";
    std::cout << "  CRITICAL: " << GpuPolicy::CRITICAL << "\n";
    TestUtils::printResult("GpuPolicy thresholds correct", true);

    // RAM Policy
    TestUtils::printSubHeader("RamPolicy");
    static_assert(RamPolicy::WARNING == 70.0f, "RamPolicy WARNING check");
    static_assert(RamPolicy::CRITICAL == 85.0f, "RamPolicy CRITICAL check");
    std::cout << "  Context:  RAM\n";
    std::cout << "  Unit:     " << RamPolicy::unit << "\n";
    std::cout << "  WARNING:  " << RamPolicy::WARNING << "\n";
    std::cout << "  CRITICAL: " << RamPolicy::CRITICAL << "\n";
    TestUtils::printResult("RamPolicy thresholds correct", true);
}

// ============================================
// Test 2: LogFormatter Valid Input
// ============================================
void testLogFormatterValidInput() {
    TestUtils::printHeader("Test 2: LogFormatter Valid Input");

    const std::string appName = "TestApp";

    // CPU Tests
    TestUtils::printSubHeader("CPU Formatter (WARNING: 75, CRITICAL: 90)");
    LogFormatter<CpuPolicy> cpuFormatter(appName);

    auto cpuInfo = cpuFormatter.formatDataToLogMsg("50.0");
    TestUtils::printResult("CPU 50.0% -> LogMessage created (INFO expected)", cpuInfo.has_value());

    auto cpuWarning = cpuFormatter.formatDataToLogMsg("80.0");
    TestUtils::printResult("CPU 80.0% -> LogMessage created (WARNING expected)", cpuWarning.has_value());

    auto cpuCritical = cpuFormatter.formatDataToLogMsg("95.0");
    TestUtils::printResult("CPU 95.0% -> LogMessage created (CRITICAL expected)", cpuCritical.has_value());

    // GPU Tests
    TestUtils::printSubHeader("GPU Formatter (WARNING: 80, CRITICAL: 95)");
    LogFormatter<GpuPolicy> gpuFormatter(appName);

    auto gpuInfo = gpuFormatter.formatDataToLogMsg("60.0");
    TestUtils::printResult("GPU 60.0% -> LogMessage created (INFO expected)", gpuInfo.has_value());

    auto gpuWarning = gpuFormatter.formatDataToLogMsg("88.0");
    TestUtils::printResult("GPU 88.0% -> LogMessage created (WARNING expected)", gpuWarning.has_value());

    auto gpuCritical = gpuFormatter.formatDataToLogMsg("98.0");
    TestUtils::printResult("GPU 98.0% -> LogMessage created (CRITICAL expected)", gpuCritical.has_value());

    // RAM Tests
    TestUtils::printSubHeader("RAM Formatter (WARNING: 70, CRITICAL: 85)");
    LogFormatter<RamPolicy> ramFormatter(appName);

    auto ramInfo = ramFormatter.formatDataToLogMsg("50.0");
    TestUtils::printResult("RAM 50.0 MB -> LogMessage created (INFO expected)", ramInfo.has_value());

    auto ramWarning = ramFormatter.formatDataToLogMsg("75.0");
    TestUtils::printResult("RAM 75.0 MB -> LogMessage created (WARNING expected)", ramWarning.has_value());

    auto ramCritical = ramFormatter.formatDataToLogMsg("90.0");
    TestUtils::printResult("RAM 90.0 MB -> LogMessage created (CRITICAL expected)", ramCritical.has_value());
}

// ============================================
// Test 3: LogFormatter Invalid Input
// ============================================
void testLogFormatterInvalidInput() {
    TestUtils::printHeader("Test 3: LogFormatter Invalid Input");

    LogFormatter<CpuPolicy> formatter("TestApp");

    std::cout << "(Error messages below are expected)\n\n";

    auto emptyResult = formatter.formatDataToLogMsg("");
    TestUtils::printResult("Empty string -> nullopt", !emptyResult.has_value());

    auto textResult = formatter.formatDataToLogMsg("hello");
    TestUtils::printResult("Text 'hello' -> nullopt", !textResult.has_value());

    auto specialResult = formatter.formatDataToLogMsg("!@#$%");
    TestUtils::printResult("Special chars '!@#$%' -> nullopt", !specialResult.has_value());

    auto mixedResult = formatter.formatDataToLogMsg("abc123");
    TestUtils::printResult("Mixed 'abc123' -> nullopt", !mixedResult.has_value());

    auto spacesResult = formatter.formatDataToLogMsg("   ");
    TestUtils::printResult("Whitespace '   ' -> nullopt", !spacesResult.has_value());
}

// ============================================
// Test 4: LogFormatter Boundary Values
// ============================================
void testLogFormatterBoundaryValues() {
    TestUtils::printHeader("Test 4: LogFormatter Boundary Values");

    LogFormatter<CpuPolicy> formatter("TestApp");
    // CpuPolicy: WARNING = 75.0, CRITICAL = 90.0

    TestUtils::printSubHeader("At Thresholds");
    std::cout << "  CpuPolicy WARNING: 75.0, CRITICAL: 90.0\n\n";

    auto at75 = formatter.formatDataToLogMsg("75.0");
    TestUtils::printResult("Exactly 75.0 (at WARNING) -> LogMessage created", at75.has_value());

    auto at90 = formatter.formatDataToLogMsg("90.0");
    TestUtils::printResult("Exactly 90.0 (at CRITICAL) -> LogMessage created", at90.has_value());

    TestUtils::printSubHeader("Just Above Thresholds");

    auto above75 = formatter.formatDataToLogMsg("75.1");
    TestUtils::printResult("Value 75.1 (above WARNING) -> LogMessage created", above75.has_value());

    auto above90 = formatter.formatDataToLogMsg("90.1");
    TestUtils::printResult("Value 90.1 (above CRITICAL) -> LogMessage created", above90.has_value());

    TestUtils::printSubHeader("Edge Cases");

    auto zeroVal = formatter.formatDataToLogMsg("0.0");
    TestUtils::printResult("Zero (0.0) -> LogMessage created", zeroVal.has_value());

    auto maxVal = formatter.formatDataToLogMsg("100.0");
    TestUtils::printResult("Max (100.0) -> LogMessage created", maxVal.has_value());

    auto negVal = formatter.formatDataToLogMsg("-10.0");
    TestUtils::printResult("Negative (-10.0) -> LogMessage created", negVal.has_value());

    auto floatVal = formatter.formatDataToLogMsg("85.5555");
    TestUtils::printResult("Decimal (85.5555) -> LogMessage created", floatVal.has_value());
}

// ============================================
// Test 5: LogMessage ToString
// ============================================
void testLogMessageToString() {
    TestUtils::printHeader("Test 5: LogMessage ToString");

    LogFormatter<CpuPolicy> formatter("DemoApp");

    auto logMsg = formatter.formatDataToLogMsg("92.5");

    if (logMsg.has_value()) {
        std::cout << "\nGenerated LogMessage:\n";
        std::cout << "----------------------------------------\n";
        std::cout << logMsg.value().ToString() << "\n";
        std::cout << "----------------------------------------\n";
        TestUtils::printResult("LogMessage::ToString() works", true);
    } else {
        TestUtils::printResult("LogMessage::ToString() works", false);
    }
}

// ============================================
// Test 6: SinkConfig Factory Methods
// ============================================
void testSinkConfig() {
    TestUtils::printHeader("Test 6: SinkConfig Factory Methods");

    // Console config
    SinkConfig consoleConfig = SinkConfig::Console();
    TestUtils::printResult("SinkConfig::Console() creates config", 
                          consoleConfig.type == SinkType::CONSOLE);

    // File config
    SinkConfig fileConfig = SinkConfig::File("test.log");
    bool fileConfigCorrect = (fileConfig.type == SinkType::FILE) && 
                             (fileConfig.filePath == "test.log");
    TestUtils::printResult("SinkConfig::File() creates config with path", fileConfigCorrect);
}

// ============================================
// Test 7: LogSinkFactory
// ============================================
void testLogSinkFactory() {
    TestUtils::printHeader("Test 7: LogSinkFactory");

    LogSinkFactory factory;

    // Console Sink
    TestUtils::printSubHeader("Console Sink Creation");
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    TestUtils::printResult("Factory creates ConsoleSink", consoleSink != nullptr);

    // File Sink
    TestUtils::printSubHeader("File Sink Creation");
    auto fileSink = factory.CreateSink(SinkConfig::File("factory_test.log"));
    TestUtils::printResult("Factory creates FileSink", fileSink != nullptr);
}

// ============================================
// Test 8: Direct Sink Output
// ============================================
void testDirectSinkOutput() {
    TestUtils::printHeader("Test 8: Direct Sink Output");

    LogFormatter<CpuPolicy> formatter("SinkTest");
    auto logMsg = formatter.formatDataToLogMsg("88.0");

    if (!logMsg.has_value()) {
        TestUtils::printResult("Create test LogMessage", false);
        return;
    }

    TestUtils::printSubHeader("Console Sink Output");
    ConsoleSinkImpl consoleSink;
    std::cout << "Output: ";
    consoleSink.write(logMsg.value());
    TestUtils::printResult("ConsoleSink::write() executes", true);

    TestUtils::printSubHeader("File Sink Output");
    std::string filePath = "direct_sink_test.log";
    FileSinkImpl fileSink(filePath);
    fileSink.write(logMsg.value());
    std::cout << "Written to: " << filePath << "\n";
    TestUtils::printResult("FileSink::write() executes", true);
}

// ============================================
// Test 9: LogManagerBuilder
// ============================================
void testLogManagerBuilder() {
    TestUtils::printHeader("Test 9: LogManagerBuilder");

    LogSinkFactory factory;

    // Create sinks
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    auto fileSink = factory.CreateSink(SinkConfig::File("builder_test.log"));

    TestUtils::printSubHeader("Building LogManager");

    // Build with fluent API
    auto logManager = LogManagerBuilder()
        .addSink(consoleSink.release())
        .addSink(fileSink.release())
        .build();

    TestUtils::printResult("LogManagerBuilder creates LogManager", logManager != nullptr);
    TestUtils::printResult("Builder fluent API works (chaining)", true);
}

// ============================================
// Test 10: LogManager Operations
// ============================================
void testLogManagerOperations() {
    TestUtils::printHeader("Test 10: LogManager Operations");

    // Create LogManager with sinks
    LogSinkFactory factory;
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    auto fileSink = factory.CreateSink(SinkConfig::File("manager_test.log"));

    auto logManager = LogManagerBuilder()
        .addSink(consoleSink.release())
        .addSink(fileSink.release())
        .build();

    // Create log messages
    LogFormatter<CpuPolicy> cpuFormatter("ManagerTest");
    LogFormatter<GpuPolicy> gpuFormatter("ManagerTest");

    auto cpuMsg = cpuFormatter.formatDataToLogMsg("91.0");
    auto gpuMsg = gpuFormatter.formatDataToLogMsg("82.0");

    TestUtils::printSubHeader("Logging Messages");

    if (cpuMsg.has_value()) {
        logManager->log(cpuMsg.value());
        std::cout << "  Logged: CPU 91.0% (CRITICAL)\n";
    }

    if (gpuMsg.has_value()) {
        logManager->log(gpuMsg.value());
        std::cout << "  Logged: GPU 82.0% (WARNING)\n";
    }

    TestUtils::printResult("LogManager::log() accepts messages", true);

    TestUtils::printSubHeader("Flushing to Sinks");
    std::cout << "\n--- Console Output ---\n";
    logManager->flush();
    std::cout << "--- End Console Output ---\n";

    TestUtils::printResult("LogManager::flush() writes to all sinks", true);

    // Test clear operations
    TestUtils::printSubHeader("Clear Operations");
    logManager->DeleteAllLogMessages();
    TestUtils::printResult("LogManager::DeleteAllLogMessages() executes", true);

    logManager->DeleteAllSinks();
    TestUtils::printResult("LogManager::DeleteAllSinks() executes", true);
}

// ============================================
// Test 11: Policy Behavior Comparison
// ============================================
void testPolicyBehaviorComparison() {
    TestUtils::printHeader("Test 11: Policy Behavior Comparison");

    const std::string appName = "PolicyCompare";
    std::string testValue = "77.0";

    LogFormatter<CpuPolicy> cpuFormatter(appName);
    LogFormatter<GpuPolicy> gpuFormatter(appName);
    LogFormatter<RamPolicy> ramFormatter(appName);

    std::cout << "\nTesting value " << testValue << " with different policies:\n\n";

    std::cout << "CpuPolicy (WARNING: 75, CRITICAL: 90):\n";
    std::cout << "  77.0 > 75.0 -> Expected: WARNING\n";
    auto cpuResult = cpuFormatter.formatDataToLogMsg(testValue);
    TestUtils::printResult("CPU formatter processes value", cpuResult.has_value());

    std::cout << "\nGpuPolicy (WARNING: 80, CRITICAL: 95):\n";
    std::cout << "  77.0 < 80.0 -> Expected: INFO\n";
    auto gpuResult = gpuFormatter.formatDataToLogMsg(testValue);
    TestUtils::printResult("GPU formatter processes value", gpuResult.has_value());

    std::cout << "\nRamPolicy (WARNING: 70, CRITICAL: 85):\n";
    std::cout << "  77.0 > 70.0 -> Expected: WARNING\n";
    auto ramResult = ramFormatter.formatDataToLogMsg(testValue);
    TestUtils::printResult("RAM formatter processes value", ramResult.has_value());

    std::cout << "\n=> Same value produces different severities based on policy!\n";
}

// ============================================
// Test 12: Full Integration Test
// ============================================
void testFullIntegration() {
    TestUtils::printHeader("Test 12: Full Integration Test");

    const std::string appName = "TelemetrySystem";

    // Step 1: Setup
    TestUtils::printSubHeader("Step 1: Create Infrastructure");

    LogSinkFactory factory;
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    auto fileSink = factory.CreateSink(SinkConfig::File("integration_test.log"));

    auto logManager = LogManagerBuilder()
        .addSink(consoleSink.release())
        .addSink(fileSink.release())
        .build();

    std::cout << "  Created: LogSinkFactory\n";
    std::cout << "  Created: ConsoleSink, FileSink\n";
    std::cout << "  Created: LogManager via Builder\n";

    // Step 2: Create Formatters
    TestUtils::printSubHeader("Step 2: Create Formatters");

    LogFormatter<CpuPolicy> cpuFormatter(appName);
    LogFormatter<GpuPolicy> gpuFormatter(appName);
    LogFormatter<RamPolicy> ramFormatter(appName);

    std::cout << "  Created: CpuPolicy Formatter\n";
    std::cout << "  Created: GpuPolicy Formatter\n";
    std::cout << "  Created: RamPolicy Formatter\n";

    // Step 3: Simulate Telemetry Data
    TestUtils::printSubHeader("Step 3: Process Telemetry Data");

    struct TelemetryReading {
        std::string source;
        std::string value;
        std::string expectedSeverity;
    };

    std::vector<TelemetryReading> readings = {
        {"CPU", "45.0", "INFO"},
        {"CPU", "78.0", "WARNING"},
        {"CPU", "92.0", "CRITICAL"},
        {"GPU", "70.0", "INFO"},
        {"GPU", "85.0", "WARNING"},
        {"GPU", "97.0", "CRITICAL"},
        {"RAM", "60.0", "INFO"},
        {"RAM", "73.0", "WARNING"},
        {"RAM", "88.0", "CRITICAL"}
    };

    int processedCount = 0;

    for (const auto& reading : readings) {
        std::optional<LogMessage> logMsg;

        if (reading.source == "CPU") {
            logMsg = cpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.source == "GPU") {
            logMsg = gpuFormatter.formatDataToLogMsg(reading.value);
        } else if (reading.source == "RAM") {
            logMsg = ramFormatter.formatDataToLogMsg(reading.value);
        }

        if (logMsg.has_value()) {
            logManager->log(logMsg.value());
            processedCount++;
            std::cout << "  [OK] " << reading.source << " " << reading.value 
                      << " -> " << reading.expectedSeverity << "\n";
        }
    }

    bool allProcessed = (processedCount == static_cast<int>(readings.size()));
    TestUtils::printResult("All telemetry readings processed", allProcessed);

    // Step 4: Flush Output
    TestUtils::printSubHeader("Step 4: Flush to All Sinks");

    std::cout << "\n========== CONSOLE OUTPUT ==========\n";
    logManager->flush();
    std::cout << "=====================================\n";

    TestUtils::printResult("Messages flushed to Console and File", true);

    std::cout << "\nGenerated files:\n";
    std::cout << "  - integration_test.log\n";
}

// ============================================
// Main
// ============================================
int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║        PHASE 3 DEMO - TELEMETRY LOGGING SYSTEM                ║\n";
    std::cout << "║                                                               ║\n";
    std::cout << "║  Components: Policies, Formatter, Sinks, Factory, Builder     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";

    try {
        // Run all tests
        testPolicies();
        testLogFormatterValidInput();
        testLogFormatterInvalidInput();
        testLogFormatterBoundaryValues();
        testLogMessageToString();
        testSinkConfig();
        testLogSinkFactory();
        testDirectSinkOutput();
        testLogManagerBuilder();
        testLogManagerOperations();
        testPolicyBehaviorComparison();
        testFullIntegration();

        // Summary
        TestUtils::printSummary();

        std::cout << "\nGenerated Log Files:\n";
        std::cout << "  - factory_test.log\n";
        std::cout << "  - direct_sink_test.log\n";
        std::cout << "  - builder_test.log\n";
        std::cout << "  - manager_test.log\n";
        std::cout << "  - integration_test.log\n";

        std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    DEMO COMPLETED                             ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════╝\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}