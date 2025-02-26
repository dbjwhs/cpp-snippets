# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(iterator)

set(CMAKE_CXX_STANDARD 20)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create a simple test program
# Change this to the name of your actual source file if different
add_executable(iterator_test src/main.cpp)