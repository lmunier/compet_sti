cmake_minimum_required(VERSION 3.8)
project(main_cpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp beacon.cpp beacon.h uart.cpp uart.h bottles.cpp bottles.h stepper.cpp stepper.h)
add_executable(main_cpp ${SOURCE_FILES})