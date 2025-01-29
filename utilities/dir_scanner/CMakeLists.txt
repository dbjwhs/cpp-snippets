# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(dir_scanner)

set(CMAKE_CXX_STANDARD 20)

add_executable(dir_scanner src/dir_scanner.cpp)

target_include_directories(dir_scanner PUBLIC ${PROJECT_SOURCE_DIR}/include)
