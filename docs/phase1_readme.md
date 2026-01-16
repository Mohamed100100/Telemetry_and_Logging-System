# Phase 1: Core Synchronous Logging Foundation

## 📋 Overview

Phase 1 establishes the foundational architecture for the Telemetry and Logging System. This phase focuses on creating a minimal but functional logging system following Agile methodology principles - delivering working software incrementally.

## 🎯 Goals

1. **Create a concrete skeleton** for the project upon which everything will be built
2. **Provide a minimalist version** of the overall functionality of the final product
3. **Follow Agile Methodology** practices with incremental development

## 📚 C++ Concepts Covered

### Keywords

| Keyword | Purpose | Usage in Project |
|---------|---------|------------------|
| `virtual` | Enable runtime polymorphism | `ILogSink::write()` |
| `default` | Use compiler-generated implementation | Destructors, constructors |
| `public` | Public access specifier | Interface methods |
| `protected` | Protected access specifier | Inheritance hierarchy |
| `private` | Private access specifier | Internal data members |

### Object-Oriented Concepts

| Concept | Description | Application |
|---------|-------------|-------------|
| Classes and Interfaces | Blueprint for objects with pure virtual functions | `ILogSink` as pure interface |
| Inheritance | Derive new classes from existing ones | `ConsoleSinkImpl` extends `ILogSink` |
| Virtual Functions | Enable runtime method resolution | Polymorphic `write()` calls |
| Virtual Destructors | Ensure proper cleanup in hierarchies | `~ILogSink()` declared virtual |
| Operator Overloading | Custom behavior for operators | `ToString()` for formatting |

## 🏛️ Design Pattern: Strategy

The **Strategy Pattern** is a behavioral design pattern that enables selecting an algorithm's behavior at runtime.

### Pattern Structure

