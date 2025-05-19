# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.31)
project(cpp23_finite_state_machine)

set(CMAKE_CXX_STANDARD 23)

add_executable(cpp23_finite_state_machine src/fsm.cpp
        include/fsm.hpp)
