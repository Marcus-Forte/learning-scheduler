cmake_minimum_required(VERSION 3.20)
project(learning-scheduler)
set(CMAKE_CXX_STANDARD 20)

add_executable(main src/main.cc
 src/scheduler.cc)
