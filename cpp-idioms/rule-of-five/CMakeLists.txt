cmake_minimum_required(VERSION 3.20)

project(RuleOfFive 
    VERSION 1.0.0
    DESCRIPTION "Comprehensive demonstration of the Rule of Five C++ idiom"
    LANGUAGES CXX)

# require C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set compiler-specific flags for warnings and optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
        -Wno-poison-system-directories
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-unused-variable
        -Wno-self-assign-overloaded
        -Wno-self-move
    )
endif()

# set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# optimization flags for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# debug flags for debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O0 -g3 -ggdb)
    endif()
    add_compile_definitions(RULE_OF_FIVE_DEBUG)
endif()

# find required packages
find_package(Threads REQUIRED)

# include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# create main executable
add_executable(rule_of_five_demo
    src/main.cpp
)

# link libraries to main executable
target_link_libraries(rule_of_five_demo
    PRIVATE 
    Threads::Threads
)

# set executable properties
set_target_properties(rule_of_five_demo PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "rule_of_five_demo"
)

# enable testing with CTest
enable_testing()

# create test executable
add_executable(rule_of_five_tests
    src/test_rule_of_five.cpp
)

# link libraries to test executable
target_link_libraries(rule_of_five_tests
    PRIVATE 
    Threads::Threads
)

# set test executable properties
set_target_properties(rule_of_five_tests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "rule_of_five_tests"
)

# add CTest tests
add_test(
    NAME RuleOfFiveUnitTests
    COMMAND rule_of_five_tests
)

# set test properties
set_tests_properties(RuleOfFiveUnitTests PROPERTIES
    TIMEOUT 30
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# add custom target for running all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS rule_of_five_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all Rule of Five tests"
)

# add custom target for running the demo
add_custom_target(run_demo
    COMMAND rule_of_five_demo
    DEPENDS rule_of_five_demo
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running Rule of Five demonstration"
)

# add custom target to clean log files
add_custom_target(clean_logs
    COMMAND ${CMAKE_COMMAND} -E remove -f
        rule_of_five_demo.log
        rule_of_five_tests.log
        custom.log
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Cleaning log files"
)

# print configuration summary
message(STATUS "")
message(STATUS "Rule of Five Configuration Summary:")
message(STATUS "==================================")
message(STATUS "CMAKE_BUILD_TYPE:           ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_STANDARD:         ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_CXX_COMPILER_ID:      ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_SYSTEM:               ${CMAKE_SYSTEM}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:     ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  rule_of_five_demo   - Build and run the main demonstration")
message(STATUS "  rule_of_five_tests  - Build the test suite")
message(STATUS "  run_demo           - Run the demonstration")
message(STATUS "  run_tests          - Run all tests with CTest")
message(STATUS "  clean_logs         - Clean generated log files")
message(STATUS "")

# install targets (optional)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Installation directory" FORCE)
endif()

install(TARGETS rule_of_five_demo rule_of_five_tests
    RUNTIME DESTINATION bin
)

install(FILES headers/resource_manager.hpp
    DESTINATION include/rule_of_five
)

install(FILES README.md
    DESTINATION .
)

# create simple uninstall target
if(NOT TARGET uninstall)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -E echo "Uninstall complete. Please manually remove installed files from ${CMAKE_INSTALL_PREFIX}"
        COMMENT "Simple uninstall target"
    )
endif()