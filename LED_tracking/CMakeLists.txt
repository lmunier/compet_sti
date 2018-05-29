cmake_minimum_required(VERSION 3.8)
project(LED_tracking)

set(CMAKE_CXX_STANDARD 11)

include_directories("${PROJECT_SOURCE_DIR}")

set(SOURCE_FILES main.cpp tracking.h tracking.cpp CheapStepper.cpp CheapStepper.h)
add_executable(LED_tracking ${SOURCE_FILES})