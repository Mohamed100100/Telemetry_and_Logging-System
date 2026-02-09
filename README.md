# 📊 Telemetry and Logging System

A modern, high-performance C++17 telemetry monitoring and logging system featuring policy-based design, multiple output sinks, thread-safe ring buffers, asynchronous logging, and a thread pool for parallel sink writing.

---

<div align="center">

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Build](https://img.shields.io/badge/build-CMake-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)
![Threads](https://img.shields.io/badge/threading-Multi--threaded-orange.svg)

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Architecture](#-architecture)
- [Project Phases](#-project-phases)
- [Design Patterns](#-design-patterns)
- [Core Concepts](#-core-concepts)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Components](#-components)
- [Performance](#-performance)
- [Contributing](#-contributing)

---

## 🎯 Overview

The **Telemetry and Logging System** is a comprehensive logging framework designed for monitoring system telemetry data (CPU, GPU, RAM usage). It processes raw telemetry readings, classifies them by severity based on configurable thresholds, and outputs formatted log messages to multiple destinations simultaneously.

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
│   ┌─────────────────────────────────────────────────────────┐                   │
│   │                    PROCESSING LAYER                     │                   │
│   │                                                         │                   │
│   │  ┌──────────────┐    ┌───────────────────────────────┐  │                   │
│   │  │   Policies   │    │      LogFormatter<Policy>     │  │                   │
│   │  ├──────────────┤    ├───────────────────────────────┤  │                   │
│   │  │ • CpuPolicy  │───▶│ • Parse raw telemetry data    │  │                   │
│   │  │ • GpuPolicy  │    │ • Apply policy thresholds     │  │                   │
│   │  │ • RamPolicy  │    │ • Generate severity level     │  │                   │
│   │  └──────────────┘    │ • Create timestamped message  │  │                   │
│   │                      └───────────────┬───────────────┘  │                   │
│   └──────────────────────────────────────┼──────────────────┘                   │
│                                          │                                      │
│                                          ▼                                      │
│   ┌─────────────────────────────────────────────────────────┐                   │
│   │                 ASYNC LOGGING LAYER                     │                   │
│   │                                                         │                   │
│   │  ┌─────────────────┐         ┌────────────────────┐     │                   │
│   │  │   LogManager    │◀────────│ LogManagerBuilder  │     │                   │
│   │  ├─────────────────┤         └────────────────────┘     │                   │
│   │  │                 │                                    │                   │
│   │  │ ┌─────────────┐ │    log() is NON-BLOCKING!          │                   │
│   │  │ │ RingBuffer  │ │    Main thread never waits.        │                   │
│   │  │ │<LogMessage> │ │                                    │                   │
│   │  │ │ (bounded)   │ │                                    │                   │
│   │  │ └─────────────┘ │                                    │                   │
│   │  │       │         │                                    │                   │
│   │  │       ▼         │                                    │                   │
│   │  │ ┌─────────────┐ │                                    │                   │
│   │  │ │  Flushing   │ │    Background thread consumes      │                   │
│   │  │ │   Thread    │ │    messages from buffer.           │                   │
│   │  │ └─────────────┘ │                                    │                   │
│   │  │       │         │                                    │                   │
│   │  │       ▼         │                                    │                   │
│   │  │ ┌─────────────┐ │                                    │                   │
│   │  │ │ ThreadPool  │ │    Parallel sink writing!          │                   │
│   │  │ │ (N workers) │ │                                    │                   │
│   │  │ └─────────────┘ │                                    │                   │
│   │  └────────┬────────┘                                    │                   │
│   └───────────┼─────────────────────────────────────────────┘                   │
│               │                                                                 │
│               ▼                                                                 │
│   ┌─────────────────────────────────────────────────────────┐                   │
│   │                    OUTPUT LAYER                         │                   │
│   │                                                         │                   │
│   │  ┌────────────────┐                                     │                   │
│   │  │ LogSinkFactory │                                     │                   │
│   │  └───────┬────────┘                                     │                   │
│   │          │ creates                                      │                   │
│   │          ▼                                              │                   │
│   │  ┌──────────────────────────────────────────────────┐   │                   │
│   │  │                    ILogSink                      │   │                   │
│   │  │     ▲                 ▲                 ▲        │   │                   │
│   │  │     │                 │                 │        │   │                   │
│   │  │ ┌───┴────┐       ┌────┴────┐       ┌────┴─────┐  │   │                   │
│   │  │ │Console │       │  File   │       │ (Future) │  │   │                   │
│   │  │ │ Sink   │       │  Sink   │       │  Sinks   │  │   │                   │
│   │  │ └────────┘       └─────────┘       └──────────┘  │   │                   │
│   │  └──────────────────────────────────────────────────┘   │                   │
│   │          │                  │                           │                   │
│   │          ▼                  ▼                           │                   │
│   │     ┌────────┐         ┌────────┐                       │                   │
│   │     │ stdout │         │  .log  │                       │                   │
│   │     └────────┘         │ files  │                       │                   │
│   │                        └────────┘                       │                   │
│   └─────────────────────────────────────────────────────────┘                   │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘
```

---

## 📈 Project Phases

This project was developed incrementally across four phases, each building upon the previous:

### Phase 1: Foundation

**Objective:** Establish the project structure and core interfaces.

| Component | Description |
|-----------|-------------|
| Project Structure | Organized directory layout for headers and sources |
| Enum Definitions | Severity levels, sink types, telemetry sources |
| Core Interfaces | `ILogSink`, `ITelemetrySource` abstract base classes |
| RAII Wrappers | `SafeFile`, `SafeSocket` for resource management |

**Key Learnings:**
- C++ project organization
- Interface design with pure virtual functions
- RAII principles for resource safety

---

### Phase 2: Core Implementation

**Objective:** Implement the basic logging functionality.

| Component | Description |
|-----------|-------------|
| Telemetry Sources | File and Socket source implementations |
| Sink Implementations | Console and File sinks |
| LogMessage | Data class for log entries with timestamp |
| Basic Logging | Synchronous log processing |

**Key Learnings:**
- Implementation of interfaces
- File I/O operations
- String formatting and manipulation
- Basic logging workflow

---

### Phase 3: Design Patterns & Extensibility

**Objective:** Apply design patterns for flexibility and maintainability.

| Component | Description |
|-----------|-------------|
| Policy-Based Design | Template-based threshold configuration |
| LogFormatter | Generic formatter using policy templates |
| Factory Pattern | `LogSinkFactory` for sink creation |
| Builder Pattern | `LogManagerBuilder` for fluent configuration |
| Ring Buffer | Fixed-size circular buffer (basic version) |

**Design Patterns Implemented:**

```
┌─────────────────────────────────────────────────────────────────┐
│                    DESIGN PATTERNS                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  POLICY-BASED DESIGN                                            │
│  ═══════════════════                                            │
│  Compile-time configuration using template parameters.          │
│  Zero runtime overhead, type-safe threshold configuration.      │
│                                                                 │
│  FACTORY PATTERN                                                │
│  ═══════════════                                                │
│  Encapsulates object creation logic.                            │
│  Decouples client from concrete implementations.                │
│                                                                 │
│  BUILDER PATTERN                                                │
│  ═══════════════                                                │
│  Fluent API for complex object construction.                    │
│  Readable, self-documenting configuration code.                 │
│                                                                 │
│  STRATEGY PATTERN (via Interface)                               │
│  ═════════════════════════════════                              │
│  Interchangeable sink implementations.                          │
│  Easy to add new output destinations.                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Key Learnings:**
- Template metaprogramming
- Design pattern implementation
- Fluent interface design
- Circular buffer data structure

---

### Phase 4: Asynchronous Logging

**Objective:** Transform to high-performance multi-threaded architecture.

| Component | Description |
|-----------|-------------|
| Thread-Safe Ring Buffer | Mutex-protected with `std::optional<T>` storage |
| Condition Variables | Efficient thread synchronization |
| Async LogManager | Non-blocking log operations |
| Thread Pool | Parallel sink writing |
| Graceful Shutdown | Clean resource cleanup |

**Threading Architecture:**

```
┌─────────────────────────────────────────────────────────────────┐
│                ASYNC LOGGING ARCHITECTURE                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  MAIN THREAD (Producer)                                         │
│  ══════════════════════                                         │
│  • Reads telemetry data                                         │
│  • Formats log messages                                         │
│  • Pushes to ring buffer (NON-BLOCKING)                         │
│  • Notifies worker via condition variable                       │
│  • Continues immediately - never waits!                         │
│                                                                 │
│                         │                                       │
│                         ▼                                       │
│              ┌─────────────────────┐                            │
│              │ Thread-Safe         │                            │
│              │ Ring Buffer         │                            │
│              │ (bounded memory)    │                            │
│              └─────────────────────┘                            │
│                         │                                       │
│                         ▼                                       │
│                                                                 │
│  FLUSHING THREAD (Consumer)                                     │
│  ══════════════════════════                                     │
│  • Waits on condition variable (no busy-waiting)                │
│  • Pops messages from buffer                                    │
│  • Submits write tasks to thread pool                           │
│                                                                 │
│                         │                                       │
│                         ▼                                       │
│                                                                 │
│  THREAD POOL (Parallel Writers)                                 │
│  ══════════════════════════════                                 │
│  • N worker threads ready                                       │
│  • Each sink write is a separate task                           │
│  • Sinks written in PARALLEL!                                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Key Learnings:**
- Multi-threaded programming with `std::thread`
- Synchronization with `std::mutex` and `std::condition_variable`
- Producer-consumer pattern
- Thread pool implementation
- Lock-free design principles
- Graceful shutdown handling

---

## 🎨 Design Patterns

### 1. Policy-Based Design

Compile-time configuration using template parameters for zero runtime overhead.

| Policy | Telemetry | Unit | WARNING Threshold | CRITICAL Threshold |
|--------|-----------|------|-------------------|-------------------|
| `CpuPolicy` | CPU Usage | % | 75.0 | 90.0 |
| `GpuPolicy` | GPU Usage | % | 80.0 | 95.0 |
| `RamPolicy` | RAM Usage | MB | 70.0 | 85.0 |

### 2. Factory Pattern

Encapsulates sink creation logic, allowing new sink types to be added without modifying client code.

### 3. Builder Pattern

Provides a fluent API for constructing LogManager with complex configurations.

### 4. RAII (Resource Acquisition Is Initialization)

Ensures resources (files, sockets, threads) are properly released even when exceptions occur.

### 5. Strategy Pattern

Allows interchangeable sink implementations through the `ILogSink` interface.

### 6. Producer-Consumer Pattern

Separates data production (main thread) from data consumption (worker thread) using a shared buffer.

### 7. Thread Pool Pattern

Reuses a fixed number of threads to execute multiple tasks, avoiding thread creation overhead.

---

## 🔑 Core Concepts

### Thread-Safe Ring Buffer

A fixed-size circular buffer that provides:

| Feature | Benefit |
|---------|---------|
| Bounded Memory | Prevents memory exhaustion under high load |
| O(1) Operations | Constant time push and pop |
| Thread Safety | Mutex-protected access |
| Optional Storage | No default constructor required for stored type |

### Condition Variables

Efficient thread synchronization mechanism:

| Aspect | Description |
|--------|-------------|
| No Busy-Waiting | Sleeping threads use 0% CPU |
| Automatic Unlock | Releases mutex while waiting |
| Spurious Wakeup Handling | Predicate function ensures correct behavior |

### Thread Pool

Pre-created worker threads that process tasks from a queue:

| Benefit | Description |
|---------|-------------|
| Reduced Overhead | Avoids repeated thread creation/destruction |
| Parallel Execution | Multiple sinks written simultaneously |
| Bounded Resources | Fixed number of threads |
| Task Queuing | Handles bursts of work gracefully |

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

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | GCC 8+, Clang 7+, or MSVC 2019+ |
| CMake | 3.16 or higher |
| Standard | C++17 |
| Threading | pthread (Linux) or Windows threads |

### Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/Telemetry_and_Logging-System.git
cd Telemetry_and_Logging-System

# Create build directory and configure
cmake -S . -B build

# Build
cmake --build build

# Run demo
./build/examples/Demo
```

### Quick Start

1. **Create formatters** for your telemetry types
2. **Create sinks** using the factory
3. **Build LogManager** with desired configuration
4. **Call log()** - it returns immediately (non-blocking)
5. **Let destructor handle cleanup** - graceful shutdown

---

## 🔧 Components

### Severity Levels

| Level | Condition | Description |
|-------|-----------|-------------|
| `INFO` | value ≤ WARNING | Normal operation |
| `WARNING` | WARNING < value ≤ CRITICAL | Attention needed |
| `CRITICAL` | value > CRITICAL | Immediate action required |

### Output Sinks

| Sink | Output | Use Case |
|------|--------|----------|
| `ConsoleSinkImpl` | `stdout` | Development, debugging |
| `FileSinkImpl` | `.log` files | Production logging, audit trails |
| Future: `NetworkSinkImpl` | TCP/UDP socket | Remote logging |
| Future: `DatabaseSinkImpl` | SQL database | Persistent storage |

### Log Message Format

```
[TIMESTAMP] <SEVERITY> (APP_NAME - CONTEXT) : MESSAGE
```

Example:
```
[2024-01-15 14:30:45] <WARNING> (TelemetryApp - CPU) : CPU usage: 78.500000%
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

### Performance Benefits

```
┌─────────────────────────────────────────────────────────────────┐
│                 PERFORMANCE COMPARISON                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  SYNCHRONOUS (Phase 3):                                         │
│  ══════════════════════                                         │
│                                                                 │
│  Main: [Read][Format][Log][WAIT 260ms][Read][Format][Log][WAIT] │
│                           ↑                                     │
│                    Blocked on I/O!                              │
│                                                                 │
│  Time per message: 260ms (10 + 50 + 200 for 3 sinks)            │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ASYNCHRONOUS (Phase 4):                                        │
│  ═══════════════════════                                        │
│                                                                 │
│  Main:   [Read][Format][Push][Read][Format][Push][Read]...      │
│                          │                                      │
│  Worker:            [Pop][Submit to Pool]                       │
│                               │                                 │
│  Pool:                 [Sink1][Sink2][Sink3] (parallel!)        │
│                                                                 │
│  Time per message: ~200ms (max of parallel sinks)               │
│  Main thread time: ~0.01ms (just push to buffer)                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---


## 📚 Topics Covered

### C++ Language Features

| Topic | Usage in Project |
|-------|------------------|
| Templates | Policy-based design, RingBuffer, LogFormatter |
| Smart Pointers | `unique_ptr` for sink ownership |
| Move Semantics | Efficient message passing |
| Lambda Expressions | Thread pool tasks, predicates |
| `std::optional` | Ring buffer storage, try_pop return |
| `std::function` | Thread pool task queue |
| `constexpr` | Compile-time policy thresholds |

### Concurrency

| Topic | Usage in Project |
|-------|------------------|
| `std::thread` | Flushing thread, thread pool workers |
| `std::mutex` | Protecting shared data |
| `std::lock_guard` | RAII mutex locking |
| `std::unique_lock` | Condition variable usage |
| `std::condition_variable` | Thread synchronization |
| `std::atomic` | Stop flags |

### Design Principles

| Principle | Application |
|-----------|-------------|
| SOLID | Interface segregation, single responsibility |
| RAII | Resource management (files, sockets, threads) |
| DRY | Policy-based design eliminates duplication |
| Composition over Inheritance | Sink strategies |

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