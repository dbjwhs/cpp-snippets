# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.31)
project(backtrace)

set(CMAKE_CXX_STANDARD 23)

# Enable debug symbols for better stack traces
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O2")

add_executable(backtrace main.cpp)
