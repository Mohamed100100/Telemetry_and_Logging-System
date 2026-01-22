
# 📖 README - Phase 3: Policy-Based Logging System

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Components](#components)
4. [Design Patterns Used](#design-patterns-used)
5. [Class Diagram](#class-diagram)
6. [File Structure](#file-structure)
7. [API Reference](#api-reference)
8. [Usage Examples](#usage-examples)
9. [Building the Project](#building-the-project)
10. [Testing](#testing)

---

## Overview

Phase 3 implements a **policy-based logging system** for telemetry data. The system processes raw telemetry readings (CPU, GPU, RAM usage), classifies them by severity based on configurable thresholds, and outputs formatted log messages to multiple destinations (console, file).

### Key Features

- ✅ **Policy-Based Design**: Compile-time configuration of thresholds per telemetry type
- ✅ **Factory Pattern**: Flexible sink creation without exposing concrete classes
- ✅ **Builder Pattern**: Fluent API for constructing LogManager with multiple sinks
- ✅ **RAII**: Safe resource management for files and sockets
- ✅ **Template Metaprogramming**: Type-safe formatter using policy classes
- ✅ **Extensible**: Easy to add new telemetry sources, policies, and sinks

---

## Architecture

```
┌────────────────────────────────────────────────────────────────────────────┐
│                        TELEMETRY LOGGING SYSTEM                            │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌──────────────┐     ┌──────────────────┐     ┌──────────────────────┐    │
│  │  Telemetry   │     │   LogFormatter   │     │     LogManager       │    │
│  │   Sources    │────▶│    <Policy>      │────▶│                      │    │
│  │              │     │                  │     │  ┌────────────────┐  │    │
│  │ • File       │     │ • CpuPolicy      │     │  │ SinksBuffer    │  │    │
│  │ • Socket     │     │ • GpuPolicy      │     │  ├────────────────┤  │    │
│  └──────────────┘     │ • RamPolicy      │     │  │ ConsoleSink    │  │    │
│                       └──────────────────┘     │  │ FileSink       │  │    │
│                               │                │  └────────────────┘  │    │
│                               │                │                      │    │
│                               ▼                │  ┌────────────────┐  │    │
│                       ┌──────────────────┐     │  │ LogMessages    │  │    │
│                       │   LogMessage     │────▶│  │ Buffer         │  │    │
│                       │                  │     │  └────────────────┘  │    │
│                       │ • appName        │     │                      │    │
│                       │ • context        │     │  flush() ────────────┼────┼──▶ Output
│                       │ • severity       │     │                      │    │
│                       │ • timestamp      │     └──────────────────────┘    │
│                       │ • message        │                                 │
│                       └──────────────────┘                                 │
│                                                                            │
└────────────────────────────────────────────────────────────────────────────┘
```

---

## Components

### 1. Policy Classes

Policy classes define thresholds and metadata for each telemetry type.

| Policy | Context | Unit | WARNING | CRITICAL |
|--------|---------|------|---------|----------|
| `CpuPolicy` | CPU | % | 75.0 | 90.0 |
| `GpuPolicy` | GPU | % | 80.0 | 95.0 |
| `RamPolicy` | RAM | MB | 70.0 | 85.0 |

### 2. LogFormatter\<Policy\>

Template class that converts raw telemetry strings to `LogMessage` objects using policy-defined thresholds.

### 3. LogMessage

Data class containing:
- `appName`: Application identifier
- `context`: Telemetry source (CPU/GPU/RAM)
- `severity`: CRITICAL/WARNING/INFO
- `time`: Timestamp
- `message`: Formatted description

### 4. Sinks

| Sink | Description |
|------|-------------|
| `ConsoleSinkImpl` | Outputs to `std::cout` |
| `FileSinkImpl` | Appends to specified file |

### 5. LogManager

Manages multiple sinks and log message buffering. Supports:
- Adding/removing sinks
- Buffering log messages
- Flushing all messages to all sinks

### 6. LogManagerBuilder

Fluent builder for constructing `LogManager` instances.

### 7. LogSinkFactory

Factory for creating sink instances based on `SinkConfig`.

---

## Design Patterns Used

### 1. Policy-Based Design (Template Pattern)

```cpp
template <typename _PolicyType>
class LogFormatter {
    // Uses _PolicyType::WARNING, _PolicyType::CRITICAL, etc.
};

// Usage
LogFormatter<CpuPolicy> cpuFormatter("App");  // Compile-time configuration
```

### 2. Factory Pattern

```cpp
LogSinkFactory factory;
auto sink = factory.CreateSink(SinkConfig::Console());
```

### 3. Builder Pattern

```cpp
auto logManager = LogManagerBuilder()
    .addSink(consoleSink)
    .addSink(fileSink)
    .build();
```

### 4. RAII (Resource Acquisition Is Initialization)

```cpp
class SafeFile {
    int fd;
public:
    SafeFile(std::string& path);  // Acquires resource
    ~SafeFile();                   // Releases resource
};
```

---

## Class Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              ENUMS                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  TelemetrySrc_enum          SinkType              SeverityLvl_enum          │
│  ├── CPU                    ├── CONSOLE           ├── CRITICAL              │
│  ├── GPU                    └── FILE              ├── WARNING               │
│  └── RAM                                          └── INFO                  │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                            POLICIES                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐              │
│  │   CpuPolicy     │  │   GpuPolicy     │  │   RamPolicy     │              │
│  ├─────────────────┤  ├─────────────────┤  ├─────────────────┤              │
│  │ context : CPU   │  │ context : GPU   │  │ context : RAM   │              │
│  │ unit    : "%"   │  │ unit    : "%"   |  │ unit    : "MB"  │              │
│  │ WARNING : 75.0  │  │ WARNING : 80.0  │  │ WARNING : 70.0  │              │
│  │ CRITICAL: 90.0  │  │ CRITICAL: 95.0  │  │ CRITICAL: 85.0  │              │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                           FORMATTER                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌───────────────────────────────┐    ┌─────────────────────────────────┐   │
│  │   LogFormatter<PolicyType>    │    │    LogFormatterHelper           │   │
│  ├───────────────────────────────┤    ├─────────────────────────────────┤   │
│  │ - AppName: string             │    │ + GetDescription(): string      │   │
│  ├───────────────────────────────┤    │ + GetSeverity(): string         │   │
│  │ - GetContext(): string        │    │ + GetCurrentTimeStamp(): string │   │
│  │ + formatDataToLogMsg():       │    └─────────────────────────────────┘   │
│  │     optional<LogMessage>      │                                          │
│  └───────────────────────────────┘                                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                             SINKS                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│                        ┌─────────────────┐                                  │
│                        │   <<interface>> │                                  │
│                        │    ILogSink     │                                  │
│                        ├─────────────────┤                                  │
│                        │ + write()       │                                  │
│                        └────────┬────────┘                                  │
│                                 │                                           │
│               ┌─────────────────┼─────────────────┐                         │
│               │                 │                 │                         │
│               ▼                 ▼                 ▼                         │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐              │
│  │ ConsoleSinkImpl │  │  FileSinkImpl   │  │  (Future Sinks) │              │
│  ├─────────────────┤  ├─────────────────┤  └─────────────────┘              │
│  │ + write()       │  │ - FilePath      │                                   │
│  └─────────────────┘  │ + write()       │                                   │
│                       └─────────────────┘                                   │
│                                                                             │
│  ┌─────────────────┐  ┌─────────────────┐                                   │
│  │  LogSinkFactory │  │   SinkConfig    │                                   │
│  ├─────────────────┤  ├─────────────────┤                                   │
│  │ + CreateSink()  │  │ + type          │                                   │
│  └─────────────────┘  │ + filePath      │                                   │
│                       │ + Console()     │                                   │
│                       │ + File()        │                                   │
│                       └─────────────────┘                                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                            LOGGER                                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────┐        ┌─────────────────────────┐             │
│  │      LogManager         │        │   LogManagerBuilder     │             │
│  ├─────────────────────────┤        ├─────────────────────────┤             │
│  │ - SinksBuffer           │◀───────│ - PtrToLogManager       │             │
│  │ - LogMessagesBuffer     │        ├─────────────────────────┤             │
│  ├─────────────────────────┤        │ + addSink(): Builder&   │             │
│  │ + addSink()             │        │ + addLogMessage(): ...  │             │
│  │ + removeSink()          │        │ + build(): unique_ptr   │             │
│  │ + log()                 │        └─────────────────────────┘             │
│  │ + flush()               │                                                │
│  └─────────────────────────┘                                                │
│                                                                             │
│  ┌─────────────────────────┐                                                │
│  │      LogMessage         │                                                │
│  ├─────────────────────────┤                                                │
│  │ - appName: string       │                                                │
│  │ - context: string       │                                                │
│  │ - severity: string      │                                                │
│  │ - time: string          │                                                │
│  │ - message: string       │                                                │
│  ├─────────────────────────┤                                                │
│  │ + ToString(): string    │                                                │
│  └─────────────────────────┘                                                │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## File Structure

```
project/
├── include/
│   ├── enums/
│   │   ├── LogSinkType.hpp
│   │   ├── SeverityLevel.hpp
│   │   ├── SinksType.hpp
│   │   └── TelemetrySource.hpp
│   │
│   ├── formatter/
│   |   ├── policies/
│   │   │       ├── CpuPolicy.hpp
│   │   │       ├── GpuPolicy.hpp
|   │   │       └── RamPolicy.hpp
│   │   ├── LogFormatter.hpp
│   │   └── LogFormatterHelper.hpp
│   │
│   ├── logger/
│   │   ├── LogManager.hpp
│   │   ├── LogManagerBuilder.hpp
│   │   └── LogMessage.hpp
│   │
│   ├── sinks/
│   │   ├── ILogSink.hpp
│   │   ├── ConsoleSinkImpl.hpp
│   │   ├── FileSinkImpl.hpp
│   │   ├── LogSinkFactory.hpp
│   │   └── SinkConfig.hpp
│   │
│   ├── sources/
│   │   ├── ITelemetrySource.hpp
│   │   ├── FileTelemetrySourceImpl.hpp
│   │   └── SocketTelemetrySourceImpl.hpp
│   │
│   └── raii/
│       ├── SafeFile.hpp
│       └── SafeSocket.hpp
│
├── src/
│   ├── formatter/
|   |   ├── CMakeLists.txt
│   │   └── LogFormatterHelper.cpp
│   │
│   ├── logger/
|   |   ├── CMakeLists.txt
│   │   ├── LogManager.cpp
│   │   ├── LogManagerBuilder.cpp
│   │   └── LogMessage.cpp
│   │
│   │── raii/
|   |   ├── CMakeLists.txt
│   │   ├── SafeFile.cpp
│   │   └── SafeSocket.cpp
│   └── sinks/
|       ├── CMakeLists.txt
│       ├── ConsoleSinkImpl.cpp
│       ├── FileSinkImpl.cpp
│       ├── LogSinkFactory.cpp
│       └── SinkConfig.cpp
│
├── examples/
|   ├── CMakeLists.txt
|   ├── phase1_demo.cpp
|   ├── phase2_demo.cpp
│   └── phase3_demo.cpp
│
├── third_party/
│   └── magic_enum.hpp
|
├── scripts/
│   └── socket_server.sh
│
└── CMakeLists.txt
```

---

## API Reference

### LogFormatter\<PolicyType\>

```cpp
template <typename PolicyType>
class LogFormatter {
public:
    // Constructor - requires application name
    LogFormatter(const std::string& appName);
    
    // Convert raw telemetry string to LogMessage
    // Returns nullopt if parsing fails
    std::optional<LogMessage> formatDataToLogMsg(const std::string& raw);
};
```

### LogManager

```cpp
class LogManager {
public:
    void addSink(ILogSink* sink);           // Add output destination
    void removeSink(ILogSink* sink);        // Remove output destination
    void log(const LogMessage& message);    // Buffer a log message
    void flush();                           // Write all buffered messages to all sinks
    void DeleteAllSinks();                  // Clear all sinks
    void DeleteAllLogMessages();            // Clear message buffer
};
```

### LogManagerBuilder

```cpp
class LogManagerBuilder {
public:
    LogManagerBuilder& addSink(ILogSink* sink);           // Fluent API
    LogManagerBuilder& addLogMessage(LogMessage& msg);    // Fluent API
    std::unique_ptr<LogManager> build();                  // Create LogManager
};
```

### LogSinkFactory

```cpp
class LogSinkFactory {
public:
    std::unique_ptr<ILogSink> CreateSink(const SinkConfig& config);
};
```

### SinkConfig

```cpp
struct SinkConfig {
    SinkType type;
    std::string filePath;
    
    static SinkConfig Console();                    // Create console config
    static SinkConfig File(const std::string& path); // Create file config
};
```

---

## Usage Examples

### Basic Usage

```cpp
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "logger/LogManagerBuilder.hpp"

int main() {
    // 1. Create formatter
    LogFormatter<CpuPolicy> formatter("MyApp");
    
    // 2. Create sinks
    LogSinkFactory factory;
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    auto fileSink = factory.CreateSink(SinkConfig::File("app.log"));
    
    // 3. Build LogManager
    auto logManager = LogManagerBuilder()
        .addSink(consoleSink.release())
        .addSink(fileSink.release())
        .build();
    
    // 4. Process telemetry
    auto logMsg = formatter.formatDataToLogMsg("85.5");
    if (logMsg.has_value()) {
        logManager->log(logMsg.value());
    }
    
    // 5. Flush to outputs
    logManager->flush();
    
    return 0;
}
```

### Multiple Telemetry Sources

```cpp
LogFormatter<CpuPolicy> cpuFormatter("App");
LogFormatter<GpuPolicy> gpuFormatter("App");
LogFormatter<RamPolicy> ramFormatter("App");

auto cpuLog = cpuFormatter.formatDataToLogMsg("92.0");  // CRITICAL
auto gpuLog = gpuFormatter.formatDataToLogMsg("75.0");  // INFO
auto ramLog = ramFormatter.formatDataToLogMsg("72.0");  // WARNING
```

---

## Building the Project


### Build Commands

```bash
>> cmake -S ./examples -B build
>> cmake --build ./build    
>> ./build/Demo
```

---

## Testing

See `phase3_demo.cpp` for comprehensive test cases covering:

- Policy threshold verification
- Valid/invalid input handling
- Boundary value testing
- Factory pattern testing
- Builder pattern testing
- Full integration testing

---