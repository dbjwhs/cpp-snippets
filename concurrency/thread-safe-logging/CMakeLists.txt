# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(thread_safe_logging)

set(CMAKE_CXX_STANDARD 20)

add_executable(thread_safe_logging src/thread-logger.cpp)
