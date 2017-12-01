cmake_minimum_required(VERSION 3.5)
project(alternating_server)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(alternating_server main.cpp)

target_link_libraries(alternating_server ${CMAKE_SOURCE_DIR}/../utils/cmake-build-debug/libutils.a)
include_directories(${CMAKE_SOURCE_DIR}/../utils)
