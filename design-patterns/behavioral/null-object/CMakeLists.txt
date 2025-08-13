# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.20)

# Project configuration
project(NullObjectPattern
    VERSION 1.0.0
    DESCRIPTION "Implementation of the Null Object behavioral design pattern"
    LANGUAGES CXX
)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wunused -Wuninitialized
        -Wshadow -Wcast-align
        -Wcast-qual -Wold-style-cast
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-sign-conversion
    )
    
    # Debug configuration
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG_MODE")
    
    # Release configuration
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(
        /W4 /WX
        /permissive-
        /Zc:__cplusplus
    )
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../../headers)

# Source files
set(LIBRARY_SOURCES
    src/service_interface.cpp
    src/real_service.cpp
    src/null_service.cpp
)

set(LIBRARY_HEADERS
    headers/service_interface.hpp
    headers/real_service.hpp
    headers/null_service.hpp
)

# Create static library
add_library(null_object_pattern_lib STATIC 
    ${LIBRARY_SOURCES} 
    ${LIBRARY_HEADERS}
)

# Library properties
set_target_properties(null_object_pattern_lib PROPERTIES
    OUTPUT_NAME "null_object_pattern"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Main executable
add_executable(null_object_pattern_demo src/main.cpp)

# Link library to executable
target_link_libraries(null_object_pattern_demo 
    PRIVATE null_object_pattern_lib
)

# Executable properties
set_target_properties(null_object_pattern_demo PROPERTIES
    OUTPUT_NAME "null_object_pattern_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Enable testing
enable_testing()
include(CTest)

# Test configuration
if(BUILD_TESTING)
    # Main functionality test
    add_test(
        NAME null_object_pattern_functionality_test
        COMMAND null_object_pattern_demo
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Set test properties for comprehensive validation
    set_tests_properties(null_object_pattern_functionality_test PROPERTIES
        TIMEOUT 60
        PASS_REGULAR_EXPRESSION "null object pattern test suite completed successfully"
        FAIL_REGULAR_EXPRESSION "test suite failed|assertion failed|error:"
    )
    
    # Memory leak detection test (if available)
    find_program(VALGRIND_PROGRAM valgrind)
    if(VALGRIND_PROGRAM AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_test(
            NAME null_object_pattern_memory_test
            COMMAND ${VALGRIND_PROGRAM}
                --tool=memcheck
                --leak-check=full
                --show-leak-kinds=all
                --track-origins=yes
                --error-exitcode=1
                --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp
                $<TARGET_FILE:null_object_pattern_demo>
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        set_tests_properties(null_object_pattern_memory_test PROPERTIES
            TIMEOUT 180
            DEPENDS null_object_pattern_functionality_test
        )
    endif()
    
    # Performance benchmark test
    add_test(
        NAME null_object_pattern_performance_test
        COMMAND null_object_pattern_demo
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    set_tests_properties(null_object_pattern_performance_test PROPERTIES
        TIMEOUT 30
        PASS_REGULAR_EXPRESSION "performance benchmark results"
        LABELS "performance;benchmark"
    )
    
    # Static analysis with clang-tidy (if available)
    find_program(CLANG_TIDY_PROGRAM clang-tidy)
    if(CLANG_TIDY_PROGRAM)
        set(CLANG_TIDY_CHECKS
            "bugprone-*"
            "cert-*"
            "clang-analyzer-*"
            "cppcoreguidelines-*"
            "modernize-*"
            "performance-*"
            "portability-*"
            "readability-*"
        )
        
        string(REPLACE ";" "," CLANG_TIDY_CHECKS_STR "${CLANG_TIDY_CHECKS}")
        
        set_target_properties(null_object_pattern_lib PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_PROGRAM};-checks=${CLANG_TIDY_CHECKS_STR}"
        )
        
        set_target_properties(null_object_pattern_demo PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_PROGRAM};-checks=${CLANG_TIDY_CHECKS_STR}"
        )
    endif()
endif()

# Custom targets for development workflow
add_custom_target(run_demo
    COMMAND $<TARGET_FILE:null_object_pattern_demo>
    DEPENDS null_object_pattern_demo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running null object pattern demonstration"
)

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS null_object_pattern_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests with verbose output"
)

add_custom_target(benchmark
    COMMAND $<TARGET_FILE:null_object_pattern_demo>
    DEPENDS null_object_pattern_demo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running performance benchmarks"
)

# Installation configuration
install(TARGETS null_object_pattern_lib null_object_pattern_demo
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${LIBRARY_HEADERS}
    DESTINATION include/null_object_pattern
)

# Package configuration
set(CPACK_PACKAGE_NAME "null_object_pattern")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "dbjwhs")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../../../LICENSE")

include(CPack)

# Documentation generation (if Doxygen is available)
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Null Object Pattern")
    set(DOXYGEN_PROJECT_BRIEF "Behavioral design pattern implementation")
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_XML YES)
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    
    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        COMMENT "Generating API documentation"
    )
endif()

# Print configuration summary
message(STATUS "=== Null Object Pattern Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Testing enabled: ${BUILD_TESTING}")
if(VALGRIND_PROGRAM)
    message(STATUS "Valgrind found: ${VALGRIND_PROGRAM}")
endif()
if(CLANG_TIDY_PROGRAM)
    message(STATUS "Clang-tidy found: ${CLANG_TIDY_PROGRAM}")
endif()
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found: documentation target available")
endif()
message(STATUS "==========================================")