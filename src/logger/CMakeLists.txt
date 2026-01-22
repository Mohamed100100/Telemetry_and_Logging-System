cmake_minimum_required(VERSION 3.10)

project(logger C CXX ASM)

add_library(${PROJECT_NAME} STATIC LogMessage.cpp LogManager.cpp LogManagerBuilder.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/)