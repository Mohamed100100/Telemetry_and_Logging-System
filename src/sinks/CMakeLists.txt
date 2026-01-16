cmake_minimum_required(VERSION 3.10)

project(sinks C CXX ASM)

add_library(${PROJECT_NAME} STATIC ConsoleSinkImpl.cpp FileSinkImpl.cpp) 

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/)