# CMakeLists.txt for stop_token_source_callback

cmake_minimum_required(VERSION 3.12)
project(stop_token_source_callback)

# Set C++ standard to C++20 or higher
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the experimental library flag for macOS
if(APPLE)
    add_compile_options(-fexperimental-library)
endif()

# Set source files
set(SOURCE_FILES
        src/stop_token_source_callback.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Find threading library and link it
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
        NOMINMAX  # For Windows to avoid min/max macro conflicts
)

# Set specific compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()

# Print configurations
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
if(APPLE)
    message(STATUS "macOS detected - enabling experimental library for std::stop_token")
endif()