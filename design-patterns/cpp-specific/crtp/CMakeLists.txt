cmake_minimum_required(VERSION 3.28)
project(crtp_pattern_demo VERSION 1.0.0 LANGUAGES CXX)

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

# create library for crtp pattern
add_library(crtp_pattern_lib INTERFACE)
target_include_directories(crtp_pattern_lib INTERFACE
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/../../../../headers
)
target_compile_features(crtp_pattern_lib INTERFACE cxx_std_23)

# main demonstration executable
add_executable(crtp_pattern_demo
    src/main.cpp
)

target_link_libraries(crtp_pattern_demo
    PRIVATE
    crtp_pattern_lib
    Threads::Threads
)

# set output directory for demo executable
set_target_properties(crtp_pattern_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# test executables — one per CRTP concept
set(CRTP_TEST_SOURCES
    test_static_polymorphism
    test_mixins
    test_object_counting
    test_serialization
    test_performance
)

foreach(test_name ${CRTP_TEST_SOURCES})
    add_executable(${test_name} src/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE crtp_pattern_lib Threads::Threads)
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()

# register each test with ctest
add_test(
    NAME CRTPStaticPolymorphism
    COMMAND test_static_polymorphism
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPStaticPolymorphism PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All static polymorphism tests passed"
)

add_test(
    NAME CRTPMixins
    COMMAND test_mixins
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPMixins PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All mixin tests passed"
)

add_test(
    NAME CRTPObjectCounting
    COMMAND test_object_counting
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPObjectCounting PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All object counting tests passed"
)

add_test(
    NAME CRTPSerialization
    COMMAND test_serialization
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPSerialization PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All serialization tests passed"
)

add_test(
    NAME CRTPPerformance
    COMMAND test_performance
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPPerformance PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "All performance tests passed"
)

add_test(
    NAME CRTPDemonstration
    COMMAND crtp_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_tests_properties(CRTPDemonstration PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "All demonstrations completed successfully"
)

# custom target for running tests with verbose output
add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${CRTP_TEST_SOURCES} crtp_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# custom target for running just the pattern tests
add_custom_target(run_pattern_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose -E "Demonstration"
    DEPENDS ${CRTP_TEST_SOURCES}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# custom target for running the demonstration
add_custom_target(run_demo
    COMMAND $<TARGET_FILE:crtp_pattern_demo>
    DEPENDS crtp_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# install targets
install(TARGETS crtp_pattern_demo ${CRTP_TEST_SOURCES}
    RUNTIME DESTINATION bin
)

install(DIRECTORY headers/
    DESTINATION include/crtp_pattern
    FILES_MATCHING PATTERN "*.hpp"
)

# package configuration
set(CPACK_PACKAGE_NAME "CRTPPatternDemo")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++23 CRTP Design Pattern Implementation")
set(CPACK_PACKAGE_VENDOR "dbjwhs")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md")

include(CPack)

# generate compilation database for ide support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# create custom log directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/logs)

# summary information
message(STATUS "=== CRTP Pattern Demo Configuration ===")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Demo Executable: ${CMAKE_BINARY_DIR}/bin/crtp_pattern_demo")
message(STATUS "Test Executable: ${CMAKE_BINARY_DIR}/bin/crtp_pattern_test")
message(STATUS "===========================================")
