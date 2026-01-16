
# Phase 2: Data Sources & Smart Resource Management

## 📋 Overview

Phase 2 extends the logging foundation with telemetry data sources and introduces critical C++ resource management concepts. This phase emphasizes **safe resource handling** through RAII and proper **move semantics**.

## 🎯 Goals

1. **Create data sources** that continuously receive data from multiple source types
2. **Implement RAII-compliant** resource wrappers for safe resource management
3. **Apply Rule of 3/5/0** for proper special member function handling

## 📚 C++ Concepts Covered

### References

| Type | Syntax | Purpose | Example Use |
|------|--------|---------|-------------|
| Lvalue Reference | `T&` | Alias to existing object | Output parameters |
| Const Reference | `const T&` | Read-only alias | Efficient input parameters |
| Rvalue Reference | `T&&` | Bind to temporaries | Move semantics |

### Move vs Copy Semantics

```
┌─────────────────────────────────────────────────────────────────┐
│                    COPY SEMANTICS                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   Source Object          Destination Object                     │
│   ┌──────────────┐      ┌──────────────┐                        │
│   │ data: [A,B,C]│ ───▶ │ data: [A,B,C]│  (duplicate created)   │
│   │ fd: 5        │      │ fd: 6        │  (new resource)        │
│   └──────────────┘      └──────────────┘                        │
│         │                      │                                │
│         ▼                      ▼                                │
│   Still valid            New copy                               │
│                                                                 │
│   Result: Two independent objects, both valid                   │
│   Cost: Expensive (full duplication)                            │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    MOVE SEMANTICS                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   Source Object          Destination Object                     │
│   ┌──────────────┐      ┌──────────────┐                        │
│   │ data: nullptr│ ◀─── │ data: [A,B,C]│  (ownership transfer)  │
│   │ fd: -1       │      │ fd: 5        │  (same resource)       │
│   └──────────────┘      └──────────────┘                        │
│         │                      │                                │
│         ▼                      ▼                                │
│   Invalid/empty          Now owns resource                      │
│                                                                 │
│   Result: One valid object, source is empty                     │
│   Cost: Cheap (pointer/handle transfer)                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

| Aspect | Copy | Move |
|--------|------|------|
| Operation | Duplicate data | Transfer ownership |
| Source after operation | Valid, unchanged | Empty/invalid |
| Performance | Expensive | Cheap |
| Use case | Need two copies | Transferring resources |

### Rule of 3/5/0

| Rule | When to Use | Members to Define |
|------|-------------|-------------------|
| **Rule of 0** | Class manages no resources | None (use defaults) |
| **Rule of 3** | Pre-C++11 resource management | Destructor, Copy Constructor, Copy Assignment |
| **Rule of 5** | C++11+ resource management | Rule of 3 + Move Constructor, Move Assignment |

**Application in This Project:**

| Class | Rule Applied | Reason |
|-------|--------------|--------|
| `LogMessage` | Rule of 0 | Only `std::string` members (self-managing) |
| `SafeFile` | Rule of 5 | Owns raw file descriptor |
| `SafeSocket` | Rule of 5 | Owns raw socket descriptor |
| `FileTelemetrySourceImpl` | Rule of 0 | Uses `std::unique_ptr<SafeFile>` |
| `SocketTelemetrySourceImpl` | Rule of 0 | Uses `std::unique_ptr<SafeSocket>` |

### Special Member Functions

| Function | Purpose | Default Behavior |
|----------|---------|------------------|
| Default Constructor | Create object with default values | Initialize members to defaults |
| Destructor | Clean up when object is destroyed | Destroy members in reverse order |
| Copy Constructor | Create object as copy of another | Member-wise copy |
| Copy Assignment | Assign one object to another | Member-wise copy assignment |
| Move Constructor | Create object by moving from another | Member-wise move |
| Move Assignment | Assign by moving from another | Member-wise move assignment |

## 🏛️ Design Idiom: RAII

**Resource Acquisition Is Initialization** (RAII) binds resource lifetime to object lifetime.

### RAII Principle

```
┌─────────────────────────────────────────────────────────────────┐
│                         RAII Principle                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌─────────────────────────────────────────────────────────┐   │
│   │                    Object Lifetime                      │   │
│   │                                                         │   │
│   │  Construction              Destruction                  │   │
│   │       │                         │                       │   │
│   │       ▼                         ▼                       │   │
│   │  ┌─────────┐              ┌─────────┐                   │   │
│   │  │ Acquire │              │ Release │                   │   │
│   │  │ Resource│              │ Resource│                   │   │
│   │  │         │              │         │                   │   │
│   │  │ open()  │   ──────▶    │ close() │                   │   │
│   │  │ malloc()│              │ free()  │                   │   │
│   │  │ lock()  │              │ unlock()│                   │   │
│   │  └─────────┘              └─────────┘                   │   │
│   │                                                         │   │
│   └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│   Benefits:                                                     │
│   ✓ No resource leaks (even with exceptions)                    │
│   ✓ Clear ownership semantics                                   │
│   ✓ Automatic cleanup                                           │
│   ✓ Exception-safe code                                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### RAII vs Manual Resource Management

