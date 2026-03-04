cmake_minimum_required(VERSION 3.16)

project(TelemetryGUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# Find Qt6 (or Qt5 as fallback)
find_package(Qt6 COMPONENTS Widgets Charts Concurrent QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Charts Concurrent)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Project root directory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Header files
set(GUI_HEADERS
    include/MainWindow.hpp
    include/DashboardWidget.hpp
    include/TelemetryGaugeWidget.hpp
    include/TelemetryChartWidget.hpp
    include/LogViewerWidget.hpp
    include/ConfigDialog.hpp
    include/SourceStatusWidget.hpp
    include/TelemetryAppController.hpp
)

# Source files
set(GUI_SOURCES
    main.cpp
    src/MainWindow.cpp
    src/DashboardWidget.cpp
    src/TelemetryGaugeWidget.cpp
    src/TelemetryChartWidget.cpp
    src/LogViewerWidget.cpp
    src/ConfigDialog.cpp
    src/SourceStatusWidget.cpp
    src/TelemetryAppController.cpp
)

# Resources
set(GUI_RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_ROOT}/include
    ${PROJECT_ROOT}/third_party
)

# Link Qt libraries
if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Charts
        Qt6::Concurrent
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        Qt5::Charts
        Qt5::Concurrent
    )
endif()

# Link project libraries (build them first)
add_subdirectory(${PROJECT_ROOT}/src/logger     ${CMAKE_BINARY_DIR}/logger)
add_subdirectory(${PROJECT_ROOT}/src/sinks      ${CMAKE_BINARY_DIR}/sinks)
add_subdirectory(${PROJECT_ROOT}/src/sources    ${CMAKE_BINARY_DIR}/sources)
add_subdirectory(${PROJECT_ROOT}/src/formatter  ${CMAKE_BINARY_DIR}/formatter)
add_subdirectory(${PROJECT_ROOT}/src/raii       ${CMAKE_BINARY_DIR}/raii)
add_subdirectory(${PROJECT_ROOT}/src/app        ${CMAKE_BINARY_DIR}/app)
add_subdirectory(${PROJECT_ROOT}/src/utils      ${CMAKE_BINARY_DIR}/utils)
add_subdirectory(${PROJECT_ROOT}/src/services   ${CMAKE_BINARY_DIR}/server)

target_link_libraries(${PROJECT_NAME} PRIVATE
    app
    logger
    sinks
    sources
    formatter
    raii
    utils
)

# Optional SOME/IP support
option(ENABLE_SOMEIP "Enable SOME/IP support" OFF)
if(ENABLE_SOMEIP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE SOMEIP_ENABLED)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "Qt version: ${QT_VERSION_MAJOR}")
message(STATUS "Building TelemetryGUI...")