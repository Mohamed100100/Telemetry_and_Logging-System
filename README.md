

# 📊 Telemetry and Logging System

A modern, extensible C++17 telemetry monitoring and logging system featuring policy-based design, multiple output sinks, ring buffer for efficient message queuing, and robust resource management through RAII principles.

---

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![License](https://img.shields.io/badge/license-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Architecture](#-architecture)
- [Getting Started](#-getting-started)
- [Project Structure](#-project-structure)
- [Design Patterns](#-design-patterns)
- [Components](#-components)
- [Ring Buffer](#-ring-buffer)
- [Usage Examples](#-usage-examples)
- [Phase Development](#-phase-development)
- [API Reference](#-api-reference)
- [Building & Testing](#-building--testing)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🎯 Overview

The **Telemetry and Logging System** is a high-performance, flexible logging framework designed for monitoring system telemetry data (CPU, GPU, RAM usage). It processes raw telemetry readings, classifies them by severity based on configurable thresholds, and outputs formatted log messages to multiple destinations.

### Why This Project?

- **Real-world Application**: Demonstrates practical software engineering for system monitoring
- **Modern C++**: Showcases C++17 features and best practices
- **Design Patterns**: Implements industry-standard patterns for maintainability
- **Extensibility**: Easy to add new telemetry sources, policies, and output sinks

---

## ✨ Features

### Core Features

| Feature | Description |
|---------|-------------|
| 🎛️ **Policy-Based Configuration** | Compile-time threshold configuration per telemetry type |
| 📝 **Multiple Output Sinks** | Console, File, and extensible for Network/Database |
| 🔄 **Ring Buffer** | Efficient fixed-size circular buffer for message queuing |
| 🏭 **Factory Pattern** | Flexible sink creation without exposing implementations |
| 🔨 **Builder Pattern** | Fluent API for constructing complex logging configurations |
| 🔒 **RAII Resource Management** | Safe handling of files, sockets, and system resources |
| 📊 **Severity Classification** | Automatic INFO/WARNING/CRITICAL classification |
| ⏰ **Timestamping** | Automatic timestamp generation for all log entries |
| 🔌 **Pluggable Architecture** | Easy integration of new telemetry sources |

### Technical Highlights

- ✅ Modern C++17 standard
- ✅ Template metaprogramming for type safety
- ✅ Smart pointers for memory safety
- ✅ Exception-safe design
- ✅ Header-only policy classes
- ✅ Zero-cost abstractions
- ✅ Efficient circular buffer with O(1) operations

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                         TELEMETRY AND LOGGING SYSTEM                            │
├─────────────────────────────────────────────────────────────────────────────────┤
│                                                                                 │
│   ┌─────────────────┐                                                           │
│   │   DATA SOURCES  │                                                           │
│   ├─────────────────┤                                                           │
│   │ • File Source   │─────┐                                                     │
│   │ • Socket Source │     │                                                     │
│   │ • (Extensible)  │     │                                                     │
│   └─────────────────┘     │                                                     │
│                           ▼                                                     │
│   ┌─────────────────────────────────────────────────────────────┐               │
│   │                    PROCESSING LAYER                         │               │
│   ├─────────────────────────────────────────────────────────────┤               │
│   │                                                             │               │
│   │  ┌──────────────┐    ┌───────────────────────────────────┐  │               │
│   │  │   Policies   │    │      LogFormatter<Policy>         │  │               │
│   │  ├──────────────┤    ├───────────────────────────────────┤  │               │
│   │  │ • CpuPolicy  │───▶│ • Parse raw telemetry data        │  │               │
│   │  │ • GpuPolicy  │    │ • Apply policy thresholds         │  │               │
│   │  │ • RamPolicy  │    │ • Generate severity level         │  │               │
│   │  └──────────────┘    │ • Create timestamped LogMessage   │  │               │
│   │                      └───────────────┬───────────────────┘  │               │
│   │                                      │                      │               │
│   └──────────────────────────────────────┼──────────────────────┘               │
│                                          │                                      │
│                                          ▼                                      │
│   ┌─────────────────────────────────────────────────────────────┐               │
│   │                    LOGGING LAYER                            │               │
│   ├─────────────────────────────────────────────────────────────┤               │
│   │                                                             │               │
│   │  ┌─────────────────┐         ┌────────────────────────┐     │               │
│   │  │  LogManager     │◀────────│  LogManagerBuilder     │     │               │
│   │  ├─────────────────┤         ├────────────────────────┤     │               │
│   │  │                 │         │ • Fluent API           │     │               │
│   │  │ ┌─────────────┐ │         │ • addSink()            │     │               │
│   │  │ │ RingBuffer  │ │         │ • setBufferSize()      │     │               │
│   │  │ │ <LogMessage>│ │         │ • build()              │     │               │
│   │  │ └─────────────┘ │         └────────────────────────┘     │               │
│   │  │                 │                                        │               │
│   │  │ • Sink Registry │                                        │               │
│   │  │ • flush()       │                                        │               │
│   │  └────────┬────────┘                                        │               │
│   │           │                                                 │               │
│   └───────────┼─────────────────────────────────────────────────┘               │
│               │                                                                 │
│               ▼                                                                 │
│   ┌─────────────────────────────────────────────────────────────┐               │
│   │                    OUTPUT LAYER                             │               │
│   ├─────────────────────────────────────────────────────────────┤               │
│   │                                                             │               │
│   │  ┌────────────────┐                                         │               │
│   │  │ LogSinkFactory │                                         │               │
│   │  └───────┬────────┘                                         │               │
│   │          │ creates                                          │               │
│   │          ▼                                                  │               │
│   │  ┌──────────────────────────────────────────────────────┐   │               │
│   │  │                    ILogSink                          │   │               │
│   │  ├──────────────────────────────────────────────────────┤   │               │
│   │  │     ▲                 ▲                 ▲            │   │               │
│   │  │     │                 │                 │            │   │               │
│   │  │ ┌───┴────┐       ┌────┴────┐       ┌────┴─────┐      │   │               │
│   │  │ │Console │       │  File   │       │ (Future) │      │   │               │
│   │  │ │ Sink   │       │  Sink   │       │  Sinks   │      │   │               │
│   │  │ └────────┘       └─────────┘       └──────────┘      │   │               │
│   │  └──────────────────────────────────────────────────────┘   │               │
│   │          │                  │                               │               │
│   └──────────┼──────────────────┼───────────────────────────────┘               │
│              ▼                  ▼                                               │
│         ┌────────┐         ┌────────┐                                           │
│         │ stdout │         │  .log  │                                           │
│         └────────┘         │ files  │                                           │
│                            └────────┘                                           │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | GCC 8+, Clang 7+, or MSVC 2019+ |
| CMake | 3.16 or higher |
| Standard | C++17 |

### Installation

```bash
# Clone the repository
>> git clone https://github.com/yourusername/Telemetry_and_Logging-System.git
>> cd Telemetry_and_Logging-System

>> cmake -S ./examples -B build
>> cmake --build ./build    
>> ./build/Demo
```

### Quick Start

```cpp
#include "formatter/LogFormatter.hpp"
#include "policies/CpuPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "logger/LogManagerBuilder.hpp"

int main() {
    // 1. Create formatter with policy
    LogFormatter<CpuPolicy> formatter("MyApp");
    
    // 2. Create sinks via factory
    LogSinkFactory factory;
    auto consoleSink = factory.CreateSink(SinkConfig::Console());
    auto fileSink = factory.CreateSink(SinkConfig::File("app.log"));
    
    // 3. Build LogManager with custom buffer size
    auto logManager = LogManagerBuilder()
        .setBufferSize(500)  // Ring buffer capacity
        .addSink(consoleSink.release())
        .addSink(fileSink.release())
        .build();
    
    // 4. Process telemetry and log
    auto logMsg = formatter.formatDataToLogMsg("85.5");
    if (logMsg.has_value()) {
        logManager->log(logMsg.value());
    }
    
    // 5. Flush to all sinks
    logManager->flush();
    
    return 0;
}
```

---

## 📁 Project Structure

```

project/
├── 📂include/
│   ├── 📂enums/
│   │   ├── LogSinkType.hpp
│   │   ├── SeverityLevel.hpp
│   │   ├── SinksType.hpp
│   │   └── TelemetrySource.hpp
│   │
│   ├── 📂formatter/
│   |   ├── 📂policies/
│   │   │       ├── CpuPolicy.hpp
│   │   │       ├── GpuPolicy.hpp
|   │   │       └── RamPolicy.hpp
│   │   ├── LogFormatter.hpp
│   │   └── LogFormatterHelper.hpp
│   │
│   ├── 📂logger/
│   │   ├── LogManager.hpp
│   │   ├── LogManagerBuilder.hpp
│   │   └── LogMessage.hpp
│   │
│   ├── 📂sinks/
│   │   ├── ILogSink.hpp
│   │   ├── ConsoleSinkImpl.hpp
│   │   ├── FileSinkImpl.hpp
│   │   ├── LogSinkFactory.hpp
│   │   └── SinkConfig.hpp
│   │
│   ├── 📂sources/
│   │   ├── ITelemetrySource.hpp
│   │   ├── FileTelemetrySourceImpl.hpp
│   │   └── SocketTelemetrySourceImpl.hpp
│   ├── 📂utils
│   │   └── RingBuffer.hpp
│   │
│   └── 📂raii/
│       ├── SafeFile.hpp
│       └── SafeSocket.hpp
│
├── 📂src/
│   ├── 📂formatter/
|   |   ├── CMakeLists.txt
│   │   └── LogFormatterHelper.cpp
│   │
│   ├── 📂logger/
|   |   ├── CMakeLists.txt
│   │   ├── LogManager.cpp
│   │   ├── LogManagerBuilder.cpp
│   │   └── LogMessage.cpp
│   │
│   │── 📂raii/
|   |   ├── CMakeLists.txt
│   │   ├── SafeFile.cpp
│   │   └── SafeSocket.cpp
│   └── 📂sinks/
|       ├── CMakeLists.txt
│       ├── ConsoleSinkImpl.cpp
│       ├── FileSinkImpl.cpp
│       ├── LogSinkFactory.cpp
│       └── SinkConfig.cpp
│
├── 📂examples/
|   ├── CMakeLists.txt
|   ├── phase1_demo.cpp
|   ├── phase2_demo.cpp
│   └── phase3_demo.cpp
│
├── 📂third_party/
│   └── magic_enum.hpp
|
└── 📂scripts/
    └── socket_server.sh
```

---

## 🎨 Design Patterns

### 1. Policy-Based Design

Compile-time configuration using template parameters:

```cpp
// Define policy with thresholds
struct CpuPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::CPU;
    static constexpr const char* unit = "%";
    static constexpr float WARNING = 75.0f;
    static constexpr float CRITICAL = 90.0f;
};

// Use policy at compile time
LogFormatter<CpuPolicy> formatter("App");  // Zero runtime overhead
```

**Benefits:**
- Type safety at compile time
- Zero runtime overhead
- Easy to add new policies

### 2. Factory Pattern

Encapsulates object creation logic:

```cpp
LogSinkFactory factory;

// Create sinks without knowing concrete types
auto consoleSink = factory.CreateSink(SinkConfig::Console());
auto fileSink = factory.CreateSink(SinkConfig::File("app.log"));
```

**Benefits:**
- Decouples client from concrete implementations
- Centralizes creation logic
- Easy to extend with new sink types

### 3. Builder Pattern

Fluent API for complex object construction:

```cpp
auto logManager = LogManagerBuilder()
    .setBufferSize(1000)
    .addSink(consoleSink)
    .addSink(fileSink)
    .addSink(networkSink)
    .build();
```

**Benefits:**
- Readable, self-documenting code
- Flexible configuration
- Immutable final object

### 4. RAII (Resource Acquisition Is Initialization)

Safe resource management:

```cpp
class SafeFile {
    int fd;
public:
    SafeFile(const std::string& path) {
        fd = open(path.c_str(), O_RDONLY);  // Acquire
    }
    ~SafeFile() {
        if (fd >= 0) close(fd);              // Release
    }
};
// Resource automatically released when object goes out of scope
```

**Benefits:**
- Exception-safe resource handling
- No resource leaks
- Automatic cleanup

### 5. Strategy Pattern (via Interface)

Interchangeable sink implementations:

```cpp
class ILogSink {
public:
    virtual void write(const LogMessage& msg) = 0;
    virtual ~ILogSink() = default;
};

// Different strategies
class ConsoleSinkImpl : public ILogSink { /* ... */ };
class FileSinkImpl : public ILogSink { /* ... */ };
```

---

## 🔧 Components

### Policies

| Policy | Telemetry | Unit | WARNING | CRITICAL |
|--------|-----------|------|---------|----------|
| `CpuPolicy` | CPU Usage | % | 75.0 | 90.0 |
| `GpuPolicy` | GPU Usage | % | 80.0 | 95.0 |
| `RamPolicy` | RAM Usage | MB | 70.0 | 85.0 |

### Severity Levels

| Level | Condition | Description |
|-------|-----------|-------------|
| `INFO` | value ≤ WARNING | Normal operation |
| `WARNING` | WARNING < value ≤ CRITICAL | Attention needed |
| `CRITICAL` | value > CRITICAL | Immediate action required |

### Sinks

| Sink | Output | Use Case |
|------|--------|----------|
| `ConsoleSinkImpl` | `stdout` | Development, debugging |
| `FileSinkImpl` | `.log` files | Production logging, audit trails |

### Log Message Format

```
[TIMESTAMP] <SEVERITY> (APP_NAME - CONTEXT) : MESSAGE
```

Example:
```
[2024-01-15 14:30:45] <WARNING> (TelemetryApp - CPU) : CPU usage: 78.500000%
```

---

## 🔄 Ring Buffer

The system uses a **Ring Buffer** (Circular Buffer) for efficient message queuing in the `LogManager`. This provides constant memory usage and O(1) operations.

### What is a Ring Buffer?

A ring buffer is a fixed-size data structure that uses a single, contiguous block of memory as if it were connected end-to-end. When the buffer is full, new elements overwrite the oldest ones.

### Visual Representation

```
RING BUFFER OPERATION (capacity = 5)
════════════════════════════════════

1. Initial State (empty):
   ┌───┬───┬───┬───┬───┐
   │   │   │   │   │   │   head=0, tail=0, count=0
   └───┴───┴───┴───┴───┘
     ▲
     head/tail

2. After push_back(A, B, C):
   ┌───┬───┬───┬───┬───┐
   │ A │ B │ C │   │   │   head=3, tail=0, count=3
   └───┴───┴───┴───┴───┘
     ▲           ▲
     tail        head

3. After push_back(D, E) - Buffer Full:
   ┌───┬───┬───┬───┬───┐
   │ A │ B │ C │ D │ E │   head=0, tail=0, count=5
   └───┴───┴───┴───┴───┘
     ▲
     head/tail (wrapped)

4. After push_back(F) - Overwrites Oldest (A):
   ┌───┬───┬───┬───┬───┐
   │ F │ B │ C │ D │ E │   head=1, tail=1, count=5
   └───┴───┴───┴───┴───┘
       ▲
       head/tail

5. After clear():
   ┌───┬───┬───┬───┬───┐
   │   │   │   │   │   │   head=0, tail=0, count=0
   └───┴───┴───┴───┴───┘
     ▲
     head/tail
```

### Why Use a Ring Buffer?

| Advantage | Description |
|-----------|-------------|
| **Fixed Memory** | Constant memory usage regardless of log volume |
| **O(1) Operations** | push_back and clear are constant time |
| **No Allocations** | After initialization, no dynamic memory allocation |
| **Cache Friendly** | Contiguous memory improves cache performance |
| **Automatic Cleanup** | Old messages automatically discarded when full |
| **Bounded Memory** | Prevents memory exhaustion in high-volume scenarios |

### Ring Buffer vs Standard Vector

| Feature | Ring Buffer | std::vector |
|---------|-------------|-------------|
| Memory Growth | Fixed | Dynamic |
| push_back | O(1) | O(1) amortized, O(n) worst |
| Memory Usage | Constant | Unbounded |
| Old Data | Auto-overwritten | Retained |
| Best For | Bounded queues | Dynamic collections |

### Implementation

```cpp
template <typename T>
class RingBuffer {
private:
    std::vector<T> buffer_;
    size_t head_;       // Next write position
    size_t tail_;       // Oldest element position
    size_t count_;      // Current number of elements
    size_t capacity_;   // Maximum capacity

public:
    explicit RingBuffer(size_t capacity);
    
    void push_back(const T& value);  // Add element (overwrites if full)
    void push_back(T&& value);       // Move version
    void clear();                     // Reset buffer
    
    size_t size() const;             // Current element count
    size_t capacity() const;         // Maximum capacity
    bool empty() const;              // Check if empty
    
    T& operator[](size_t index);     // Access by logical index
};
```

### Usage in LogManager

```cpp
class LogManager {
private:
    RingBuffer<LogMessage> LogMessagesBuffer;  // Fixed-size message queue
    
public:
    explicit LogManager(size_t bufferSize = 1000) 
        : LogMessagesBuffer(bufferSize) {}
    
    void log(const LogMessage& msg) {
        LogMessagesBuffer.push_back(msg);  // O(1), auto-overwrites oldest
    }
    
    void flush() {
        for (size_t i = 0; i < LogMessagesBuffer.size(); i++) {
            // Write to sinks
        }
        LogMessagesBuffer.clear();
    }
};
```

### Configuration

```cpp
// Default buffer size (1000 messages)
auto logManager1 = LogManagerBuilder()
    .addSink(consoleSink)
    .build();

// Custom buffer size
auto logManager2 = LogManagerBuilder()
    .setBufferSize(5000)  // Larger buffer for high-volume logging
    .addSink(consoleSink)
    .addSink(fileSink)
    .build();

// Small buffer for memory-constrained environments
auto logManager3 = LogManagerBuilder()
    .setBufferSize(100)
    .addSink(consoleSink)
    .build();
```

### Ring Buffer Behavior Example

```cpp
// Buffer with capacity 3
RingBuffer<LogMessage> buffer(3);

// Add 3 messages (buffer now full)
buffer.push_back(msg1);  // [msg1, _, _]
buffer.push_back(msg2);  // [msg1, msg2, _]
buffer.push_back(msg3);  // [msg1, msg2, msg3] - FULL

// Add 4th message - overwrites msg1
buffer.push_back(msg4);  // [msg4, msg2, msg3]
                         //        ^oldest  ^newest

// Iterate (returns in order: msg2, msg3, msg4)
for (size_t i = 0; i < buffer.size(); i++) {
    process(buffer[i]);
}
```

---

## 💻 Usage Examples

### Basic Logging

```cpp
#include "formatter/LogFormatter.hpp"
#include "policies/CpuPolicy.hpp"

LogFormatter<CpuPolicy> formatter("MyApp");
auto logMsg = formatter.formatDataToLogMsg("85.5");

if (logMsg.has_value()) {
    std::cout << logMsg.value().ToString() << std::endl;
}
```

### Multiple Telemetry Sources

```cpp
LogFormatter<CpuPolicy> cpuFormatter("Monitor");
LogFormatter<GpuPolicy> gpuFormatter("Monitor");
LogFormatter<RamPolicy> ramFormatter("Monitor");

// Same value, different severities based on policy
auto cpuLog = cpuFormatter.formatDataToLogMsg("77.0");  // WARNING (> 75)
auto gpuLog = gpuFormatter.formatDataToLogMsg("77.0");  // INFO    (< 80)
auto ramLog = ramFormatter.formatDataToLogMsg("77.0");  // WARNING (> 70)
```

### Multiple Output Sinks

```cpp
LogSinkFactory factory;

auto logManager = LogManagerBuilder()
    .setBufferSize(2000)
    .addSink(factory.CreateSink(SinkConfig::Console()).release())
    .addSink(factory.CreateSink(SinkConfig::File("app.log")).release())
    .addSink(factory.CreateSink(SinkConfig::File("errors.log")).release())
    .build();

// All sinks receive the message
logManager->log(message);
logManager->flush();
```

### High-Volume Logging with Ring Buffer

```cpp
// For high-volume scenarios, use larger buffer
auto logManager = LogManagerBuilder()
    .setBufferSize(10000)  // Buffer 10,000 messages
    .addSink(factory.CreateSink(SinkConfig::File("telemetry.log")).release())
    .build();

// Continuous telemetry processing
while (monitoring) {
    auto reading = getTelemetryReading();
    auto logMsg = formatter.formatDataToLogMsg(reading);
    
    if (logMsg.has_value()) {
        logManager->log(logMsg.value());
    }
    
    // Periodic flush
    if (shouldFlush()) {
        logManager->flush();
    }
}
```

### Custom Policy

```cpp
// Create your own policy
struct NetworkPolicy {
    static constexpr TelemetrySrc_enum context = TelemetrySrc_enum::NETWORK;
    static constexpr const char* unit = " Mbps";
    static constexpr float WARNING = 80.0f;
    static constexpr float CRITICAL = 95.0f;
};

// Use it
LogFormatter<NetworkPolicy> networkFormatter("NetworkMonitor");
```

---

## 📈 Phase Development

This project was developed in three phases:

### Phase 1: Foundation

- ✅ Project structure setup
- ✅ Enum definitions
- ✅ Basic interfaces (`ILogSink`, `ITelemetrySource`)
- ✅ RAII wrappers (`SafeFile`, `SafeSocket`)

### Phase 2: Core Implementation

- ✅ Telemetry source implementations
- ✅ Sink implementations (Console, File)
- ✅ LogMessage data class
- ✅ Basic logging functionality

### Phase 3: Advanced Features

- ✅ Policy-based design
- ✅ Template LogFormatter
- ✅ Factory pattern for sinks
- ✅ Builder pattern for LogManager
- ✅ Ring Buffer for message queuing
- ✅ Full integration

### Future Enhancements

- ⏳ Network sink (TCP/UDP)
- ⏳ Database sink
- ⏳ Async logging
- ⏳ Log rotation
- ⏳ JSON output format
- ⏳ Filtering by severity
- ⏳ Thread-safe ring buffer
- ⏳ Unit test coverage

---

## 📚 API Reference

### RingBuffer\<T\>

```cpp
template <typename T>
class RingBuffer {
public:
    // Constructor
    explicit RingBuffer(size_t capacity);
    
    // Modifiers
    void push_back(const T& value);  // Add element (overwrites oldest if full)
    void push_back(T&& value);       // Move version
    void clear();                     // Remove all elements
    
    // Capacity
    size_t size() const;             // Current element count
    size_t capacity() const;         // Maximum capacity
    bool empty() const;              // Check if empty
    
    // Element access
    T& operator[](size_t index);           // Access element (0 = oldest)
    const T& operator[](size_t index) const;
};
```

### LogFormatter\<PolicyType\>

```cpp
template <typename PolicyType>
class LogFormatter {
public:
    // Constructor
    LogFormatter(const std::string& appName);
    
    // Convert raw value to LogMessage
    // Returns std::nullopt on parse failure
    std::optional<LogMessage> formatDataToLogMsg(const std::string& raw);
};
```

### LogManager

```cpp
class LogManager {
public:
    // Constructor
    explicit LogManager(size_t bufferSize = 1000);
    
    // Sink management
    void addSink(ILogSink* sink);            // Add output sink
    void removeSink(ILogSink* sink);         // Remove sink
    void DeleteAllSinks();                   // Clear all sinks
    
    // Logging
    void log(const LogMessage& message);     // Queue message in ring buffer
    void flush();                            // Write all messages to all sinks
    void DeleteAllLogMessages();             // Clear message buffer
};
```

### LogManagerBuilder

```cpp
class LogManagerBuilder {
public:
    LogManagerBuilder& setBufferSize(size_t size);        // Set ring buffer capacity
    LogManagerBuilder& addSink(ILogSink* sink);           // Chain method
    LogManagerBuilder& addLogMessage(LogMessage& msg);    // Chain method
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
    
    static SinkConfig Console();                     // Factory method
    static SinkConfig File(const std::string& path); // Factory method
};
```

### LogMessage

```cpp
class LogMessage {
public:
    LogMessage(const std::string& appName,
               const std::string& context,
               const std::string& severity,
               const std::string& time,
               const std::string& message);
               
    std::string ToString();  // Format for output
};
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

### Commit Messages

```
type(scope): description

[optional body]
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`



---

## 👤 Author

**Eng.Mohamed Gamal**


---

<div align="center">

**⭐ Star this repository if you find it helpful! ⭐**

</div>