```
┌─────────────────────────────────────────────────────────────────┐
│              Manual vs RAII Resource Management                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ❌ MANUAL MANAGEMENT (Error-Prone)                             │
│  ─────────────────────────────────                              │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  1. open(file)                                          │   │
│  │  2. process...                                          │   │
│  │  3. if (error) return;  ◄── LEAK! forgot close()        │   │
│  │  4. more processing...                                  │   │
│  │  5. if (error) return;  ◄── LEAK! forgot close()        │   │
│  │  6. close(file)         ◄── Only reached in happy path  │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  ✅ RAII MANAGEMENT (Safe)                                      │
│  ────────────────────────                                       │
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  1. SafeFile file(path);  ◄── Constructor: open()       │    │
│  │  2. process...                                          │    │
│  │  3. if (error) return;    ◄── Destructor: close() ✓     │    │
│  │  4. more processing...                                  │    │
│  │  5. if (error) return;    ◄── Destructor: close() ✓     │    │
│  │  6. }                     ◄── Destructor: close() ✓     │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                 │
│  RAII guarantees cleanup regardless of exit path!               │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

| Aspect | Manual Management | RAII |
|--------|-------------------|------|
| Resource acquisition | Explicit call | Constructor |
| Resource release | Explicit call (often forgotten) | Destructor (automatic) |
| Exception safety | Requires try-catch | Built-in |
| Code complexity | High | Low |
| Leak probability | High | Near zero |

## 📦 Deliverables

### Class Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                    Phase 2 Class Hierarchy                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                    INTERFACES                             │  │
│  │                                                           │  │
│  │  ┌─────────────────────────┐                              │  │
│  │  │    <<interface>>        │                              │  │
│  │  │   ITelemetrySource      │                              │  │
│  │  ├─────────────────────────┤                              │  │
│  │  │ + openSource(): bool    │                              │  │
│  │  │ + readSource(&): bool   │                              │  │
│  │  │ + ~ITelemetrySource()   │                              │  │
│  │  └───────────┬─────────────┘                              │  │
│  │              │                                            │  │
│  │      ┌───────┴────────┐                                   │  │
│  │      │                │                                   │  │
│  │      ▼                ▼                                   │  │
│  │  ┌──────────────┐  ┌───────────────┐                      │  │
│  │  │FileTelemetry │  │SocketTelemetry│                      │  │
│  │  │SourceImpl    │  │SourceImpl     │                      │  │
│  │  ├──────────────┤  ├───────────────┤                      │  │
│  │  │-FilePath     │  │-FilePath      │                      │  │
│  │  │-_safeFilePtr │  │-_safeSocketPtr│                      │  │
│  │  └──────┬───────┘  └───────┬───────┘                      │  │
│  │         │ uses             │ uses                         │  │
│  └─────────┼──────────────────┼──────────────────────────────┘  │
│            │                  │                                 │
│  ┌─────────┼──────────────────┼──────────────────────────────┐  │
│  │         │  RAII WRAPPERS   │                              │  │
│  │         ▼                  ▼                              │  │
│  │  ┌──────────────┐  ┌───────────────┐                      │  │
│  │  │  SafeFile    │  │  SafeSocket   │                      │  │
│  │  ├──────────────┤  ├───────────────┤                      │  │
│  │  │ - fd: int    │  │ - SocketFd:int│                      │  │
│  │  ├──────────────┤  ├───────────────┤                      │  │
│  │  │ + IsOpen()   │  │ + IsOpen()    │                      │  │
│  │  │ + Read()     │  │ + Read()      │                      │  │
│  │  │ + ~SafeFile()│  │ + ~SafeSocket()│                     │  │
│  │  └──────────────┘  └───────────────┘                      │  │
│  │                                                           │  │
│  │  Rule of 5: Move only, Copy deleted                       │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Component Descriptions

#### 1. ITelemetrySource Interface

| Aspect | Description |
|--------|-------------|
| **Purpose** | Defines contract for all telemetry data sources |
| **Type** | Abstract interface (pure virtual) |

| Method | Return Type | Description |
|--------|-------------|-------------|
| `openSource()` | `bool` | Opens/establishes connection to data source |
| `readSource(string&)` | `bool` | Reads data and stores in output parameter |
| `~ITelemetrySource()` | - | Virtual destructor for proper cleanup |

#### 2. SafeFile (RAII Wrapper)

| Aspect | Description |
|--------|-------------|
| **Purpose** | Wraps POSIX file operations in RAII-compliant class |
| **Resource** | File descriptor (`int fd`) |
| **Rule Applied** | Rule of 5 (Move only) |

| Member | Type | Description |
|--------|------|-------------|
| `fd` | `int` | POSIX file descriptor |

| Method | Description |
|--------|-------------|
| Constructor | Opens file, acquires file descriptor |
| Destructor | Closes file descriptor if valid |
| `IsOpen()` | Returns true if file descriptor is valid |
| `Read()` | Reads one line from file |

| Special Member | Status | Reason |
|----------------|--------|--------|
| Default Constructor | Deleted | Must provide file path |
| Copy Constructor | Deleted | Cannot share file descriptor |
| Copy Assignment | Deleted | Cannot share file descriptor |
| Move Constructor | Defaulted | Transfer ownership |
| Move Assignment | Defaulted | Transfer ownership |

#### 3. SafeSocket (RAII Wrapper)

| Aspect | Description |
|--------|-------------|
| **Purpose** | Wraps Unix Domain Socket operations in RAII-compliant class |
| **Resource** | Socket file descriptor (`int SocketFd`) |
| **Rule Applied** | Rule of 5 (Move only) |

| Member | Type | Description |
|--------|------|-------------|
| `SocketFd` | `int` | Socket file descriptor |

| Method | Description |
|--------|-------------|
| Constructor | Creates socket, connects to path |
| Destructor | Closes socket if valid |
| `IsOpen()` | Returns true if socket is connected |
| `Read()` | Reads one line from socket |

| Special Member | Status | Reason |
|----------------|--------|--------|
| Default Constructor | Deleted | Must provide socket path |
| Copy Constructor | Deleted | Cannot share socket descriptor |
| Copy Assignment | Deleted | Cannot share socket descriptor |
| Move Constructor | Defaulted | Transfer ownership |
| Move Assignment | Defaulted | Transfer ownership |

#### 4. FileTelemetrySourceImpl

| Aspect | Description |
|--------|-------------|
| **Purpose** | Reads telemetry data from files |
| **Implements** | `ITelemetrySource` |
| **Rule Applied** | Rule of 0 (uses `std::unique_ptr`) |

| Member | Type | Description |
|--------|------|-------------|
| `FilePath` | `std::string` | Path to data file |
| `_safeFilePtr` | `std::unique_ptr<SafeFile>` | RAII-managed file wrapper |

| Method | Description |
|--------|-------------|
| `openSource()` | Creates SafeFile, returns success status |
| `readSource()` | Reads line from file into output parameter |

#### 5. SocketTelemetrySourceImpl

| Aspect | Description |
|--------|-------------|
| **Purpose** | Reads telemetry data from Unix Domain Sockets |
| **Implements** | `ITelemetrySource` |
| **Rule Applied** | Rule of 0 (uses `std::unique_ptr`) |

| Member | Type | Description |
|--------|------|-------------|
| `FilePath` | `std::string` | Path to socket file |
| `_safeSocketPtr` | `std::unique_ptr<SafeSocket>` | RAII-managed socket wrapper |

| Method | Description |
|--------|-------------|
| `openSource()` | Creates SafeSocket, returns success status |
| `readSource()` | Reads line from socket into output parameter |

## 🔌 Unix Domain Sockets (UDS)

### Overview

| Aspect | Description |
|--------|-------------|
| **Type** | Inter-Process Communication (IPC) mechanism |
| **Scope** | Same machine only |
| **Address** | File system path (e.g., `/tmp/socket.sock`) |
| **Protocol** | Stream (TCP-like) or Datagram (UDP-like) |
| **Performance** | Faster than network sockets |

### Communication Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                 Unix Domain Socket Communication                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌──────────────────┐         ┌──────────────────┐             │
│   │   Server         │         │   Client         │             │
│   │   Process        │         │   Process        │             │
│   │                  │         │                  │             │
│   │  socket()        │         │  socket()        │             │
│   │     │            │         │     │            │             │
│   │  bind()          │         │     │            │             │
│   │     │            │         │     │            │             │
│   │  listen()        │         │     │            │             │
│   │     │            │         │     │            │             │
│   │  accept() ◀──────┼─────────┼── connect()      │             │
│   │     │            │         │     │            │             │
│   │  write() ────────┼─────────┼──▶ read()        │             │
│   │     │            │         │     │            │             │
│   │  close()         │         │  close()         │             │
│   └──────────────────┘         └──────────────────┘             │
│                                                                 │
│   Socket File: /tmp/telemetry_test.sock                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Server vs Client Operations

| Operation | Server | Client |
|-----------|--------|--------|
| Create socket | `socket()` | `socket()` |
| Bind to path | `bind()` | - |
| Listen | `listen()` | - |
| Connect | `accept()` | `connect()` |
| Send data | `write()` | `write()` |
| Receive data | `read()` | `read()` |
| Cleanup | `close()` | `close()` |

## 🔄 System Flow

### Data Source Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                    Telemetry Data Flow                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    DATA SOURCES                         │    │
│  │                                                         │    │
│  │   File System              Unix Domain Socket           │    │
│  │   ┌──────────┐            ┌──────────────────┐          │    │
│  │   │ data.txt │            │ telemetry.sock   │          │    │
│  │   │ CPU:45%  │            │ (socket server)  │          │    │
│  │   │ MEM:62%  │            │                  │          │    │
│  │   └────┬─────┘            └────────┬─────────┘          │    │
│  │        │                           │                    │    │
│  └────────┼───────────────────────────┼────────────────────┘    │
│           │                           │                         │
│           ▼                           ▼                         │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    RAII WRAPPERS                        │    │
│  │                                                         │    │
│  │   ┌──────────────┐        ┌──────────────────┐          │    │
│  │   │  SafeFile    │        │   SafeSocket     │          │    │
│  │   │  (manages    │        │   (manages       │          │    │
│  │   │   file fd)   │        │    socket fd)    │          │    │
│  │   └──────┬───────┘        └────────┬─────────┘          │    │
│  │          │                         │                    │    │
│  └──────────┼─────────────────────────┼────────────────────┘    │
│             │                         │                         │
│             ▼                         ▼                         │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                SOURCE IMPLEMENTATIONS                   │    │
│  │                                                         │    │
│  │   ┌──────────────────┐    ┌──────────────────────┐      │    │
│  │   │FileTelemetry     │    │SocketTelemetry       │      │    │
│  │   │SourceImpl        │    │SourceImpl            │      │    │
│  │   │                  │    │                      │      │    │
│  │   │ openSource()     │    │ openSource()         │      │    │
│  │   │ readSource()     │    │ readSource()         │      │    │
│  │   └────────┬─────────┘    └──────────┬───────────┘      │    │
│  │            │                         │                  │    │
│  └────────────┼─────────────────────────┼──────────────────┘    │
│               │                         │                       │
│               └───────────┬─────────────┘                       │
│                           │                                     │
│                           ▼                                     │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    LOG MANAGER                          │    │
│  │                                                         │    │
│  │   Telemetry data ──▶ LogMessage ──▶ Sinks               │    │
│  │                                                         │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### RAII Lifecycle

```
┌─────────────────────────────────────────────────────────────────┐
│                    SafeFile RAII Lifecycle                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Object Creation                                                │
│  ───────────────                                                │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────┐            │
│  │  Constructor: SafeFile(path)                    │            │
│  │  ┌───────────────────────────────────────────┐  │            │
│  │  │  fd = open(path, O_RDWR)                  │  │            │
│  │  │                                           │  │            │
│  │  │  if (fd == -1)                            │  │            │
│  │  │      // File open failed                  │  │            │
│  │  │  else                                     │  │            │
│  │  │      // Resource acquired ✓               │  │            │
│  │  └───────────────────────────────────────────┘  │            │
│  └─────────────────────────────────────────────────┘            │
│       │                                                         │
│       ▼                                                         │
│  Object Usage                                                   │
│  ────────────                                                   │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────┐            │
│  │  IsOpen() ──▶ Check if fd is valid              │            │
│  │  Read()   ──▶ Read line from file               │            │
│  └─────────────────────────────────────────────────┘            │
│       │                                                         │
│       ▼                                                         │
│  Object Destruction (automatic)                                 │
│  ──────────────────────────────                                 │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────────────────────────────────────────────┐            │
│  │  Destructor: ~SafeFile()                        │            │
│  │  ┌───────────────────────────────────────────┐  │            │
│  │  │  if (fd != -1)                            │  │            │
│  │  │      close(fd)  // Resource released ✓    │  │            │
│  │  └───────────────────────────────────────────┘  │            │
│  └─────────────────────────────────────────────────┘            │
│                                                                 │
│  Cleanup guaranteed regardless of:                              │
│  • Normal function return                                       │
│  • Early return due to error                                    │
│  • Exception thrown                                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 🔧 Build Instructions

