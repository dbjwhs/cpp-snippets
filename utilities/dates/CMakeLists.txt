# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.15)
project(dates)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_path(DATE_INCLUDE_DIR date/date.h)
find_library(DATE_LIBRARY date-tz)

add_executable(dates src/dates.cpp)
target_link_libraries(dates PRIVATE ${DATE_LIBRARY} CURL::libcurl)
target_include_directories(dates PRIVATE ${DATE_INCLUDE_DIR})