# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.23)
project(thread_affinity_example)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(headers)
include_directories(../../../headers)

# Create executable
add_executable(thread_affinity 
    src/main.cpp
)

# Link libraries
target_link_libraries(thread_affinity 
    Threads::Threads
)

# Enable testing
enable_testing()

# Add test
add_test(NAME thread_affinity_test COMMAND thread_affinity)