### Prerequisites

| Requirement | Minimum Version | Notes |
|-------------|-----------------|-------|
| C++ Compiler | C++11 | g++, clang++ |
| CMake | 3.10 | Build system |
| Operating System | Linux/Unix | Required for POSIX and UDS |
| netcat (nc) | Any | Optional, for socket testing |

### Build Steps

```bash
# Step 1: Navigate to project root
cd TELEMETRY_AND_LOGGING_SYSTEM

# Step 2: Configure CMake for Phase 2
cmake -S ./examples -B build -DApp_Source_File=phase2_demo.cpp

# Step 3: Build
cmake --build build

# Step 3: Run
./build/Demo
```

### Testing with Socket Server

```bash
# Terminal 1: Start socket server
cd scripts
chmod +x socket_server.sh
./socket_server.sh

# Terminal 2: Run demo
cd build
./Demo
```



## 💻 Usage

### Basic Workflow

| Step | Action | Description |
|------|--------|-------------|
| 1 | Create Source | Instantiate `FileTelemetrySourceImpl` or `SocketTelemetrySourceImpl` |
| 2 | Open Source | Call `openSource()` to establish connection |
| 3 | Read Data | Call `readSource()` in loop to get telemetry |
| 4 | Create LogMessages | Convert telemetry data to log messages |
| 5 | Log and Flush | Use LogManager to output data |
| 6 | Automatic Cleanup | RAII handles resource cleanup |

