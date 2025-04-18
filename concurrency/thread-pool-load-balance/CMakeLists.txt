cmake_minimum_required(VERSION 3.20)
project(thread_pool_load_balance VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 as the default C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments)
endif()

# Create a variable for source files
set(SOURCES
        src/main.cpp
)

# Create a variable for header files
set(HEADERS
        include/thread_pool.hpp
)

# Set include directories
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../../headers  # For project_utils.hpp
)

# Add the executable
add_executable(thread_pool_load_balance ${SOURCES} ${HEADERS})

# Install targets
install(TARGETS thread_pool_load_balance
        RUNTIME DESTINATION bin
)

# Add a testing option
option(BUILD_TESTING "Build the testing" ON)

if(BUILD_TESTING)
    enable_testing()
    add_test(NAME thread_pool_test COMMAND thread_pool_load_balance)
endif()

# Print configuration summary
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "BUILD_TESTING: ${BUILD_TESTING}")