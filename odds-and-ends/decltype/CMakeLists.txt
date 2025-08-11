# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.20)
project(decltype_deep_dive LANGUAGES CXX)

# set c++23 standard with strict compliance
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable comprehensive compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion")

# enable concepts and latest c++ features
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
endif()

# include directories for headers
include_directories(headers)
include_directories(../../../headers)

# create decltype examples library
add_library(decltype_lib
    src/decltype_examples.cpp
)

target_include_directories(decltype_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../headers
)

# main executable demonstrating decltype functionality
add_executable(decltype_demo
    src/main.cpp
)

target_link_libraries(decltype_demo decltype_lib)

# comprehensive test executable
add_executable(decltype_tests
    src/test_decltype.cpp
)

target_link_libraries(decltype_tests decltype_lib)

# enable testing with ctest
enable_testing()

# add test cases for comprehensive decltype functionality
add_test(NAME BasicDeclTypeTest 
         COMMAND decltype_tests)

add_test(NAME MainDemoTest 
         COMMAND decltype_demo)

# custom test target for all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS decltype_tests decltype_demo
    COMMENT "running comprehensive decltype test suite"
)

# custom target for running main demo
add_custom_target(run_demo
    COMMAND decltype_demo
    DEPENDS decltype_demo
    COMMENT "running decltype demonstration"
)

# set build type to debug by default for development
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
endif()

# print configuration information
message(STATUS "cmake configuration for decltype deep dive:")
message(STATUS "  cxx compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  cxx standard: c++${CMAKE_CXX_STANDARD}")
message(STATUS "  build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  compiler flags: ${CMAKE_CXX_FLAGS}")