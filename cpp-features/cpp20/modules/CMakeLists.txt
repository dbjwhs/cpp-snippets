# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)  # CMake 3.28+ for proper C++20 modules support
project(cpp20_modules_example LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler-specific module support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC module support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")
    # Explicitly disable module scanning since GCC doesn't provide a scanner mechanism yet
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # Clang module support - requires very recent Clang versions
    # For older Clang, we'll skip modules and build as regular C++
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "15.0")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fmodules")
    else()
        message(WARNING "C++20 modules require Clang 15+, falling back to regular C++ compilation")
    endif()
elseif(MSVC)
    # MSVC module support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:module")
endif()

# Specify source files - note the order is important for modules
set(SOURCES
        src/math_module.cppm  # Module interface must be first
        src/math_module.cpp   # Module implementation
        src/modules.cpp       # Main file that imports modules
)

# Add executable with modules
add_executable(modules_example ${SOURCES})

# Setup include directories
target_include_directories(modules_example PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../..  # For project_utils.hpp
)

# Install target
install(TARGETS modules_example
        RUNTIME DESTINATION bin
)