### Expected Demo Output

```
============================================
         PHASE 2: Simple Test Demo          
============================================

[SETUP] Creating test data file...
[SETUP] Test data file created: /tmp/phase2_test_data.txt

---------- TEST 1: FileTelemetrySourceImpl ----------
[PASS] File source opened successfully
[INFO] Line 1: CPU:45%
[INFO] Line 2: MEM:62%
[INFO] Line 3: DISK:80%
[PASS] Read 3 lines from file
[INFO] FileSource going out of scope (RAII cleanup)...
[PASS] FileSource destroyed automatically

---------- TEST 2: Integration with LogManager ----------
[INFO] Reading telemetry and logging:
[2024-06-01 10:00:00] <INFO> (TelemetryApp - FileReader) : CPU:45%
[2024-06-01 10:00:00] <INFO> (TelemetryApp - FileReader) : MEM:62%
[2024-06-01 10:00:00] <INFO> (TelemetryApp - FileReader) : DISK:80%
[PASS] Logged all telemetry data
[INFO] Output saved to: /tmp/phase2_output.txt

---------- TEST 3: SocketTelemetrySourceImpl ----------
[INFO] Attempting to connect to socket: /tmp/telemetry_test.sock
[INFO] (Run socket_server.sh first if you want this to work)
[SKIP] Socket server not running (this is OK for testing)
[INFO] SocketSource going out of scope (RAII cleanup)...
[PASS] SocketSource destroyed automatically

---------- TEST 4: Multiple File Sources ----------
[INFO] Reading from two sources:
[INFO] Source 1 data:
[2024-06-01 10:00:00] <INFO> (App - Source1) : CPU:45%
[2024-06-01 10:00:00] <INFO> (App - Source1) : MEM:62%
[2024-06-01 10:00:00] <INFO> (App - Source1) : DISK:80%
[INFO] Source 2 data:
[PASS] Multiple sources handled correctly

---------- TEST 5: Error Handling ----------
[PASS] Correctly failed to open invalid file
[PASS] Correctly returned false for unopened source

============================================
         ALL TESTS COMPLETED                
============================================

Phase 2 Features Tested:
  [x] FileTelemetrySourceImpl
  [x] SocketTelemetrySourceImpl
  [x] RAII (automatic resource cleanup)
  [x] Integration with LogManager
  [x] Multiple sources
  [x] Error handling
```

