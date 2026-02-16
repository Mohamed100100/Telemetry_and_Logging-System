cmake_minimum_required(VERSION 3.10)

project(Demo C CXX ASM)

set(App_Source_File "phase1_demo.cpp" CACHE STRING "Demo source file")


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/raii 
                 ${CMAKE_BINARY_DIR}/raii_build)           

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/sinks 
                 ${CMAKE_BINARY_DIR}/sinks_build)


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/logger 
                 ${CMAKE_BINARY_DIR}/logger_build)


add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/sources
                 ${CMAKE_BINARY_DIR}/sources_build)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/formatter
                 ${CMAKE_BINARY_DIR}/formatter_build)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/utils
                 ${CMAKE_BINARY_DIR}/utils_build)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/services
                 ${CMAKE_BINARY_DIR}/server_build)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../src/app 
                 ${CMAKE_BINARY_DIR}/app_build)


add_executable(${PROJECT_NAME} ${App_Source_File})


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/ PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../third_party/)
target_link_libraries(${PROJECT_NAME} raii logger sinks sources formatter utils app)

