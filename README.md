# 📊 Telemetry and Logging System

A modern, high-performance C++17 telemetry monitoring and logging system featuring policy-based design, asynchronous logging, SOME/IP network communication, and a simple façade interface with JSON-based runtime configuration.

---

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)
![Threading](https://img.shields.io/badge/threading-Multi--threaded-orange.svg)
![SOME/IP](https://img.shields.io/badge/SOME%2FIP-vsomeip3-purple.svg)
![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Architecture](#-architecture)
- [Getting Started](#-getting-started)
- [Quick Start](#-quick-start)
- [Configuration](#-configuration)
- [Project Structure](#-project-structure)
- [Design Patterns](#-design-patterns)
- [Components](#-components)
- [Phase Development](#-phase-development)
- [API Reference](#-api-reference)
- [Performance](#-performance)
- [Troubleshooting](#-troubleshooting)
- [Contributing](#-contributing)
- [Author](#-author)
- [Acknowledgments](#-acknowledgments)

---

## 🎯 Overview

The **Telemetry and Logging System** is a comprehensive logging framework designed for monitoring system telemetry data (CPU, GPU, RAM usage). It processes raw telemetry readings from multiple sources (files, sockets, SOME/IP network), classifies them by severity based on configurable thresholds, and outputs formatted log messages to multiple destinations simultaneously and asynchronously.

### Why This Project?

| Aspect | Description |
|--------|-------------|
| **Real-world Application** | Demonstrates practical software engineering for system monitoring |
| **Modern C++17** | Showcases modern C++ features and best practices |
| **Design Patterns** | Implements 8+ industry-standard patterns for maintainability |
| **Concurrency** | Features multi-threaded architecture for high performance |
| **Extensibility** | Easy to add new telemetry sources, policies, and output sinks |
| **Automotive Ready** | Includes SOME/IP middleware integration for automotive applications |
| **Simple API** | Façade pattern provides 2-line usage despite internal complexity |

### The Power of Façade

```cpp
// BEFORE: 50+ lines of setup code
// AFTER: Just 2 lines!

TelemetryApp app("config.json");
app.start();
```

---

## ✨ Features

### Core Features

| Feature | Description |
|---------|-------------|
| 🎛️ **Policy-Based Configuration** | Compile-time threshold configuration per telemetry type |
| 📝 **Multiple Output Sinks** | Console, File, and extensible for Network/Database |
| 🔄 **Thread-Safe Ring Buffer** | Efficient fixed-size circular buffer with mutex protection |
| ⚡ **Asynchronous Logging** | Non-blocking log operations using background threads |
| 🧵 **Thread Pool** | Parallel sink writing for maximum throughput |
| 🌐 **SOME/IP Integration** | Network telemetry via vSOME/IP middleware |
| 🎭 **Façade Pattern** | Simple 2-line API hiding all complexity |
| 📄 **JSON Configuration** | Runtime configuration without recompilation |
| 🔒 **RAII Resource Management** | Safe handling of files, sockets, and system resources |
| 🛑 **Graceful Shutdown** | Signal handling (Ctrl+C) with clean resource cleanup |

### Design Patterns Implemented

| Pattern | Usage |
|---------|-------|
| **Strategy** | Interchangeable sink implementations |
| **Policy-Based Design** | Compile-time threshold configuration |
| **Factory** | Flexible sink creation |
| **Builder** | Fluent LogManager construction |
| **Singleton** | Single SOME/IP client instance |
| **Adapter** | SOME/IP to ITelemetrySource bridge |
| **Façade** | Simple TelemetryApp interface |
| **Producer-Consumer** | Async logging architecture |

### Technical Highlights

- ✅ Modern C++17 standard
- ✅ Template metaprogramming for type safety
- ✅ Smart pointers for memory safety
- ✅ Exception-safe design
- ✅ Header-only policy classes
- ✅ Zero-cost abstractions
- ✅ Efficient circular buffer with O(1) operations
- ✅ Bounded memory usage
- ✅ Graceful shutdown handling
- ✅ Signal-safe stop mechanism

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         TELEMETRY AND LOGGING SYSTEM                            │
│                              Complete Architecture                              │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│  ┌───────────────────────────────────────────────────────────────────────────┐  │
│  │                         FAÇADE LAYER (Phase 6)                            │  │
│  │                                                                           │  │
│  │   ┌─────────────┐         ┌─────────────────────────────────────────┐     │  │
│  │   │ config.json │────────▶│            TelemetryApp                 │     │  │
│  │   └─────────────┘         │                                         │     │  │
│  │                           │  • TelemetryApp(configPath)             │     │  │
│  │                           │  • start()  → runs until Ctrl+C         │     │  │
│  │                           │  • stop()   → graceful shutdown         │     │  │
│  │                           │                                         │     │  │
│  │                           └─────────────────────────────────────────┘     │  │
│  │                                           │                               │  │
│  └───────────────────────────────────────────┼───────────────────────────────┘  │
│                                              │                                  │
│  ┌───────────────────────────────────────────┼───────────────────────────────┐  │
│  │                    DATA SOURCE LAYER (Phase 2 & 5)                        │  │
│  │                                           │                               │  │
│  │   ┌───────────────────────────────────────┼────────────────────────────┐  │  │
│  │   │              ITelemetrySource Interface                            │  │  │
│  │   └───────────────────────────────────────┼────────────────────────────┘  │  │
│  │                    │                      │                     │         │  │
│  │                    ▼                      ▼                     ▼         │  │
│  │   ┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────┐   │  │
│  │   │ FileTelemetrySource │  │SocketTelemetrySource│  │SomeIPTelemetry  │   │  │
│  │   │       Impl          │  │       Impl          │  │  SourceAdapter  │   │  │
│  │   │                     │  │                     │  │                 │   │  │
│  │   │   ┌───────────┐     │  │   ┌───────────┐     │  │  ┌───────────┐  │   │  │
│  │   │   │ SafeFile  │     │  │   │SafeSocket │     │  │  │ Singleton │  │   │  │
│  │   │   │  (RAII)   │     │  │   │  (RAII)   │     │  │  │  Client   │  │   │  │
│  │   │   └───────────┘     │  │   └───────────┘     │  │  └───────────┘  │   │  │
│  │   └─────────────────────┘  └─────────────────────┘  └────────┬────────┘   │  │
│  │                                                              │            │  │
│  │                                                      ┌───────▼────────┐   │  │
│  │                                                      │   vsomeip /    │   │  │
│  │                                                      │   CommonAPI    │   │  │
│  │                                                      └────────────────┘   │  │
│  └───────────────────────────────────────────────────────────────────────────┘  │
│                                              │                                  │
│  ┌───────────────────────────────────────────┼───────────────────────────────┐  │
│  │                    PROCESSING LAYER (Phase 3)                             │  │
│  │                                           │                               │  │
│  │   ┌──────────────────┐    ┌───────────────▼───────────────────────────┐   │  │
│  │   │     Policies     │    │         LogFormatter<Policy>              │   │  │
│  │   │                  │    │                                           │   │  │
│  │   │ ┌──────────────┐ │    │  • Parse raw telemetry data ("45.5")      │   │  │
│  │   │ │  CpuPolicy   │ │───▶│  • Apply policy thresholds                │   │  │
│  │   │ │  WARN: 75%   │ │    │  • Generate severity (INFO/WARN/CRITICAL) │   │  │
│  │   │ │  CRIT: 90%   │ │    │  • Create timestamped LogMessage          │   │  │
│  │   │ └──────────────┘ │    │                                           │   │  │
│  │   │ ┌──────────────┐ │    └───────────────────────────────────────────┘   │  │
│  │   │ │  GpuPolicy   │ │                                                    │  │
│  │   │ │  WARN: 80%   │ │                                                    │  │
│  │   │ │  CRIT: 95%   │ │                                                    │  │
│  │   │ └──────────────┘ │                                                    │  │
│  │   │ ┌──────────────┐ │                                                    │  │
│  │   │ │  RamPolicy   │ │                                                    │  │
│  │   │ │  WARN: 70%   │ │                                                    │  │
│  │   │ │  CRIT: 85%   │ │                                                    │  │
│  │   │ └──────────────┘ │                                                    │  │
│  │   └──────────────────┘                                                    │  │
│  └───────────────────────────────────────────────────────────────────────────┘  │
│                                              │                                  │
│  ┌───────────────────────────────────────────┼───────────────────────────────┐  │
│  │                 ASYNC LOGGING LAYER (Phase 1 & 4)                         │  │
│  │                                           │                               │  │
│  │   ┌───────────────────────────────────────▼───────────────────────────┐   │  │
│  │   │                        LogManager                                 │   │  │
│  │   │                                                                   │   │  │
│  │   │  ┌─────────────────────────────────────────────────────────────┐  │   │  │
│  │   │  │              Thread-Safe Ring Buffer                        │  │   │  │
│  │   │  │  ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐          │  │   │  │
│  │   │  │  │ msg │ msg │ msg │     │     │     │     │     │          │  │   │  │
│  │   │  │  └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘          │  │   │  │
│  │   │  │    ▲                   ▲                                    │  │   │  │
│  │   │  │   tail               head          (bounded memory!)        │  │   │  │
│  │   │  └─────────────────────────────────────────────────────────────┘  │   │  │
│  │   │                           │                                       │   │  │
│  │   │                    Flushing Thread                                │   │  │
│  │   │                     (cv.wait())                                   │   │  │
│  │   │                           │                                       │   │  │
│  │   │  ┌────────────────────────▼────────────────────────────────────┐  │   │  │
│  │   │  │                    Thread Pool                              │  │   │  │
│  │   │  │                                                             │  │   │  │
│  │   │  │    ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │  │   │  │
│  │   │  │    │ Worker 1 │  │ Worker 2 │  │ Worker 3 │  │ Worker 4 │   │  │   │  │
│  │   │  │    └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘   │  │   │  │
│  │   │  │         │             │             │             │         │  │   │  │
│  │   │  └─────────┼─────────────┼─────────────┼─────────────┼─────────┘  │   │  │
│  │   │            │             │             │             │            │   │  │
│  │   └────────────┼─────────────┼─────────────┼─────────────┼────────────┘   │  │
│  │                │             │             │             │                │  │
│  └────────────────┼─────────────┼─────────────┼─────────────┼────────────────┘  │
│                   │             │             │             │                   │
│  ┌────────────────┼─────────────┼─────────────┼─────────────┼────────────────┐  │
│  │                ▼             ▼             ▼             ▼                │  │
│  │                      OUTPUT LAYER (Phase 1)                               │  │
│  │                                                                           │  │
│  │   ┌───────────────────────────────────────────────────────────────────┐   │  │
│  │   │                      ILogSink Interface                           │   │  │
│  │   └───────────────────────────────────────────────────────────────────┘   │  │
│  │                    │                            │                         │  │
│  │                    ▼                            ▼                         │  │
│  │   ┌─────────────────────────┐    ┌─────────────────────────┐              │  │
│  │   │    ConsoleSinkImpl      │    │     FileSinkImpl        │              │  │
│  │   │                         │    │                         │              │  │
│  │   │    ┌───────────────┐    │    │    ┌───────────────┐    │              │  │
│  │   │    │    stdout     │    │    │    │   app.log     │    │              │  │
│  │   │    └───────────────┘    │    │    └───────────────┘    │              │  │
│  │   └─────────────────────────┘    └─────────────────────────┘              │  │
│  │                                                                           │  │
│  └───────────────────────────────────────────────────────────────────────────┘  │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version | Required For |
|-------------|---------|--------------|
| C++ Compiler | GCC 8+ / Clang 7+ | All features |
| CMake | 3.16+ | Build system |
| C++ Standard | C++17 | Language features |
| pthread | - | Threading (Linux) |
| vsomeip3 | 3.x | SOME/IP features (optional) |
| CommonAPI | 3.2+ | SOME/IP features (optional) |
| nlohmann/json | 3.x | JSON configuration (included) |

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/Telemetry_and_Logging-System.git
cd Telemetry_and_Logging-System

# Configure and build (without SOME/IP)
cmake -S ./examples -B build -DApp_Source_File=phase6_demo.cpp
cmake --build build

# Run the demo
./build/Demo
```

### Build with SOME/IP Support

```bash
# Install vsomeip and CommonAPI first (see Phase 5 documentation)

# Build with SOME/IP enabled
cmake -S ./examples -B build -DApp_Source_File=phase6_demo.cpp -DENABLE_SOMEIP=ON
cmake --build build
```

---

## ⚡ Quick Start

### The Simplest Way (Recommended)

```cpp
#include "app/TelemetryApp.hpp"

int main() {
    telemetry::TelemetryApp app("config.json");
    app.start();  // Runs until Ctrl+C
    return 0;
}
```

**That's it!** The `TelemetryApp` façade handles everything:
- ✅ Configuration loading
- ✅ Source creation
- ✅ Sink creation
- ✅ Formatter setup
- ✅ Rate limiting
- ✅ Signal handling
- ✅ Graceful shutdown

### Manual Setup (Advanced)

```cpp
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "logger/LogManager.hpp"

int main() {
    // 1. Create formatter with policy
    LogFormatter<CpuPolicy> formatter("MyApp");
    
    // 2. Create LogManager (starts worker threads automatically)
    LogManager logManager(100, 4);  // 100 buffer size, 4 pool workers
    
    // 3. Add sinks
    LogSinkFactory factory;
    logManager.addSink(factory.CreateSink(SinkConfig::Console()).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("app.log")).release());
    
    // 4. Log messages (NON-BLOCKING!)
    auto logMsg = formatter.formatDataToLogMsg("85.5");
    if (logMsg.has_value()) {
        logManager.log(logMsg.value());  // Returns immediately!
    }
    
    return 0;  // Destructor handles graceful shutdown
}
```

---

## ⚙️ Configuration

### JSON Configuration File

Create a `config.json` file:

```json
{
    "application": {
        "name": "MyTelemetryApp"
    },
    "sources": [
        {
            "type": "file",
            "path": "/tmp/cpu_data.txt",
            "telemetryType": "CPU",
            "rateMs": 500
        },
        {
            "type": "file",
            "path": "/tmp/gpu_data.txt",
            "telemetryType": "GPU",
            "rateMs": 300
        },
        {
            "type": "socket",
            "path": "/tmp/telemetry.sock",
            "telemetryType": "RAM",
            "rateMs": 1000
        },
        {
            "type": "someip",
            "telemetryType": "CPU",
            "rateMs": 1000
        }
    ],
    "sinks": [
        {
            "type": "console"
        },
        {
            "type": "file",
            "path": "/var/log/telemetry.log"
        }
    ]
}
```

### Configuration Parameters

| Parameter | Type | Description | Example |
|-----------|------|-------------|---------|
| `application.name` | string | Application identifier | `"MyApp"` |
| `sources[].type` | string | Source type | `"file"`, `"socket"`, `"someip"` |
| `sources[].path` | string | Path for file/socket | `"/tmp/data.txt"` |
| `sources[].telemetryType` | string | Data type | `"CPU"`, `"GPU"`, `"RAM"` |
| `sources[].rateMs` | number | Polling rate (ms) | `500` |
| `sinks[].type` | string | Sink type | `"console"`, `"file"` |
| `sinks[].path` | string | Path for file sink | `"/var/log/app.log"` |

### Programmatic Configuration

```cpp
#include "app/TelemetryApp.hpp"

int main() {
    telemetry::AppConfig config;
    config.appName = "MyApp";
    
    // Add CPU source
    telemetry::SourceConfig cpuSource;
    cpuSource.sourceType = telemetry::SourceType::FILE;
    cpuSource.path = "/tmp/cpu_data.txt";
    cpuSource.telemetryType = telemetry::TelemetryType::CPU;
    cpuSource.rateMs = 500;
    config.sources.push_back(cpuSource);
    
    // Add console sink
    telemetry::SinkConfigData consoleSink;
    consoleSink.sinkType = telemetry::SinkType::CONSOLE;
    config.sinks.push_back(consoleSink);
    
    telemetry::TelemetryApp app(config);
    app.start();
    return 0;
}
```

---

## 📁 Project Structure

```
Telemetry_and_Logging-System/
│
├── 📂 include/
│   ├── 📂 app/                          # Phase 6: Façade
│   │   ├── AppConfig.hpp                # Configuration structures
│   │   └── TelemetryApp.hpp             # Main façade class
│   │
│   ├── 📂 enums/                        # Enumerations
│   │   ├── LogSinkType.hpp
│   │   ├── SeverityLevel.hpp
│   │   ├── SinksType.hpp
│   │   └── TelemetrySource.hpp
│   │
│   ├── 📂 formatter/                    # Phase 3: Policy-based formatting
│   │   ├── 📂 policies/
│   │   │   ├── CpuPolicy.hpp
│   │   │   ├── GpuPolicy.hpp
│   │   │   └── RamPolicy.hpp
│   │   ├── LogFormatter.hpp
│   │   └── LogFormatterHelper.hpp
│   │
│   ├── 📂 logger/                       # Phase 1 & 4: Core logging
│   │   ├── LogManager.hpp
│   │   ├── LogManagerBuilder.hpp
│   │   └── LogMessage.hpp
│   │
│   ├── 📂 sinks/                        # Phase 1: Output destinations
│   │   ├── ILogSink.hpp
│   │   ├── ConsoleSinkImpl.hpp
│   │   ├── FileSinkImpl.hpp
│   │   ├── LogSinkFactory.hpp
│   │   └── SinkConfig.hpp
│   │
│   ├── 📂 sources/                      # Phase 2 & 5: Data sources
│   │   ├── ITelemetrySource.hpp
│   │   ├── FileTelemetrySourceImpl.hpp
│   │   ├── SocketTelemetrySourceImpl.hpp
│   │   ├── SomeIPTelemetrySourceImpl.hpp      # Phase 5
│   │   └── SomeIPTelemetrySourceAdapter.hpp   # Phase 5
│   │
│   ├── 📂 services/                     # Phase 5: SOME/IP server
│   │   └── TelemetryServiceImpl.hpp
│   │
│   ├── 📂 utils/                        # Phase 4: Concurrency utilities
│   │   ├── RingBuffer.hpp
│   │   └── ThreadPool.hpp
│   │
│   └── 📂 raii/                         # Phase 2: Resource management
│       ├── SafeFile.hpp
│       └── SafeSocket.hpp
│
├── 📂 src/
│   ├── 📂 app/                          # Façade implementation
│   │   ├── AppConfig.cpp
│   │   ├── TelemetryApp.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 formatter/
│   │   ├── LogFormatterHelper.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 logger/
│   │   ├── LogManager.cpp
│   │   ├── LogManagerBuilder.cpp
│   │   ├── LogMessage.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 sinks/
│   │   ├── ConsoleSinkImpl.cpp
│   │   ├── FileSinkImpl.cpp
│   │   ├── LogSinkFactory.cpp
│   │   ├── SinkConfig.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 sources/
│   │   ├── FileTelemetrySourceImpl.cpp
│   │   ├── SocketTelemetrySourceImpl.cpp
│   │   ├── SomeIPTelemetrySourceImpl.cpp
│   │   ├── SomeIPTelemetrySourceAdapter.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 services/
│   │   ├── TelemetryServiceImpl.cpp
│   │   ├── TelemetryServiceRunner.cpp
│   │   └── CMakeLists.txt
│   │
│   ├── 📂 raii/
│   │   ├── SafeFile.cpp
│   │   ├── SafeSocket.cpp
│   │   └── CMakeLists.txt
│   │
│   └── 📂 utils/
│       └── CMakeLists.txt
│
├── 📂 fidl/                             # Phase 5: Franca IDL
│   ├── TelemetryService.fidl
│   ├── TelemetryService.fdepl
│   └── 📂 src-gen/                      # Generated code
│
├── 📂 config/                           # Configuration files
│   ├── commonapi.ini
│   ├── vsomeip-local.json
│   └── telemetry_config.json
│
├── 📂 examples/
│   ├── CMakeLists.txt
│   ├── phase1_demo.cpp
│   ├── phase2_demo.cpp
│   ├── phase3_demo.cpp
│   ├── phase4_demo.cpp
│   ├── phase5_demo.cpp
│   └── phase6_demo.cpp
│
├── 📂 third_party/
│   ├── magic_enum.hpp
│   └── json.hpp                         # nlohmann/json
│
├── 📂 scripts/
│   └── socket_server.sh
│
├── CMakeLists.txt
├── README.md
└── LICENSE
```

---

## 🎨 Design Patterns

### Pattern Summary

| Pattern | Phase | Implementation | Purpose |
|---------|-------|----------------|---------|
| **Strategy** | 1 | `ILogSink` interface | Interchangeable sink implementations |
| **Policy-Based Design** | 3 | `LogFormatter<Policy>` | Compile-time threshold configuration |
| **Factory** | 3 | `LogSinkFactory` | Encapsulated sink creation |
| **Builder** | 3 | `LogManagerBuilder` | Fluent configuration API |
| **Singleton** | 5 | `SomeIPTelemetrySourceImpl` | Single SOME/IP client instance |
| **Adapter** | 5 | `SomeIPTelemetrySourceAdapter` | Bridge SOME/IP to `ITelemetrySource` |
| **Façade** | 6 | `TelemetryApp` | Simple unified interface |
| **Producer-Consumer** | 4 | Ring Buffer + Worker Thread | Async logging architecture |

### Pattern Details

#### 1. Façade Pattern (Phase 6)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           FAÇADE PATTERN                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   User Code:                              Hidden Complexity:                │
│   ══════════                              ══════════════════                │
│                                                                             │
│   TelemetryApp app(config);               • JSON parsing                    │
│   app.start();                            • Source creation                 │
│                                           • Sink creation                   │
│   Just 2 lines!                           • Formatter setup                 │
│                                           • Rate limiting                   │
│                                           • Signal handling                 │
│                                           • Thread management               │
│                                           • Graceful shutdown               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

#### 2. Policy-Based Design (Phase 3)

```cpp
// Zero runtime overhead - all resolved at compile time
struct CpuPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
    static constexpr const char* unit = "%";
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
};

LogFormatter<CpuPolicy> formatter("App");  // Template instantiation
```

#### 3. Singleton Pattern (Phase 5)

```cpp
// Thread-safe singleton (C++11 guarantee)
class SomeIPTelemetrySourceImpl {
private:
    SomeIPTelemetrySourceImpl() = default;  // Private constructor
    
public:
    static SomeIPTelemetrySourceImpl& getInstance() {
        static SomeIPTelemetrySourceImpl instance;  // Created once
        return instance;
    }
    
    // Delete copy/move
    SomeIPTelemetrySourceImpl(const SomeIPTelemetrySourceImpl&) = delete;
    SomeIPTelemetrySourceImpl& operator=(const SomeIPTelemetrySourceImpl&) = delete;
};
```

#### 4. Producer-Consumer (Phase 4)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      PRODUCER-CONSUMER PATTERN                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  PRODUCER (Main Thread)              CONSUMER (Flushing Thread)             │
│  ═══════════════════════             ═══════════════════════════            │
│                                                                             │
│  log(message):                       workLoop():                            │
│  {                                   while (running) {                      │
│      lock(mutex)                         lock(mutex)                        │
│      buffer.push(msg)                    cv.wait(lock, predicate)           │
│      unlock(mutex)                       msg = buffer.pop()                 │
│      cv.notify_one()   ─────────────▶    unlock(mutex)                      │
│      return immediately                  threadPool.submit(write)           │
│  }                                   }                                      │
│                                                                             │
│  Main thread NEVER blocks on I/O!                                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🔧 Components

### Telemetry Policies

| Policy | Context | Unit | WARNING | CRITICAL |
|--------|---------|------|---------|----------|
| `CpuPolicy` | CPU | % | 75.0 | 90.0 |
| `GpuPolicy` | GPU | % | 80.0 | 95.0 |
| `RamPolicy` | RAM | MB | 70.0 | 85.0 |

### Severity Levels

| Level | Condition | Description |
|-------|-----------|-------------|
| `INFO` | value ≤ WARNING | Normal operation |
| `WARNING` | WARNING < value ≤ CRITICAL | Attention needed |
| `CRITICAL` | value > CRITICAL | Immediate action required |

### Data Sources

| Source | Type | Use Case |
|--------|------|----------|
| `FileTelemetrySourceImpl` | File | Reading from log files, `/proc/*` |
| `SocketTelemetrySourceImpl` | Unix Socket | Local IPC |
| `SomeIPTelemetrySourceAdapter` | Network | Automotive/distributed systems |

### Output Sinks

| Sink | Output | Use Case |
|------|--------|----------|
| `ConsoleSinkImpl` | `stdout` | Development, debugging |
| `FileSinkImpl` | `.log` files | Production logging |

### Log Message Format

```
[TIMESTAMP] <SEVERITY> (APP_NAME - CONTEXT) : MESSAGE
```

Example:
```
[2024-01-15 14:30:45] <WARNING> (TelemetryApp - CPU) : CPU usage at 78.50% - Approaching high usage
```

---

## 📈 Phase Development

This project was developed incrementally across six phases:

### Phase 1: Core Synchronous Logging ✅

| Component | Description |
|-----------|-------------|
| `ILogSink` Interface | Strategy pattern for output destinations |
| `ConsoleSinkImpl` | Console output implementation |
| `FileSinkImpl` | File output implementation |
| `LogMessage` | Data structure for log entries |
| `LogManager` | Central logging orchestrator |

**Key Concepts:** Virtual functions, polymorphism, inheritance

### Phase 2: Data Sources & RAII ✅

| Component | Description |
|-----------|-------------|
| `ITelemetrySource` | Interface for data sources |
| `FileTelemetrySourceImpl` | File-based telemetry |
| `SocketTelemetrySourceImpl` | Unix socket telemetry |
| `SafeFile` | RAII wrapper for file descriptors |
| `SafeSocket` | RAII wrapper for socket descriptors |

**Key Concepts:** RAII, move semantics, Rule of 5

### Phase 3: Policy-Based Design ✅

| Component | Description |
|-----------|-------------|
| `CpuPolicy`, `GpuPolicy`, `RamPolicy` | Threshold policies |
| `LogFormatter<Policy>` | Template-based formatter |
| `LogSinkFactory` | Factory pattern for sinks |
| `LogManagerBuilder` | Builder pattern for configuration |
| `RingBuffer` | Basic circular buffer |

**Key Concepts:** Templates, policy-based design, factory/builder patterns

### Phase 4: Asynchronous Logging ✅

| Component | Description |
|-----------|-------------|
| Thread-Safe `RingBuffer` | Mutex-protected circular buffer |
| `ThreadPool` | Worker thread management |
| Async `LogManager` | Non-blocking logging |
| Condition Variables | Efficient thread synchronization |

**Key Concepts:** Multithreading, condition variables, producer-consumer

### Phase 5: SOME/IP Network Telemetry ✅

| Component | Description |
|-----------|-------------|
| `SomeIPTelemetrySourceImpl` | Singleton SOME/IP client |
| `SomeIPTelemetrySourceAdapter` | Adapter to `ITelemetrySource` |
| `TelemetryServiceImpl` | SOME/IP server implementation |
| Franca IDL | Interface definition |
| CommonAPI/vsomeip | Middleware integration |

**Key Concepts:** Singleton, adapter pattern, SOME/IP, code generation

### Phase 6: System Wrap-Up ✅

| Component | Description |
|-----------|-------------|
| `TelemetryApp` | Façade class |
| `AppConfig` | Configuration structures |
| JSON Configuration | Runtime configuration |
| Signal Handling | Graceful shutdown (Ctrl+C) |
| Rate Limiting | Configurable polling rates |

**Key Concepts:** Façade pattern, JSON parsing, signal handling

---

## 📚 API Reference

### TelemetryApp (Façade)

```cpp
namespace telemetry {

class TelemetryApp {
public:
    // Construct from JSON file
    explicit TelemetryApp(const std::string& configPath);
    
    // Construct from config struct
    explicit TelemetryApp(const AppConfig& config);
    
    ~TelemetryApp();
    
    void start();           // Blocking - runs until stop() or Ctrl+C
    void stop();            // Signal to stop
    bool isRunning() const; // Check if running
    
    // Non-copyable
    TelemetryApp(const TelemetryApp&) = delete;
    TelemetryApp& operator=(const TelemetryApp&) = delete;
};

} // namespace telemetry
```

### LogManager

```cpp
class LogManager {
public:
    explicit LogManager(size_t bufferCapacity, size_t poolSize = 4);
    ~LogManager();  // Handles graceful shutdown
    
    void addSink(ILogSink* sink);      // LogManager takes ownership
    void removeSink(ILogSink* sink);
    void log(const LogMessage& msg);   // Non-blocking!
    
    void DeleteAllSinks();
    void DeleteAllLogMessages();
};
```

### LogFormatter\<Policy\>

```cpp
template <typename PolicyType>
class LogFormatter {
public:
    explicit LogFormatter(const std::string& appName);
    
    std::optional<LogMessage> formatDataToLogMsg(const std::string& rawData);
};
```

### RingBuffer\<T\>

```cpp
template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity);
    
    bool try_push(T value);           // Returns false if full
    std::optional<T> try_pop();       // Returns nullopt if empty
    void clear();
    
    size_t size() const;
    size_t capacity() const;
    bool empty() const;
    bool full() const;
};
```

### ThreadPool

```cpp
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();  // Stops all workers
    
    void submit(std::function<void()> task);
};
```

---

## ⚡ Performance

### Synchronous vs Asynchronous

| Metric | Synchronous | Asynchronous |
|--------|-------------|--------------|
| Main Thread Blocking | Yes | **No** |
| Sink Writing | Sequential | **Parallel** |
| Memory Usage | Unbounded | **Bounded** |
| Log Call Latency | High (I/O wait) | **~0.01ms** |
| Throughput | Limited by slowest sink | **Limited by producer** |

### Performance Visualization

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      PERFORMANCE COMPARISON                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  SYNCHRONOUS:                                                               │
│  Main: [Read][Format][Log][===WAIT 260ms===][Read][Format][Log][===WAIT===] │
│                           ↑                                                 │
│                      Blocked on I/O!                                        │
│                                                                             │
│  ASYNCHRONOUS:                                                              │
│  Main:   [Read][Format][Push][Read][Format][Push][Read][Format][Push]...    │
│                         │          │          │                             │
│                      ~0.01ms each (just buffer push)                        │
│                         ↓          ↓          ↓                             │
│  Worker:           [Pop+Write][Pop+Write][Pop+Write]...                     │
│                                                                             │
│  IMPROVEMENT: Main thread 26,000x faster per log call!                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 🐛 Troubleshooting

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| "Cannot open config file" | File path incorrect | Check path exists |
| "Failed to open source" | File/socket doesn't exist | Create test files or start socket server |
| "SOME/IP not enabled" | Built without flag | Rebuild with `-DENABLE_SOMEIP=ON` |
| "Service not available" | SOME/IP server not running | Start server before client |
| App doesn't stop | Signal handler issue | Use Ctrl+C or call `stop()` |
| High CPU usage | Rate too low | Increase `rateMs` (minimum 100ms) |

### Debug Mode

```bash
# Enable verbose vsomeip logging
export VSOMEIP_CONFIGURATION=./config/vsomeip-local.json
# Edit vsomeip-local.json: "level": "debug"
```

---

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### Code Style

- Use meaningful variable and function names
- Follow existing code formatting
- Add comments for complex logic
- Update documentation for API changes
- Write unit tests for new features

---

## 👤 Author

**Eng. Mohamed Gamal**

- GitHub: [@MG-Codeflare](https://github.com/yourusername)
- LinkedIn: [Mohamed Gamal](https://linkedin.com/in/yourprofile)

---

## 🙏 Acknowledgments

- **Modern C++ Design** by Andrei Alexandrescu (Policy-Based Design)
- **C++ Concurrency in Action** by Anthony Williams (Threading patterns)
- **Design Patterns: Elements of Reusable Object-Oriented Software** (GoF)
- **COVESA** for vsomeip and CommonAPI
- **nlohmann** for the excellent JSON library

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

### 🎉 Project Complete!

```
╔═══════════════════════════════════════════════════════════════════╗
║                                                                   ║
║          TELEMETRY AND LOGGING SYSTEM                             ║
║                                                                   ║
║   ✓ Phase 1: Core Synchronous Logging                             ║
║   ✓ Phase 2: Data Sources & RAII                                  ║
║   ✓ Phase 3: Policy-Based Design                                  ║
║   ✓ Phase 4: Asynchronous Logging                                 ║
║   ✓ Phase 5: SOME/IP Network Telemetry                            ║
║   ✓ Phase 6: Façade & JSON Configuration                          ║
║                                                                   ║
║   Usage: TelemetryApp app("config.json");                         ║
║          app.start();                                             ║
║                                                                   ║
╚═══════════════════════════════════════════════════════════════════╝
```

**⭐ Star this repository if you find it helpful! ⭐**

Built with ❤️ and Modern C++17

</div>