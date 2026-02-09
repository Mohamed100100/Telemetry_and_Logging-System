# 📊 Telemetry and Logging System

A modern, high-performance C++17 telemetry monitoring and logging system featuring policy-based design, multiple output sinks, thread-safe ring buffers, asynchronous logging, and a thread pool for parallel sink writing.

---

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)
![Threading](https://img.shields.io/badge/threading-Multi--threaded-orange.svg)

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
- [Thread-Safe Ring Buffer](#-thread-safe-ring-buffer)
- [Asynchronous Logging](#-asynchronous-logging)
- [Thread Pool](#-thread-pool)
- [Usage Examples](#-usage-examples)
- [Phase Development](#-phase-development)
- [API Reference](#-api-reference)
- [Performance](#-performance)
- [Contributing](#-contributing)

---

## 🎯 Overview

The **Telemetry and Logging System** is a comprehensive logging framework designed for monitoring system telemetry data (CPU, GPU, RAM usage). It processes raw telemetry readings, classifies them by severity based on configurable thresholds, and outputs formatted log messages to multiple destinations simultaneously and asynchronously.

### Why This Project?

| Aspect | Description |
|--------|-------------|
| **Real-world Application** | Demonstrates practical software engineering for system monitoring |
| **Modern C++** | Showcases C++17 features and best practices |
| **Design Patterns** | Implements industry-standard patterns for maintainability |
| **Concurrency** | Features multi-threaded architecture for high performance |
| **Extensibility** | Easy to add new telemetry sources, policies, and output sinks |

---

## ✨ Features

### Core Features

| Feature | Description |
|---------|-------------|
| 🎛️ **Policy-Based Configuration** | Compile-time threshold configuration per telemetry type |
| 📝 **Multiple Output Sinks** | Console, File, and extensible for Network/Database |
| 🔄 **Thread-Safe Ring Buffer** | Efficient fixed-size circular buffer with mutex protection |
| 🏭 **Factory Pattern** | Flexible sink creation without exposing implementations |
| 🔨 **Builder Pattern** | Fluent API for constructing complex logging configurations |
| 🔒 **RAII Resource Management** | Safe handling of files, sockets, and system resources |
| 📊 **Severity Classification** | Automatic INFO/WARNING/CRITICAL classification |
| ⏰ **Timestamping** | Automatic timestamp generation for all log entries |
| 🔌 **Pluggable Architecture** | Easy integration of new telemetry sources |
| ⚡ **Asynchronous Logging** | Non-blocking log operations using background threads |
| 🧵 **Thread Pool** | Parallel sink writing for maximum throughput |
| 🔔 **Condition Variables** | Efficient thread synchronization without busy-waiting |

### Technical Highlights

- ✅ Modern C++17 standard
- ✅ Template metaprogramming for type safety
- ✅ Smart pointers for memory safety
- ✅ Exception-safe design
- ✅ Header-only policy classes
- ✅ Zero-cost abstractions
- ✅ Efficient circular buffer with O(1) operations
- ✅ Lock-free design where possible
- ✅ Bounded memory usage
- ✅ Graceful shutdown handling

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
│   │                                                             │               │
│   │  ┌──────────────┐    ┌───────────────────────────────────┐  │               │
│   │  │   Policies   │    │      LogFormatter<Policy>         │  │               │
│   │  ├──────────────┤    ├───────────────────────────────────┤  │               │
│   │  │ • CpuPolicy  │───▶│ • Parse raw telemetry data        │  │               │
│   │  │ • GpuPolicy  │    │ • Apply policy thresholds         │  │               │
│   │  │ • RamPolicy  │    │ • Generate severity level         │  │               │
│   │  └──────────────┘    │ • Create timestamped LogMessage   │  │               │
│   │                      └───────────────┬───────────────────┘  │               │
│   └──────────────────────────────────────┼──────────────────────┘               │
│                                          │                                      │
│                                          ▼                                      │
│   ┌─────────────────────────────────────────────────────────────┐               │
│   │                 ASYNC LOGGING LAYER                         │               │
│   │                                                             │               │
│   │  ┌─────────────────┐         ┌────────────────────────┐     │               │
│   │  │   LogManager    │◀────────│  LogManagerBuilder     │     │               │
│   │  ├─────────────────┤         └────────────────────────┘     │               │
│   │  │                 │                                        │               │
│   │  │ ┌─────────────┐ │    log() is NON-BLOCKING!              │               │
│   │  │ │ RingBuffer  │ │    Main thread never waits.            │               │
│   │  │ │<LogMessage> │ │                                        │               │
│   │  │ │ (bounded)   │ │                                        │               │
│   │  │ └─────────────┘ │                                        │               │
│   │  │       │         │                                        │               │
│   │  │       ▼         │                                        │               │
│   │  │ ┌─────────────┐ │                                        │               │
│   │  │ │  Flushing   │ │    Background thread consumes          │               │
│   │  │ │   Thread    │ │    messages from buffer.               │               │
│   │  │ └─────────────┘ │                                        │               │
│   │  │       │         │                                        │               │
│   │  │       ▼         │                                        │               │
│   │  │ ┌─────────────┐ │                                        │               │
│   │  │ │ ThreadPool  │ │    Parallel sink writing!              │               │
│   │  │ │ (N workers) │ │                                        │               │
│   │  │ └─────────────┘ │                                        │               │
│   │  └────────┬────────┘                                        │               │
│   └───────────┼─────────────────────────────────────────────────┘               │
│               │                                                                 │
│               ▼                                                                 │
│   ┌─────────────────────────────────────────────────────────────┐               │
│   │                    OUTPUT LAYER                             │               │
│   │                                                             │               │
│   │  ┌────────────────┐                                         │               │
│   │  │ LogSinkFactory │                                         │               │
│   │  └───────┬────────┘                                         │               │
│   │          │ creates                                          │               │
│   │          ▼                                                  │               │
│   │  ┌──────────────────────────────────────────────────────┐   │               │
│   │  │                    ILogSink                          │   │               │
│   │  │     ▲                 ▲                 ▲            │   │               │
│   │  │     │                 │                 │            │   │               │
│   │  │ ┌───┴────┐       ┌────┴────┐       ┌────┴─────┐      │   │               │
│   │  │ │Console │       │  File   │       │ (Future) │      │   │               │
│   │  │ │ Sink   │       │  Sink   │       │  Sinks   │      │   │               │
│   │  │ └────────┘       └─────────┘       └──────────┘      │   │               │
│   │  └──────────────────────────────────────────────────────┘   │               │
│   │          │                  │                               │               │
│   │          ▼                  ▼                               │               │
│   │     ┌────────┐         ┌────────┐                           │               │
│   │     │ stdout │         │  .log  │                           │               │
│   │     └────────┘         │ files  │                           │               │
│   │                        └────────┘                           │               │
│   └─────────────────────────────────────────────────────────────┘               │
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
| Threading | pthread (Linux) or Windows threads |

### Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/Telemetry_and_Logging-System.git
cd Telemetry_and_Logging-System

# Configure and build
cmake -S . -B build
cmake --build build

# Run demo
./build/examples/Demo
```

### Quick Start

```cpp
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "sinks/LogSinkFactory.hpp"
#include "logger/LogManager.hpp"

int main() {
    // 1. Create formatter with policy
    LogFormatter<CpuPolicy> formatter("MyApp");
    
    // 2. Create sinks via factory
    LogSinkFactory factory;
    
    // 3. Create async LogManager (starts worker thread automatically)
    LogManager logManager(100, 4);  // 100 buffer size, 4 thread pool workers
    logManager.addSink(factory.CreateSink(SinkConfig::Console()).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("app.log")).release());
    
    // 4. Log messages (NON-BLOCKING!)
    auto logMsg = formatter.formatDataToLogMsg("85.5");
    if (logMsg.has_value()) {
        logManager.log(logMsg.value());  // Returns immediately!
    }
    
    // 5. Destructor handles graceful shutdown
    return 0;
}
```

---

## 📁 Project Structure

```
project/
├── 📂 include/
│   ├── 📂 enums/
│   │   ├── LogSinkType.hpp
│   │   ├── SeverityLevel.hpp
│   │   ├── SinksType.hpp
│   │   └── TelemetrySource.hpp
│   │
│   ├── 📂 formatter/
│   │   ├── 📂 policies/
│   │   │   ├── CpuPolicy.hpp
│   │   │   ├── GpuPolicy.hpp
│   │   │   └── RamPolicy.hpp
│   │   ├── LogFormatter.hpp
│   │   └── LogFormatterHelper.hpp
│   │
│   ├── 📂 logger/
│   │   ├── LogManager.hpp
│   │   ├── LogManagerBuilder.hpp
│   │   └── LogMessage.hpp
│   │
│   ├── 📂 sinks/
│   │   ├── ILogSink.hpp
│   │   ├── ConsoleSinkImpl.hpp
│   │   ├── FileSinkImpl.hpp
│   │   ├── LogSinkFactory.hpp
│   │   └── SinkConfig.hpp
│   │
│   ├── 📂 sources/
│   │   ├── ITelemetrySource.hpp
│   │   ├── FileTelemetrySourceImpl.hpp
│   │   └── SocketTelemetrySourceImpl.hpp
│   │
│   ├── 📂 utils/
│   │   ├── RingBuffer.hpp
│   │   └── ThreadPool.hpp
│   │
│   └── 📂 raii/
│       ├── SafeFile.hpp
│       └── SafeSocket.hpp
│
├── 📂 src/
│   ├── 📂 formatter/
│   │   └── LogFormatterHelper.cpp
│   │
│   ├── 📂 logger/
│   │   ├── LogManager.cpp
│   │   ├── LogManagerBuilder.cpp
│   │   └── LogMessage.cpp
│   │
│   ├── 📂 sinks/
│   │   ├── ConsoleSinkImpl.cpp
│   │   ├── FileSinkImpl.cpp
│   │   ├── LogSinkFactory.cpp
│   │   └── SinkConfig.cpp
│   │
│   ├── 📂 utils/
│   │   └── ThreadPool.cpp
│   │
│   ├── 📂 sources/
│   │   ├── FileTelemetrySourceImpl.cpp
│   │   └── SocketTelemetrySourceImpl.cpp
│   │
│   └── 📂 raii/
│       ├── SafeFile.cpp
│       └── SafeSocket.cpp
│
├── 📂 examples/
│   ├── CMakeLists.txt
│   ├── phase1_demo.cpp
│   ├── phase2_demo.cpp
│   ├── phase3_demo.cpp
│   └── phase4_demo.cpp
│
├── 📂 third_party/
│   └── magic_enum.hpp
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

### 1. Policy-Based Design

Compile-time configuration using template parameters for zero runtime overhead.

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
    .setPoolSize(4)
    .addSink(consoleSink)
    .addSink(fileSink)
    .build();
```

**Benefits:**
- Readable, self-documenting code
- Flexible configuration
- Immutable final object

### 4. RAII (Resource Acquisition Is Initialization)

Safe resource management for files, sockets, threads, and mutexes:

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

### 6. Producer-Consumer Pattern

Separates data production (main thread) from consumption (worker thread):

```
Producer (Main Thread)          Consumer (Worker Thread)
        │                               │
        ▼                               ▼
   log(message)                  while(running) {
        │                           wait for data
        ▼                           pop from buffer
   push to buffer ───────────────► write to sinks
        │                         }
   return immediately
```

### 7. Thread Pool Pattern

Reuses worker threads to avoid creation overhead:

```
                    ┌─────────────────────┐
   submit(task) ───▶│    Task Queue       │
                    └──────────┬──────────┘
                               │
            ┌──────────────────┼──────────────────┐
            │                  │                  │
            ▼                  ▼                  ▼
      ┌──────────┐       ┌──────────┐       ┌──────────┐
      │ Worker 1 │       │ Worker 2 │       │ Worker 3 │
      └──────────┘       └──────────┘       └──────────┘
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
| Future: `NetworkSinkImpl` | TCP/UDP | Remote logging |
| Future: `DatabaseSinkImpl` | SQL DB | Persistent storage |

### Log Message Format

```
[TIMESTAMP] <SEVERITY> (APP_NAME - CONTEXT) : MESSAGE
```

Example:
```
[2024-01-15 14:30:45] <WARNING> (TelemetryApp - CPU) : CPU usage: 78.500000%
```

---

## 🔄 Thread-Safe Ring Buffer

The system uses a **Thread-Safe Ring Buffer** for efficient message queuing between the main thread and the flushing thread.

### What is a Ring Buffer?

A ring buffer is a fixed-size data structure that wraps around. When full, `try_push()` returns `false` instead of overwriting.

### Visual Representation

```
RING BUFFER OPERATION (capacity = 5)
════════════════════════════════════

1. Initial State (empty):
   ┌───┬───┬───┬───┬───┐
   │ ○ │ ○ │ ○ │ ○ │ ○ │   head=0, tail=0, count=0
   └───┴───┴───┴───┴───┘
     ▲
   head/tail              ○ = nullopt (empty slot)

2. After try_push(A), try_push(B), try_push(C):
   ┌───┬───┬───┬───┬───┐
   │ A │ B │ C │ ○ │ ○ │   head=3, tail=0, count=3
   └───┴───┴───┴───┴───┘
     ▲           ▲
    tail        head

3. After try_pop() returns A:
   ┌───┬───┬───┬───┬───┐
   │ ○ │ B │ C │ ○ │ ○ │   head=3, tail=1, count=2
   └───┴───┴───┴───┴───┘
         ▲       ▲
        tail    head

4. Buffer Full - try_push() returns false:
   ┌───┬───┬───┬───┬───┐
   │ E │ B │ C │ D │ E │   count=5 (FULL)
   └───┴───┴───┴───┴───┘
```

### Why Use Ring Buffer?

| Advantage | Description |
|-----------|-------------|
| **Fixed Memory** | Bounded memory usage regardless of log volume |
| **O(1) Operations** | Constant time push and pop |
| **Thread-Safe** | Mutex-protected operations |
| **No Default Constructor** | Uses `std::optional<T>` for storage |
| **Cache Friendly** | Contiguous memory improves performance |

### Why `std::optional<T>` Storage?

```
┌─────────────────────────────────────────────────────────────────┐
│              std::vector<T> vs std::vector<std::optional<T>>    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  std::vector<T>:                                                │
│  • Requires T to have default constructor                       │
│  • Constructs all N objects at initialization                   │
│  • Cannot distinguish "empty" from "valid"                      │
│                                                                 │
│  std::vector<std::optional<T>>:                                 │
│  • T doesn't need default constructor ✓                         │
│  • Slots start as nullopt (no construction) ✓                   │
│  • has_value() clearly shows if slot is used ✓                  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## ⚡ Asynchronous Logging

### The Problem with Synchronous Logging

```
SYNCHRONOUS (Blocking):
═══════════════════════

Main Thread: [Read][Format][Log][WAIT FOR I/O...][Read][Format][Log][WAIT...]
                               ↑
                        Blocked! Can't do anything else.
```

### The Solution: Async Logging

```
ASYNCHRONOUS (Non-Blocking):
════════════════════════════

Main Thread:     [Read][Format][Push][Read][Format][Push][Read]...
                              ↓         ↓         ↓
                         ┌────────────────────────────────┐
                         │    Thread-Safe Ring Buffer     │
                         └────────────────────────────────┘
                              ↓         ↓         ↓
Worker Thread:           [Pop][Write][Pop][Write][Pop][Write]...

Main thread NEVER waits! Worker handles I/O in background.
```

### How It Works

```
┌─────────────────────────────────────────────────────────────────┐
│                    ASYNC LOGGING FLOW                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  MAIN THREAD (Producer):                                        │
│  ════════════════════════                                       │
│  1. Format log message                                          │
│  2. Lock mutex                                                  │
│  3. Push to ring buffer                                         │
│  4. Unlock mutex                                                │
│  5. Notify condition variable                                   │
│  6. Return immediately ← NON-BLOCKING!                          │
│                                                                 │
│  FLUSHING THREAD (Consumer):                                    │
│  ════════════════════════════                                   │
│  while (running) {                                              │
│      1. Lock mutex                                              │
│      2. Wait on condition variable (sleeps if buffer empty)     │
│      3. Check exit condition                                    │
│      4. Unlock mutex                                            │
│      5. Pop message from buffer                                 │
│      6. Submit write task to thread pool                        │
│  }                                                              │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Condition Variable Explained

The condition variable allows the worker thread to sleep efficiently:

```
┌─────────────────────────────────────────────────────────────────┐
│                 CONDITION VARIABLE BEHAVIOR                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  cv.wait(lock, predicate):                                      │
│                                                                 │
│  1. Check predicate                                             │
│     └── TRUE  → Continue immediately                            │
│     └── FALSE → UNLOCK mutex and SLEEP (0% CPU!)                │
│                                                                 │
│  2. When notify_one()/notify_all() called:                      │
│     └── Wake up                                                 │
│     └── RE-LOCK mutex                                           │
│     └── Check predicate again                                   │
│         └── TRUE  → Continue                                    │
│         └── FALSE → Go back to sleep                            │
│                                                                 │
│  KEY: wait() RELEASES mutex while sleeping!                     │
│       This allows producer to push while consumer sleeps.       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🧵 Thread Pool

### Why Thread Pool?

```
┌─────────────────────────────────────────────────────────────────┐
│              WITHOUT THREAD POOL                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  For each task:                                                 │
│    CREATE thread (~1ms) → Execute → DESTROY thread (~0.5ms)     │
│                                                                 │
│  1000 tasks = 1000 × 1.5ms = 1500ms overhead!                   │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│              WITH THREAD POOL                                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Once: CREATE 4 threads (~4ms)                                  │
│  Per task: Push to queue (~0.001ms)                             │
│                                                                 │
│  1000 tasks = 4ms + 1000 × 0.001ms ≈ 5ms overhead!              │
│                                                                 │
│  SPEEDUP: 300x faster!                                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Thread Pool Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                    THREAD POOL ANATOMY                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌────────────────────────────────────────────────────────┐     │
│  │                     ThreadPool                          │     │
│  │                                                        │     │
│  │  std::vector<std::thread> workers_    // Worker threads│     │
│  │  std::queue<std::function<void()>> tasks_  // Task queue│    │
│  │  std::mutex mutex_                    // Protects queue│     │
│  │  std::condition_variable cv_          // Workers wait  │     │
│  │  std::atomic<bool> stop_              // Shutdown flag │     │
│  │                                                        │     │
│  │  submit(task)   // Add task to queue                   │     │
│  │  ~ThreadPool()  // Stop workers, join threads          │     │
│  │                                                        │     │
│  └────────────────────────────────────────────────────────┘     │
│                                                                 │
│  VISUAL:                                                        │
│                                                                 │
│      submit(taskA)                                              │
│      submit(taskB)     ┌─────────────────────────────┐          │
│      submit(taskC) ───▶│  [taskA][taskB][taskC]      │          │
│                        └─────────────┬───────────────┘          │
│                                      │                          │
│                     ┌────────────────┼────────────────┐         │
│                     ▼                ▼                ▼         │
│               ┌──────────┐    ┌──────────┐    ┌──────────┐      │
│               │ Worker 1 │    │ Worker 2 │    │ Worker 3 │      │
│               │  taskA   │    │  taskB   │    │  taskC   │      │
│               └──────────┘    └──────────┘    └──────────┘      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Parallel Sink Writing

```
┌─────────────────────────────────────────────────────────────────┐
│              SEQUENTIAL vs PARALLEL SINK WRITING                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  WITHOUT Thread Pool (Sequential):                              │
│  ═════════════════════════════════                              │
│                                                                 │
│  Worker: [Console 10ms][File 50ms][Network 200ms]               │
│  Total: 260ms per message                                       │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  WITH Thread Pool (Parallel):                                   │
│  ═════════════════════════════                                  │
│                                                                 │
│  Pool Worker 1: [Console 10ms]                                  │
│  Pool Worker 2: [File 50ms    ]                                 │
│  Pool Worker 3: [Network 200ms              ]                   │
│                                                                 │
│  Total: max(10, 50, 200) = 200ms per message                    │
│  SPEEDUP: 23% faster!                                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 💻 Usage Examples

### Basic Async Logging

```cpp
#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "logger/LogManager.hpp"
#include "sinks/LogSinkFactory.hpp"

int main() {
    // Create formatter
    LogFormatter<CpuPolicy> formatter("MyApp");
    
    // Create LogManager (starts worker thread)
    LogManager logManager(100, 4);  // 100 buffer, 4 pool threads
    
    // Add sinks
    LogSinkFactory factory;
    logManager.addSink(factory.CreateSink(SinkConfig::Console()).release());
    logManager.addSink(factory.CreateSink(SinkConfig::File("app.log")).release());
    
    // Log messages (NON-BLOCKING!)
    for (int i = 0; i < 100; i++) {
        auto msg = formatter.formatDataToLogMsg(std::to_string(50.0 + i * 0.5));
        if (msg.has_value()) {
            logManager.log(msg.value());  // Returns immediately!
        }
    }
    
    // Destructor handles graceful shutdown
    return 0;
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

### Reading from File Source

```cpp
#include "sources/FileTelemetrySourceImpl.hpp"

// Create file source
FileTelemetrySourceImpl source("telemetry_data.txt");

// Read and log
std::string reading;
while (source.read(reading)) {
    auto msg = formatter.formatDataToLogMsg(reading);
    if (msg.has_value()) {
        logManager.log(msg.value());
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

This project was developed incrementally across four phases:

### Phase 1: Foundation ✅

| Component | Description |
|-----------|-------------|
| Project Structure | Organized directory layout |
| Enum Definitions | Severity levels, sink types, telemetry sources |
| Core Interfaces | `ILogSink`, `ITelemetrySource` |
| RAII Wrappers | `SafeFile`, `SafeSocket` |

### Phase 2: Core Implementation ✅

| Component | Description |
|-----------|-------------|
| Telemetry Sources | File and Socket implementations |
| Sink Implementations | Console and File sinks |
| LogMessage | Data class with timestamp |
| Basic Logging | Synchronous workflow |

### Phase 3: Design Patterns ✅

| Component | Description |
|-----------|-------------|
| Policy-Based Design | Template threshold configuration |
| LogFormatter | Generic formatter with policies |
| Factory Pattern | `LogSinkFactory` |
| Builder Pattern | `LogManagerBuilder` |
| Ring Buffer | Basic circular buffer |

### Phase 4: Asynchronous Logging ✅

| Component | Description |
|-----------|-------------|
| Thread-Safe Ring Buffer | Mutex-protected with `std::optional<T>` |
| Condition Variables | Efficient thread synchronization |
| Async LogManager | Non-blocking log operations |
| Thread Pool | Parallel sink writing |
| Graceful Shutdown | Clean resource cleanup |

---

## 📚 API Reference

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
    ~ThreadPool();
    
    void submit(std::function<void()> task);
};
```

### LogManager

```cpp
class LogManager {
public:
    explicit LogManager(size_t bufferCapacity, size_t poolSize = 4);
    ~LogManager();
    
    void addSink(ILogSink* sink);
    void removeSink(ILogSink* sink);
    void log(const LogMessage& message);  // Non-blocking!
    
    void DeleteAllSinks();
    void DeleteAllLogMessages();
};
```

### LogFormatter\<PolicyType\>

```cpp
template <typename PolicyType>
class LogFormatter {
public:
    LogFormatter(const std::string& appName);
    std::optional<LogMessage> formatDataToLogMsg(const std::string& raw);
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
    
    static SinkConfig Console();
    static SinkConfig File(const std::string& path);
};
```

---

## ⚡ Performance

### Synchronous vs Asynchronous Comparison

| Metric | Synchronous | Asynchronous |
|--------|-------------|--------------|
| Main Thread Blocking | Yes | No |
| Sink Writing | Sequential | Parallel |
| Memory Usage | Unbounded | Bounded (ring buffer) |
| Throughput | Limited by slowest sink | Limited by fastest producer |
| Latency | High (waits for I/O) | Low (returns immediately) |

### Performance Gains

```
┌─────────────────────────────────────────────────────────────────┐
│                 PERFORMANCE COMPARISON                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SYNCHRONOUS (Phase 3):                                         │
│  Main: [Read][Format][Log][WAIT 260ms][Read][Format]...         │
│  Time per message: 260ms                                        │
│                                                                 │
│  ASYNCHRONOUS (Phase 4):                                        │
│  Main:   [Read][Format][Push][Read][Format][Push]...            │
│  Worker:           [Pop][Submit to Pool]                        │
│  Pool:                  [Parallel sink writing]                 │
│                                                                 │
│  Main thread time: ~0.01ms (just push to buffer)                │
│  Sink writing: ~200ms (parallel, not 260ms sequential)          │
│                                                                 │
│  IMPROVEMENT: Main thread 26,000x faster!                       │
│               Sink writing 23% faster!                          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
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

**Eng. Mohamed Gamal**

---

## 🙏 Acknowledgments

- Modern C++ Design by Andrei Alexandrescu (Policy-Based Design)
- C++ Concurrency in Action by Anthony Williams (Threading)
- Design Patterns: Elements of Reusable Object-Oriented Software (GoF Patterns)

---

<div align="center">

**⭐ Star this repository if you find it helpful! ⭐**

Built with ❤️ and Modern C++

</div>