```
┌─────────────────────────────────────────────────────────────────┐
│                    Strategy Pattern                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────┐         ┌──────────────────────────────┐       │
│  │  Context    │         │     Strategy Interface       │       │
│  │             │ uses    │                              │       │
│  │ LogManager  │────────▶│        ILogSink             │        │
│  │             │         │  + write(LogMessage) : void │        │
│  └─────────────┘         └──────────────┬───────────────┘       │
│                                         │                       │
│                          ┌──────────────┴───────────────┐       │
│                          │                              │       │
│              ┌───────────▼────────┐    ┌───────────────▼─┐      │
│              │ ConsoleSinkImpl    │    │ FileSinkImpl    │      │
│              │                    │    │                 │      │
│              │ + write(msg)       │    │ + write(msg)    │      │
│              │   prints to stdout │    │   writes to file│      │
│              └────────────────────┘    └─────────────────┘      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Benefits

| Benefit | Description |
|---------|-------------|
| Open/Closed Principle | Add new sinks without modifying existing code |
| Single Responsibility | Each sink handles one output method |
| Runtime Flexibility | Change logging destinations dynamically |
| Testability | Easy to mock sinks for unit testing |

## 📦 Deliverables

### Class Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    Phase 1 Class Hierarchy                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│                    ┌──────────────────────┐                     │
│                    │     <<interface>>    │                     │
│                    │      ILogSink        │                     │
│                    ├──────────────────────┤                     │
│                    │ + write(LogMessage)  │ pure virtual        │
│                    │ + ~ILogSink()        │ virtual default     │
│                    └──────────┬───────────┘                     │
│                               │                                 │
│               ┌───────────────┴───────────────┐                 │
│               │                               │                 │
│               ▼                               ▼                 │
│  ┌────────────────────────┐    ┌────────────────────────┐       │
│  │   ConsoleSinkImpl      │    │    FileSinkImpl        │       │
│  ├────────────────────────┤    ├────────────────────────┤       │
│  │                        │    │ - FilePath: string     │       │
│  ├────────────────────────┤    ├────────────────────────┤       │
│  │ + write(LogMessage)    │    │ + FileSinkImpl(path)   │       │
│  │ + ~ConsoleSinkImpl()   │    │ + write(LogMessage)    │       │
│  └────────────────────────┘    │ + ~FileSinkImpl()      │       │
│                                └────────────────────────┘       │
│                                                                 │
│  ┌────────────────────────┐    ┌─────────────────────────┐      │
│  │     LogMessage         │    │     LogManager          │      │
│  ├────────────────────────┤    ├─────────────────────────┤      │
│  │ - appName: string      │    │ - SinksBuffer: vector   │      │
│  │ - context: string      │    │ - LogMessagesBuffer:    │      │
│  │ - severity: string     │    │   vector                │      │
│  │ - time: string         │    ├─────────────────────────┤      │
│  │ - message: string      │    │ + addSink(ILogSink*)    │      │
│  ├────────────────────────┤    │ + removeSink(ILogSink*) │      │
│  │ + LogMessage(...)      │    │ + log(LogMessage)       │      │
│  │ + ToString(): string   │    │ + flush()               │      │
│  └────────────────────────┘    │ + DeleteAllSinks()      │      │
│                                │ + DeleteAllLogMessages()│      │
│                                └─────────────────────────┘      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Component Descriptions

#### 1. ILogSink Interface

| Aspect | Description |
|--------|-------------|
| **Purpose** | Defines contract for all log output destinations |
| **Type** | Abstract interface (pure virtual) |
| **Key Method** | `write()` - outputs a log message |
| **Destructor** | Virtual and defaulted for proper polymorphic cleanup |

#### 2. LogMessage Data Structure

| Field | Type | Description |
|-------|------|-------------|
| `appName` | string | Application identifier |
| `context` | string | Logging context or module name |
| `severity` | string | Log level (INFO, WARN, ERROR) |
| `time` | string | Timestamp of the log event |
| `message` | string | Actual log content |

**Output Format:**
```
[timestamp] <severity> (appName - context) : message
```

**Example:**
```
[2024-06-01 10:00:00] <INFO> (MyApp - Initialization) : Application started successfully.
```

#### 3. LogManager Class

| Method | Description |
|--------|-------------|
| `addSink()` | Registers a new sink for log output |
| `removeSink()` | Unregisters an existing sink |
| `log()` | Buffers a log message for later output |
| `flush()` | Writes all buffered messages to all sinks |
| `DeleteAllSinks()` | Removes all registered sinks |
| `DeleteAllLogMessages()` | Clears the message buffer |

#### 4. ConsoleSinkImpl

| Aspect | Description |
|--------|-------------|
| **Purpose** | Outputs log messages to standard output (console) |
| **Inherits** | `ILogSink` |
| **Behavior** | Prints formatted log message followed by newline |

#### 5. FileSinkImpl

| Aspect | Description |
|--------|-------------|
| **Purpose** | Outputs log messages to a file |
| **Inherits** | `ILogSink` |
| **Configuration** | Requires file path at construction |
| **Behavior** | Appends formatted log message to specified file |

## 🔄 System Flow

### LogManager Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        LogManager Flow                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│           ┌────────────────────────────────────────────┐        │
│           │              LogManager                    │        │
│           │                                            │        │
│  log() ──▶│  ┌──────────────────────────────────┐      │        │
│           │  │     LogMessagesBuffer            │      │        │
│           │  │  [msg1] [msg2] [msg3] [msg4]     │      │        │
│           │  └──────────────────────────────────┘      │        │
│           │                    │                       │        │
│           │              flush()                       │        │
│           │                    ▼                       │        │
│           │  ┌──────────────────────────────────┐      │        │
│           │  │        SinksBuffer               │      │        │
│           │  │  ┌────────┐  ┌────────┐          │      │        │
│           │  │  │Console │  │ File   │ ...      │      │        │
│           │  │  │ Sink   │  │ Sink   │          │      │        │
│           │  │  └───┬────┘  └───┬────┘          │      │        │
│           │  └──────┼───────────┼───────────────┘      │        │
│           └─────────┼───────────┼──────────────────────┘        │
│                     ▼           ▼                               │
│                  stdout      file.log                           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Flush Execution Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    flush() Execution Flow                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  LogMessagesBuffer: [msg1, msg2, msg3]                          │
│  SinksBuffer: [ConsoleSink, FileSink]                           │
│                                                                 │
│  Iteration:                                                     │
│                                                                 │
│  msg1 ──▶ ConsoleSink.write(msg1) ──▶ stdout                    │
│       └─▶ FileSink.write(msg1) ──▶ logs.txt                     │
│                                                                 │
│  msg2 ──▶ ConsoleSink.write(msg2) ──▶ stdout                    │
│       └─▶ FileSink.write(msg2) ──▶ logs.txt                     │
│                                                                 │
│  msg3 ──▶ ConsoleSink.write(msg3) ──▶ stdout                    │
│       └─▶ FileSink.write(msg3) ──▶ logs.txt                     │
│                                                                 │
│  LogMessagesBuffer.clear() ──▶ []                               │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 🔧 Build Instructions

### Prerequisites

| Requirement | Minimum Version |
|-------------|-----------------|
| C++ Compiler | C++11 (g++, clang++) |
| CMake | 3.10 |
| Operating System | Linux/Unix |

### Build Steps

```bash
# Step 1: Navigate to project root
cd TELEMETRY_AND_LOGGING_SYSTEM