## 🎓 Key Learning Points

### 1. RAII Benefits

```
┌─────────────────────────────────────────────────────────────────┐
│                    RAII Benefits Summary                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │  AUTOMATIC CLEANUP                                        │  │
│  │  ─────────────────                                        │  │
│  │  • Destructor called when object goes out of scope        │  │
│  │  • No need for explicit close/free/release calls          │  │
│  │  • Works even when exceptions are thrown                  │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │  EXCEPTION SAFETY                                         │  │
│  │  ────────────────                                         │  │
│  │  • Stack unwinding triggers destructors                   │  │
│  │  • Resources always released properly                     │  │
│  │  • No special try-catch needed for cleanup                │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │  CLEAR OWNERSHIP                                          │  │
│  │  ───────────────                                          │  │
│  │  • Object owns resource for its lifetime                  │  │
│  │  • Move semantics for ownership transfer                  │  │
│  │  • Copy deleted to prevent double-free                    │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2. Move vs Copy Decision

| Scenario | Use Copy | Use Move |
|----------|----------|----------|
| Simple data types | ✓ | Optional |
| Resource handles (fd, socket) | ✗ | ✓ |
| Large containers | Depends | ✓ |
| Unique ownership | ✗ | ✓ |
| Shared ownership | ✓ | ✗ |

### 3. Rule Selection Guide

```
┌─────────────────────────────────────────────────────────────────┐
│                    Rule Selection Flowchart                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Does your class manage resources directly?                     │
│       │                                                         │
│       ├── NO ──▶ RULE OF ZERO                                   │
│       │         Use = default for all or omit entirely          │
│       │         Examples: LogMessage, FileTelemetrySourceImpl   │
│       │                                                         │
│       └── YES ──▶ Does it need copying?                         │
│                        │                                        │
│                        ├── YES ──▶ RULE OF FIVE                 │
│                        │           Implement all 5 functions    │
│                        │                                        │
│                        └── NO ──▶ RULE OF FIVE (Move Only)      │
│                                   Delete copy, implement move   │
│                                   Examples: SafeFile, SafeSocket│
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4. Smart Pointer Benefits

