# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.15)
project(dates)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module for downloading dependencies
include(FetchContent)

# Download Howard Hinnant's date library
FetchContent_Declare(
    date
    GIT_REPOSITORY https://github.com/HowardHinnant/date.git
    GIT_TAG v3.0.1
)

# Enable timezone database features
set(USE_SYSTEM_TZ_DB ON CACHE BOOL "Use the system timezone database")
set(BUILD_TZ_LIB ON CACHE BOOL "Build timezone library")

FetchContent_MakeAvailable(date)

add_executable(dates src/dates.cpp)

# Add include directories for project headers and date library
target_include_directories(dates PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../headers
)

# Link the date library
target_link_libraries(dates date::date date-tz)