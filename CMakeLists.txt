cmake_minimum_required(VERSION 3.20)
project(learning-scheduler)
set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(format)

add_executable(main src/main.cc
 src/scheduler.cc)