| Feature | Raw Pointer | `std::unique_ptr` |
|---------|-------------|-------------------|
| Automatic deletion | ✗ | ✓ |
| Clear ownership | Ambiguous | Single owner |
| Move support | Manual | Built-in |
| Copy prevention | Manual | Built-in |
| Null check | Manual | Manual |
| Custom deleter | Manual | Supported |

## ⚠️ Common Pitfalls

| Pitfall | Problem | Solution |
|---------|---------|----------|
| Copying file descriptors | Double-close bug | Delete copy operations |
| Forgetting to check IsOpen() | Operations on invalid resource | Always verify before use |
| Raw pointer ownership | Memory leaks, double delete | Use `std::unique_ptr` |
| Not closing in destructor | Resource leaks | Always release in destructor |
| Using moved-from object | Undefined behavior | Don't use after move |

## 📁 File Structure

```
Phase 2 Files:
├── build/                        # Build directory
│
├── include/
│   ├── core/
│   │   └── ITelemetrySource.hpp  # Source interface
│   ├── raii/
│   │   ├── SafeFile.hpp          # RAII file wrapper
│   │   └── SafeSocket.hpp        # RAII socket wrapper
│   └── sources/
│       ├── FileTelemetrySourceImpl.hpp   # File source
│       └── SocketTelemetrySourceImpl.hpp # Socket source
│
├── src/
│   ├── raii/
│   │   ├── SafeFile.cpp          # SafeFile implementation
│   │   └── SafeSocket.cpp        # SafeSocket implementation
│   └── sources/
│       ├── FileTelemetrySourceImpl.cpp   # File source impl
│       └── SocketTelemetrySourceImpl.cpp # Socket source impl
│
├── scripts/
│   └── socket_server.sh          # Socket server for testing
│
└── examples/
    └── phase2_demo.cpp           # Usage demonstration
```

