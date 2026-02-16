cmake_minimum_required(VERSION 3.10)

project(sources C CXX ASM)

# Find packages
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Generated sources
set(GENERATED_SOURCES
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPProxy.cpp
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPStubAdapter.cpp
    ${PROJECT_ROOT}/fidl/src-gen/v1/log/TelemetryServiceSomeIPDeployment.cpp
)

add_library(${PROJECT_NAME} STATIC FileTelemetrySourceImpl.cpp SocketTelemetrySourceImpl.cpp SomeIPTelemetrySourceImpl.cpp  SomeIPTelemetrySourceAdapter.cpp   ${GENERATED_SOURCES})

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