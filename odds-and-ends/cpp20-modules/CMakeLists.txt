cmake_minimum_required(VERSION 3.30)  # CMake 3.28+ for proper C++20 modules support
project(cpp20_modules_example LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable GCC module support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

# Explicitly disable module scanning since GCC doesn't provide a scanner mechanism yet
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

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