# Step 2: Configure CMake for Phase 1
cmake -S ./examples -B build -DApp_Source_File=phase1_demo.cpp

# Step 3: Build
 cmake --build build

# Step 4: Run
./build/Demo
```

## 💻 Usage

### Basic Workflow

| Step | Action | Description |
|------|--------|-------------|
| 1 | Create LogMessages | Instantiate messages with app name, context, severity, time, and content |
| 2 | Initialize LogManager | Create the central manager instance |
| 3 | Add Sinks | Register output destinations (console, file, etc.) |
| 4 | Log Messages | Buffer messages using `log()` method |
| 5 | Flush | Write all buffered messages to all sinks |
| 6 | Cleanup | Delete sinks and clear buffers |

### Expected Output

**Console Output:**
```
[2024-06-01 10:00:00] <INFO> (MyApp - Initialization) : Application started successfully.
[2024-06-01 10:05:00] <ERROR> (MyApp - Database) : Failed to connect to the database.
[2024-06-01 10:10:00] <WARN> (MyApp - UserModule) : User profile is incomplete.
```

**File Output (logs.txt):**
```
[2024-06-01 10:00:00] <INFO> (MyApp - Initialization) : Application started successfully.
[2024-06-01 10:05:00] <ERROR> (MyApp - Database) : Failed to connect to the database.
[2024-06-01 10:10:00] <WARN> (MyApp - UserModule) : User profile is incomplete.
```

## 🎓 Key Learning Points

### 1. Virtual Functions and Polymorphism

```
┌─────────────────────────────────────────────────────────────────┐
│                    Virtual Function Dispatch                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Base Pointer ──▶ Derived Object                                │
│                                                                 │
│  ┌─────────────────┐      ┌─────────────────────────────┐       │
│  │ sink (ILogSink*)│─────▶│   ConsoleSinkImpl Object    │       │
│  └─────────────────┘      │  ┌───────────────────────┐  │       │
│                           │  │ vptr ─────────────────┼──┼───┐   │
│                           │  └───────────────────────┘  │   │   │
│                           └─────────────────────────────┘   │   │
│                                                             │   │
│  sink->write(msg)                                           │   │
│       │                                                     │   │
│       │   ┌──────────────────────────────────────────────┐  │   │
│       │   │            Virtual Table (vtable)            │◀─┘   │
│       │   ├──────────────────────────────────────────────┤      │
│       └──▶│ write() ──▶ ConsoleSinkImpl::write()        │       │
│           │ ~ILogSink() ──▶ ConsoleSinkImpl::~()        │       │
│           └──────────────────────────────────────────────┘      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

| Concept | Explanation |
|---------|-------------|
| Virtual Table (vtable) | Table of function pointers for virtual methods |
| vptr | Hidden pointer in each object pointing to its vtable |
| Dynamic Dispatch | Runtime resolution of which method to call |

### 2. Pure Virtual Functions

| Aspect | Description |
|--------|-------------|
| Syntax | `= 0` after function declaration |
| Effect | Makes class abstract (cannot be instantiated) |
| Purpose | Forces derived classes to provide implementation |

### 3. Virtual Destructors