## 🔗 Integration with Phase 1

```
┌─────────────────────────────────────────────────────────────────┐
│                 Phase 1 + Phase 2 Integration                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  PHASE 2 COMPONENTS              PHASE 1 COMPONENTS             │
│  ───────────────────             ──────────────────             │
│                                                                 │
│  ┌─────────────────┐                                            │
│  │ Telemetry       │                                            │
│  │ Sources         │                                            │
│  │                 │                                            │
│  │ • File          │──── telemetry ────┐                        │
│  │ • Socket        │      data         │                        │
│  └─────────────────┘                   │                        │
│                                        ▼                        │
│                              ┌─────────────────┐                │
│                              │   LogMessage    │                │
│                              │   (data struct) │                │
│                              └────────┬────────┘                │
│                                       │                         │
│                                       ▼                         │
│                              ┌─────────────────┐                │
│                              │   LogManager    │                │
│                              │   (routing)     │                │
│                              └────────┬────────┘                │
│                                       │                         │
│                          ┌────────────┴────────────┐            │
│                          │                         │            │
│                          ▼                         ▼            │
│                 ┌─────────────────┐      ┌─────────────────┐    │
│                 │ ConsoleSinkImpl │      │  FileSinkImpl   │    │
│                 │ (stdout output) │      │ (file output)   │    │
│                 └─────────────────┘      └─────────────────┘    │
│                                                                 │
│  Complete Data Flow:                                            │
│  Data Source ──▶ SafeFile/Socket ──▶ TelemetrySource            │
│       ──▶ LogMessage ──▶ LogManager ──▶ Sinks ──▶ Output        │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 🎁 Bonus Challenge

### System Telemetry from /proc

| File | Content | Use Case |
|------|---------|----------|
| `/proc/stat` | CPU statistics | Monitor CPU usage |
| `/proc/meminfo` | Memory information | Monitor RAM usage |

**Challenge:** Parse these files using `FileTelemetrySourceImpl` to collect real system metrics.

## 📚 Further Reading

| Topic | Resource |
|-------|----------|
| RAII | cppreference.com/w/cpp/language/raii |
| Move Semantics | cppreference.com/w/cpp/language/move_constructor |
| Rule of Three/Five/Zero | cppreference.com/w/cpp/language/rule_of_three |
| Unix Domain Sockets | man7.org/linux/man-pages/man7/unix.7.html |
| std::unique_ptr | cppreference.com/w/cpp/memory/unique_ptr |
| POSIX File Operations | man7.org/linux/man-pages/man2/open.2.html |
| POSIX read() | man7.org/linux/man-pages/man2/read.2.html |
| POSIX close() | man7.org/linux/man-pages/man2/close.2.html |