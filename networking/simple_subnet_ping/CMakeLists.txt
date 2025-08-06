# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.31)
project(simple_subnet_ping)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find Boost in system first
find_package(Boost 1.70 QUIET COMPONENTS system)

if(NOT Boost_FOUND)
    message(STATUS "System Boost not found. Using FetchContent to download Boost...")
    
    # Include FetchContent module for downloading dependencies
    include(FetchContent)
    
    # Download and build Boost
    FetchContent_Declare(
        Boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
        URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
    )
    
    FetchContent_MakeAvailable(Boost)
    
    # Create alias targets for consistency
    if(TARGET boost_system)
        add_library(Boost::system ALIAS boost_system)
        set(Boost_LIBRARIES boost_system)
        set(Boost_INCLUDE_DIRS ${boost_SOURCE_DIR})
    endif()
else()
    message(STATUS "Using system Boost ${Boost_VERSION}")
endif()

# Enable threading
find_package(Threads REQUIRED)

# Include directories
include_directories(headers)
include_directories(../../../headers)

if(NOT Boost_FOUND)
    # For downloaded Boost, we need to manually add all include directories
    include_directories(${boost_SOURCE_DIR})
    include_directories(${boost_SOURCE_DIR}/libs/asio/include)
    include_directories(${boost_SOURCE_DIR}/libs/system/include)
    include_directories(${boost_SOURCE_DIR}/libs/align/include)
    include_directories(${boost_SOURCE_DIR}/libs/core/include)
    include_directories(${boost_SOURCE_DIR}/libs/assert/include)
    include_directories(${boost_SOURCE_DIR}/libs/config/include)
    include_directories(${boost_SOURCE_DIR}/libs/type_traits/include)
    include_directories(${boost_SOURCE_DIR}/libs/throw_exception/include)
    include_directories(${boost_SOURCE_DIR}/libs/smart_ptr/include)
    include_directories(${boost_SOURCE_DIR}/libs/array/include)
    include_directories(${boost_SOURCE_DIR}/libs/date_time/include)
    include_directories(${boost_SOURCE_DIR}/libs/chrono/include)
    include_directories(${boost_SOURCE_DIR}/libs/utility/include)
    include_directories(${boost_SOURCE_DIR}/libs/integer/include)
    include_directories(${boost_SOURCE_DIR}/libs/static_assert/include)
    include_directories(${boost_SOURCE_DIR}/libs/predef/include)
    include_directories(${boost_SOURCE_DIR}/libs/winapi/include)
    include_directories(${boost_SOURCE_DIR}/libs/numeric/conversion/include)
    include_directories(${boost_SOURCE_DIR}/libs/mpl/include)
    include_directories(${boost_SOURCE_DIR}/libs/preprocessor/include)
else()
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Define BOOST_ALL_NO_LIB to avoid auto-linking on Windows
add_definitions(-DBOOST_ALL_NO_LIB)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "headers/*.hpp")

# Create main executable
add_executable(simple_subnet_ping ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(simple_subnet_ping 
    ${Boost_LIBRARIES}
    Threads::Threads
)

# Set compiler flags for C++23 features
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(simple_subnet_ping PRIVATE -Wall -Wextra -pedantic)
endif()

# Create test executable
add_executable(subnet_ping_tests ${SOURCES} ${HEADERS})

target_link_libraries(subnet_ping_tests 
    ${Boost_LIBRARIES}
    Threads::Threads
)

target_include_directories(subnet_ping_tests PRIVATE headers ../../../headers)
# target_compile_definitions(subnet_ping_tests PRIVATE TESTING_MODE=1)