```
┌─────────────────────────────────────────────────────────────────┐
│                    Destructor Call Order                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  With Virtual Destructor:                                       │
│                                                                 │
│  delete sink;  (where sink points to FileSinkImpl)              │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────┐                                    │
│  │ ~FileSinkImpl()         │  ◄── Derived destructor first      │
│  │   (cleanup FilePath)    │                                    │
│  └───────────┬─────────────┘                                    │
│              │                                                  │
│              ▼                                                  │
│  ┌─────────────────────────┐                                    │
│  │ ~ILogSink()             │  ◄── Base destructor second        │
│  │   (cleanup base)        │                                    │
│  └─────────────────────────┘                                    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

| Scenario | Without Virtual | With Virtual |
|----------|-----------------|--------------|
| Delete through base pointer | Only base destructor called | Both destructors called |
| Result | Memory/resource leak | Proper cleanup |

### 4. Defaulted and Deleted Special Member Functions

| Keyword | Purpose | Use Case |
|---------|---------|----------|
| `= default` | Use compiler-generated implementation | Simple types with no special logic |
| `= delete` | Prevent use of that function | Disable copying for resource-owning classes |

## 📊 Strategy Pattern Benefits

```
┌─────────────────────────────────────────────────────────────────┐
│              Adding New Sink (Strategy Pattern)                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  BEFORE: Two sinks                                              │
│                                                                 │
│  LogManager                                                     │
│  ┌─────────────────────────────────────────┐                    │
│  │ SinksBuffer:                            │                    │
│  │   [ConsoleSinkImpl] [FileSinkImpl]      │                    │
│  └─────────────────────────────────────────┘                    │
│                                                                 │
│  ADDING: New DatabaseSinkImpl (hypothetical)                    │
│                                                                 │
│  Steps:                                                         │
│  1. Create new class implementing ILogSink                      │
│  2. Override write() method                                     │
│  3. Add to LogManager                                           │
│                                                                 │
│  NO CHANGES to LogManager, ConsoleSinkImpl, or FileSinkImpl!    │
│                                                                 │
│  AFTER: Three sinks                                             │
│                                                                 │
│  LogManager                                                     │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ SinksBuffer:                                             │   │
│  │   [ConsoleSinkImpl] [FileSinkImpl] [DatabaseSinkImpl]    │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## ⚠️ Common Pitfalls

| Pitfall | Problem | Solution |
|---------|---------|----------|
| Non-virtual destructor | Memory leak when deleting through base pointer | Always make base class destructor virtual |
| Object slicing | Passing derived object by value to base type | Use pointers or references for polymorphism |
| Raw pointer ownership | Unclear who is responsible for deletion | Use smart pointers (`std::unique_ptr`) |
| Forgetting to flush | Messages remain in buffer, never output | Always call `flush()` before cleanup |

## 📁 File Structure

```
Phase 1 Files:
├── build/                       # Build Directory
│
├── include/
│   ├── core/
│   │   ├── ILogSink.hpp         # Sink interface
│   │   ├── LogManager.hpp       # Manager class declaration
│   │   └── LogMessage.hpp       # Message structure
│   └── sinks/
│       ├── ConsoleSinkImpl.hpp  # Console sink declaration
│       └── FileSinkImpl.hpp     # File sink declaration
│
├── src/
│   ├── core/
│   │   ├── LogManager.cpp       # Manager implementation
│   │   └── LogMessage.cpp       # Message implementation
│   └── sinks/
│       ├── ConsoleSinkImpl.cpp  # Console sink implementation
│       └── FileSinkImpl.cpp     # File sink implementation
│
└── examples/
    └── phase1_demo.cpp          # Usage demonstration
```

## 🔜 Next Steps (Phase 2 Preview)

| Topic | Description |
|-------|-------------|
| RAII | Resource Acquisition Is Initialization for safe resource management |
| Move Semantics | Efficient transfer of resources between objects |
| Rule of 3/5/0 | Guidelines for special member function implementation |
| Telemetry Sources | Data input from files and sockets |

## 📚 Further Reading

| Topic | Resource |
|-------|----------|
| Virtual Functions | cppreference.com/w/cpp/language/virtual |
| Strategy Pattern | refactoring.guru/design-patterns/strategy |
| Smart Pointers | cppreference.com/w/cpp/memory/unique_ptr |
| Inheritance | cppreference.com/w/cpp/language/derived_class |
| Abstract Classes | cppreference.com/w/cpp/language/abstract_class |
```