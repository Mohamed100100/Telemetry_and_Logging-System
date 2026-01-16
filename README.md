
# Telemetry and Logging System

A scalable, multi-threaded logging system written in C++, designed with best architectural and implementation practices while utilizing Modern C++ features and industry-standard libraries.

## 📋 Overview

This project implements a comprehensive telemetry collection and logging system that supports multiple data sources, flexible output destinations, and robust resource management.

## ✨ Features

- **Multiple Data Sources** - Support for local and remote telemetry inputs
- **Flexible Output** - Route logs to console, files, and other destinations
- **Safe Resource Management** - RAII-compliant wrappers for system resources
- **Extensible Architecture** - Easy to add new sources and sinks
- **Intuitive Formatting** - Clean, readable log output

## 🏗️ Project Structure

```
TELEMETRY_AND_LOGGING_SYSTEM/
├── docs/                        # Documentation
├── include/                     # Header files
│   ├── core/                    # Core components
│   │   ├── ILogSink.hpp
│   │   ├── ITelemetrySource.hpp
│   │   ├── LogManager.hpp
│   │   └── LogMessage.hpp
│   ├── raii/                    # Resource wrappers
│   │   ├── SafeFile.hpp
│   │   └── SafeSocket.hpp
│   ├── sinks/                   # Output implementations
│   │   ├── ConsoleSinkImpl.hpp
│   │   └── FileSinkImpl.hpp
│   └── sources/                 # Input implementations
│       ├── FileTelemetrySourceImpl.hpp
│       └── SocketTelemetrySourceImpl.hpp
├── src/                         # Implementation files
│   ├── core/
│   ├── raii/
│   ├── sinks/
│   └── sources/
├── examples/                    # Demo applications
├── scripts/                     # Utility scripts
└── README.md
```

## 📚 System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        DATA SOURCES                             │
│  ┌──────────────────┐  ┌──────────────────┐                     │
│  │ File Source      │  │ Socket Source    │                     │
│  └────────┬─────────┘  └────────┬─────────┘                     │
│           │                     │                               │
│  ┌────────▼─────────┐  ┌────────▼─────────┐                     │
│  │    SafeFile      │  │   SafeSocket     │   RAII Wrappers     │
│  └──────────────────┘  └──────────────────┘                     │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                        LOG MANAGER                              │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │  LogMessages Buffer    →    Sinks (Strategy Pattern)     │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                         OUTPUT                                  │
│         ┌──────────────┐        ┌──────────────────┐            │
│         │   Console    │        │   Log Files      │            │
│         └──────────────┘        └──────────────────┘            │
└─────────────────────────────────────────────────────────────────┘
```

## 🧩 Components

### Core

| Component | Description |
|-----------|-------------|
| `LogMessage` | Data structure for log entries |
| `LogManager` | Central routing and management |
| `ILogSink` | Interface for output destinations |
| `ITelemetrySource` | Interface for data sources |

### Sinks (Output)

| Component | Description |
|-----------|-------------|
| `ConsoleSinkImpl` | Outputs to standard output |
| `FileSinkImpl` | Writes to log files |

### Sources (Input)

| Component | Description |
|-----------|-------------|
| `FileTelemetrySourceImpl` | Reads telemetry from files |
| `SocketTelemetrySourceImpl` | Reads from Unix Domain Sockets |

### RAII Wrappers

| Component | Description |
|-----------|-------------|
| `SafeFile` | Safe file descriptor management |
| `SafeSocket` | Safe socket management |

## 🔧 Build Instructions

### Prerequisites

| Requirement | Version |
|-------------|---------|
| C++ Compiler | C++11+ |
| CMake | 3.10+ |
| OS | Linux/Unix |

### Build Steps

```bash
# Navigate to project
cd TELEMETRY_AND_LOGGING_SYSTEM

# Configure and build
cmake -S ./examples -B build

cmake --build build

# Run
./build/Demo
```

## 📖 C++ Concepts Demonstrated

| Category | Concepts |
|----------|----------|
| OOP | Virtual functions, Interfaces, Inheritance |
| Modern C++ | Move semantics, Smart pointers, RAII |
| Design | Strategy pattern, Rule of 3/5/0 |
| Systems | POSIX file I/O, Unix Domain Sockets |

## 🎯 Design Patterns

| Pattern | Type | Application |
|---------|------|-------------|
| Strategy | Behavioral | Interchangeable sink implementations |
| RAII | Idiom | Resource management wrappers |

## 📊 Log Format

```
[timestamp] <severity> (appName - context) : message
```

**Example:**
```
[2024-06-01 10:00:00] <INFO> (MyApp - Database) : Connection established.
[2024-06-01 10:05:00] <ERROR> (MyApp - Network) : Connection timeout.
[2024-06-01 10:10:00] <WARN> (MyApp - Memory) : High memory usage detected.
```

## 🔗 Dependencies

```
┌─────────────────────────────────────────┐
│            Demo Executable              │
├─────────────────────────────────────────┤
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  │
│  │  core   │  │  sinks  │  │ sources │  │
│  └────┬────┘  └────┬────┘  └────┬────┘  │
│       │            │            │       │
│       └────────────┼────────────┘       │
│                    │                    │
│              ┌─────▼─────┐              │
│              │   raii    │              │
│              └───────────┘              │
└─────────────────────────────────────────┘
```

## 📚 Documentation

Detailed documentation available in the `docs/` directory covering:

- System architecture
- Component interfaces
- Usage examples
- C++ concepts explained

## 📝 License

Educational project for demonstrating Modern C++ concepts and design patterns.