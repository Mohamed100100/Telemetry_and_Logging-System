cmake_minimum_required(VERSION 3.10)

project(Demo C CXX ASM)

set(App_Source_File "phase1_demo.cpp" CACHE STRING "Demo source file")


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/raii 
                 ${CMAKE_BINARY_DIR}/raii_build)           

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/sinks 
                 ${CMAKE_BINARY_DIR}/sinks_build)


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/core 
                 ${CMAKE_BINARY_DIR}/core_build)


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/sources
                 ${CMAKE_BINARY_DIR}/sources_build)

add_executable(${PROJECT_NAME} ${App_Source_File})


target_link_libraries(${PROJECT_NAME} raii core sinks sources)

