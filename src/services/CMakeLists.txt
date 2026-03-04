cmake_minimum_required(VERSION 3.10)
project(ServerSomeIp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# Configuration Options
# ============================================================
option(BUILD_FOR_RPI "Build for Raspberry Pi (cross-compile)" OFF)

# Base path for CommonAPI libraries
set(COMMONAPI_BASE_PATH "/media/gemy/Linux_Workspace/ITI/Eng_Fady/rpi-commonapi" 
    CACHE PATH "Path to CommonAPI libraries")

# ============================================================
# Cross-compilation setup
# ============================================================
if(BUILD_FOR_RPI)
    message(STATUS "=== Building for Raspberry Pi ===")
    
    # Toolchain settings
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_CROSSCOMPILING TRUE)
    
    set(TOOLCHAIN_PREFIX "$ENV{HOME}/x-tools/aarch64-rpi3-linux-gnu")
    set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}/bin/aarch64-rpi3-linux-gnu-gcc")
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}/bin/aarch64-rpi3-linux-gnu-g++")
    set(CMAKE_SYSROOT "${TOOLCHAIN_PREFIX}/aarch64-rpi3-linux-gnu/sysroot")
    
    # Search settings
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
    
    # Paths
    set(STAGE_DIR "${COMMONAPI_BASE_PATH}/stage")
    set(COMMONAPI_CORE_STAGE "${COMMONAPI_BASE_PATH}/capicxx-core-runtime-3.2.4/stage")
    set(BOOST_LIB "${COMMONAPI_BASE_PATH}/sysroot/usr/local/lib")
    set(SYSROOT_DIR "${COMMONAPI_BASE_PATH}/sysroot")
    
    # Set CMake prefix paths
    set(CMAKE_PREFIX_PATH 
        ${STAGE_DIR}
        ${COMMONAPI_CORE_STAGE}
        ${SYSROOT_DIR}/usr/local
    )
    
    # Package directories
    set(CommonAPI_DIR "${COMMONAPI_CORE_STAGE}/lib/cmake/CommonAPI-3.2.4")
    set(CommonAPI-SomeIP_DIR "${STAGE_DIR}/lib/cmake/CommonAPI-SomeIP-3.2.4")
    set(vsomeip3_DIR "${STAGE_DIR}/lib/cmake/vsomeip3")
    
    # Compiler flags
    add_compile_options(-include string)
    
    # Linker flags for Boost and stage libraries
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
        -L${BOOST_LIB} \
        -L${STAGE_DIR}/lib \
        -L${COMMONAPI_CORE_STAGE}/lib \
        -Wl,-rpath-link,${BOOST_LIB} \
        -Wl,-rpath-link,${STAGE_DIR}/lib \
        -Wl,-rpath-link,${COMMONAPI_CORE_STAGE}/lib")
    
else()
    message(STATUS "=== Building for PC (native) ===")
endif()

# ============================================================
# Find packages
# ============================================================
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

message(STATUS "CommonAPI found: ${COMMONAPI_INCLUDE_DIRS}")
message(STATUS "CommonAPI-SomeIP found: ${COMMONAPI_SOMEIP_INCLUDE_DIRS}")

# ============================================================
# Project configuration
# ============================================================
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Generated sources
set(GENERATED_SOURCES
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPProxy.cpp
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPStubAdapter.cpp
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPDeployment.cpp
)

# Server executable
add_executable(${PROJECT_NAME}
    TelemetryServiceImpl.cpp
    TelemetryServiceRunner.cpp
    ${GENERATED_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/fidl
    ${PROJECT_ROOT}/fidl/src-gen
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_SOMEIP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    CommonAPI
    CommonAPI-SomeIP
    vsomeip3
    pthread
)
