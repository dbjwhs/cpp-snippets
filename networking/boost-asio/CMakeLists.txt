# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(boost_asio VERSION 1.28.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# Set compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Set policy for modern Boost usage  
cmake_policy(SET CMP0167 NEW)

# Find Boost libraries
# Minimum version of 1.71.0 for most recent Asio features
# Note: boost_system is header-only in recent versions, so only thread is needed
find_package(Boost 1.70.0 REQUIRED COMPONENTS thread)

# Find Threads package for std::thread
find_package(Threads REQUIRED)

# Add source files
set(SOURCES
        src/boost_asio.cpp
)

# Add the executable
add_executable(boost_asio ${SOURCES})

# Add include directories
target_include_directories(boost_asio PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/headers  # For your project_utils.hpp
)

# Link against Boost libraries and other dependencies
target_link_libraries(boost_asio PRIVATE
        Boost::thread
        Threads::Threads
)

# Installation rules (optional)
install(TARGETS boost_asio
        RUNTIME DESTINATION bin
)

# Enable testing (optional)
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Output information about the configuration
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Boost version: ${Boost_VERSION}")