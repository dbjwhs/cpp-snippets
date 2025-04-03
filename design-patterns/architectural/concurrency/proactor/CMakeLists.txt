# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(proactor)

set(CMAKE_CXX_STANDARD 20)

# Set include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    # Core components
    src/core/error.cpp
    src/core/async_operation.cpp
    src/core/event_queue.cpp
    src/core/proactor.cpp
    src/core/logger.cpp
    
    # Network components
    src/network/socket.cpp
    
    # Operations
    src/operations/async_accept_operation.cpp
    src/operations/async_connect_operation.cpp
    src/operations/async_read_operation.cpp
    src/operations/async_write_operation.cpp
    
    # Tests
    src/testing/tests.cpp
    
    # Main entry point
    src/main.cpp
    
    # Still using original file during transition
    # Will be removed once all classes are extracted
    src/proactor.cpp
)

# Add executable
add_executable(proactor ${SOURCES})