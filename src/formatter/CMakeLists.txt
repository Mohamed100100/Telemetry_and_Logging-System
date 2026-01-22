cmake_minimum_required(VERSION 3.10)

project(formatter C CXX ASM)

add_library(${PROJECT_NAME} STATIC LogFormatterHelper.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/)