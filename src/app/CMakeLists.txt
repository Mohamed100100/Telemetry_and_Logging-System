cmake_minimum_required(VERSION 3.10)

project(app CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

add_library(${PROJECT_NAME} STATIC
    AppConfig.cpp
    TelemetryApp.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/third_party
)

# Optional SOME/IP
option(ENABLE_SOMEIP "Enable SOME/IP source" OFF)
if(ENABLE_SOMEIP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SOMEIP_ENABLED)
endif()

# Link against dependent libraries
target_link_libraries(${PROJECT_NAME}
    logger
    sinks
    sources
    formatter
    raii
    utils
)