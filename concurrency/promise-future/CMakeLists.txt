# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.14)
project(promise_future_tester)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(promise_future_tester
    src/main.cpp
)

target_link_libraries(promise_future_tester Threads::Threads)

if(MSVC)
    target_compile_options(promise_future_tester PRIVATE /W4)
else()
    target_compile_options(promise_future_tester PRIVATE -Wall -Wextra -Wpedantic -Wno-gnu-zero-variadic-macro-arguments)
endif()