cmake_minimum_required(VERSION 3.5)
project(stopwait_client)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(stopwait_client main.cpp
        StopWaitClient.h
        StopWaitClient.cpp)

target_link_libraries(stopwait_client ${CMAKE_SOURCE_DIR}/../utils/cmake-build-debug/libutils.a)
include_directories(${CMAKE_SOURCE_DIR}/../utils)
