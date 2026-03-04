# Build & Run Guide

```markdown
# Telemetry System - Build & Run Guide

## Prerequisites
- CMake 3.16+
- Qt6 (for GUI)
- CommonAPI & vsomeip (for SOME/IP)

---

## 1. Local CLI + Local SOME/IP Server

```bash
# Build
cmake -S ./examples -B build -DApp_Source_File=phase6_demo.cpp -DENABLE_SOMEIP=ON
cmake --build build

# Terminal 1: Start Server
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=server
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-local.json
./build/server_build/ServerSomeIp

# Terminal 2: Start Data Sources
./scripts/cpu_logger.sh &
./scripts/gpu_logger.sh &
./scripts/socket_server.sh &

# Terminal 3: Run Client
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=client
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-local.json
./build/bin/phase6_demo
```

---

## 2. Local GUI + Local SOME/IP Server

```bash
# Build
cmake -S gui -B gui/build -DENABLE_SOMEIP=ON
cmake --build gui/build

# Terminal 1: Start Server
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=server
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-local.json
./gui/build/server/ServerSomeIp

# Terminal 2: Start Data Sources
./scripts/cpu_logger.sh &
./scripts/gpu_logger.sh &

# Terminal 3: Run GUI
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=client
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-local.json
./gui/build/bin/TelemetryGUI
```

---

## 3. Raspberry Pi CLI (RPi as Client, PC as Server)

### On Host PC (Server)

```bash
# Build
cmake -S ./examples -B build -DApp_Source_File=phase6_demo.cpp -DENABLE_SOMEIP=ON
cmake --build build

# Setup network
sudo ip route add 224.224.224.245/32 dev eno1

# Start Server & Data Sources
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=server
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-server.json
./scripts/cpu_logger.sh &
./scripts/gpu_logger.sh &
./build/server_build/ServerSomeIp
```

### On Raspberry Pi (Client)

```bash
# Build (on RPi)
cmake -S ./examples -B build -DApp_Source_File=phase6_demo.cpp -DBUILD_FOR_RPI=ON -DENABLE_SOMEIP=ON
cmake --build build

# Run Client
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=client
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-client.json
./build/bin/phase6_demo
```

---

## 4. Raspberry Pi GUI (RPi as Client, PC as Server)

### On Host PC (Server)

```bash
# Build
cmake -S gui -B gui/build -DENABLE_SOMEIP=ON
cmake --build gui/build

# Setup network
sudo ip route add 224.224.224.245/32 dev eno1

# Start Server & Data Sources
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=server
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-server.json
./scripts/cpu_logger.sh &
./scripts/gpu_logger.sh &
./gui/build/server/ServerSomeIp
```

### On Raspberry Pi (Client)

```bash
# Build (on RPi)
cmake -S gui -B gui/build -DBUILD_FOR_RPI=ON -DENABLE_SOMEIP=ON
cmake --build gui/build

# Run GUI
export COMMONAPI_CONFIG=$(pwd)/config/commonapi.ini
export VSOMEIP_APPLICATION_NAME=client
export VSOMEIP_CONFIGURATION=$(pwd)/config/vsomeip-client.json
./gui/build/bin/TelemetryGUI
```

---

## Optional: GPU Stress Test

```bash
__NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia glmark2
```

---

## Network Info

| Device | IP |
|--------|-----|
| Host PC | 10.42.0.1 |
| Raspberry Pi | 10.42.0.73 |

```bash
# SSH to RPi
ssh pi@10.42.0.73
```
```
