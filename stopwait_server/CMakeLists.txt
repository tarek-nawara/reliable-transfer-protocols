cmake_minimum_required(VERSION 3.5)
project(stopwait_server)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(stopwait_server main.cpp
        StopWaitServer.h
        StopWaitServer.cpp)

target_link_libraries(stopwait_server ${CMAKE_SOURCE_DIR}/../utils/cmake-build-debug/libutils.a)
include_directories(${CMAKE_SOURCE_DIR}/../utils)
