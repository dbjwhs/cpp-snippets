cmake_minimum_required(VERSION 3.28)
project(multiton_pattern_demo VERSION 1.0.0 LANGUAGES CXX)

# set c++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable testing
enable_testing()

# find required packages
find_package(Threads REQUIRED)

# compiler specific flags for optimization and warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wno-gnu-zero-variadic-macro-arguments")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG_MODE")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
endif()

# include directories
include_directories(${CMAKE_SOURCE_DIR}/headers)
include_directories(${CMAKE_SOURCE_DIR}/../../../../headers)

# create library for multiton pattern
add_library(multiton_pattern_lib INTERFACE)
target_include_directories(multiton_pattern_lib INTERFACE 
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/../../../../headers
)
target_compile_features(multiton_pattern_lib INTERFACE cxx_std_23)

# main demonstration executable
add_executable(multiton_pattern_demo
    src/main.cpp
)

target_link_libraries(multiton_pattern_demo 
    PRIVATE 
    multiton_pattern_lib
    Threads::Threads
)

# set output directory for demo executable
set_target_properties(multiton_pattern_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# test executable
add_executable(multiton_pattern_test
    src/multiton_test.cpp
)

target_link_libraries(multiton_pattern_test 
    PRIVATE 
    multiton_pattern_lib
    Threads::Threads
)

# set output directory for test executable
set_target_properties(multiton_pattern_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# add tests to ctest
add_test(
    NAME MultitonBasicTests
    COMMAND multiton_pattern_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(
    NAME MultitonDemonstration
    COMMAND multiton_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# set test properties
set_tests_properties(MultitonBasicTests PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed successfully"
)

set_tests_properties(MultitonDemonstration PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "All demonstrations completed successfully"
)

# custom target for running tests with verbose output
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS multiton_pattern_test multiton_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# custom target for running just the pattern tests
add_custom_target(run_pattern_tests
    COMMAND $<TARGET_FILE:multiton_pattern_test>
    DEPENDS multiton_pattern_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# custom target for running the demonstration
add_custom_target(run_demo
    COMMAND $<TARGET_FILE:multiton_pattern_demo>
    DEPENDS multiton_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# install targets
install(TARGETS multiton_pattern_demo multiton_pattern_test
    RUNTIME DESTINATION bin
)

install(DIRECTORY headers/
    DESTINATION include/multiton_pattern
    FILES_MATCHING PATTERN "*.hpp"
)

# package configuration
set(CPACK_PACKAGE_NAME "MultitonPatternDemo")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++23 Multiton Design Pattern Implementation")
set(CPACK_PACKAGE_VENDOR "dbjwhs")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md")

include(CPack)

# generate compilation database for ide support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# create custom log directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# summary information
message(STATUS "=== Multiton Pattern Demo Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Demo Executable: ${CMAKE_BINARY_DIR}/bin/multiton_pattern_demo")
message(STATUS "Test Executable: ${CMAKE_BINARY_DIR}/bin/multiton_pattern_test")
message(STATUS "===========================================")