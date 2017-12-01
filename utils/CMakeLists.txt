cmake_minimum_required(VERSION 3.5)
set(SOURCE_FILES
  socket_utils.h
  socket_utils.cpp)

add_compile_options(-Wall -Wextra -Wpedantic)

add_library(utils STATIC ${SOURCE_FILES})

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
