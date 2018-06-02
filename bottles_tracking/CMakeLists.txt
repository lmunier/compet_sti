cmake_minimum_required(VERSION 3.8)
project(bottles_tracking)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp b_tracking.cpp b_tracking.h)
add_executable(bottles_tracking ${SOURCE_FILES} b_tracking.cpp b_tracking.h)