# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.20)
project(cpp23_turing_machines)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments)
endif()

# Source files
set(SOURCES
        ../src/main.cpp
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../headers
)

# Define header-only libraries
set(HEADERS
        headers/basic_turing_machine.hpp
        headers/extended_turing_machine.hpp
        headers/arithmetic_turing_machine.hpp
        headers/programmable_turing_machine.hpp
        headers/comprehensive_turing_machine.hpp
        headers/universal_turing_machine.hpp
        ../../headers/project_utils.hpp
)

# Print build configuration info
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")