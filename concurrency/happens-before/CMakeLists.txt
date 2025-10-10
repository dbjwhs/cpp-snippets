# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.30)
project(happens_before_demo)

# set c++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2 -Wno-gnu-zero-variadic-macro-arguments")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -O2 -Wno-gnu-zero-variadic-macro-arguments")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
endif()

# find required packages
find_package(Threads REQUIRED)

# include directories
include_directories(headers)
include_directories(../../headers)

# create executable
add_executable(happens_before_demo
    src/main.cpp
)

# link libraries
target_link_libraries(happens_before_demo
    Threads::Threads
)

# enable testing
enable_testing()

# add comprehensive test suite using ctest
add_test(NAME sequential_consistency_test
         COMMAND happens_before_demo)

add_test(NAME release_acquire_test
         COMMAND happens_before_demo)

add_test(NAME relaxed_ordering_test
         COMMAND happens_before_demo)

add_test(NAME thread_sync_test
         COMMAND happens_before_demo)

add_test(NAME fence_test
         COMMAND happens_before_demo)

add_test(NAME message_passing_test
         COMMAND happens_before_demo)

add_test(NAME transitivity_test
         COMMAND happens_before_demo)

add_test(NAME acq_rel_test
         COMMAND happens_before_demo)

add_test(NAME comprehensive_stress_test
         COMMAND happens_before_demo)

# set test properties for timeout
set_tests_properties(sequential_consistency_test PROPERTIES TIMEOUT 30)
set_tests_properties(release_acquire_test PROPERTIES TIMEOUT 30)
set_tests_properties(relaxed_ordering_test PROPERTIES TIMEOUT 30)
set_tests_properties(thread_sync_test PROPERTIES TIMEOUT 30)
set_tests_properties(fence_test PROPERTIES TIMEOUT 30)
set_tests_properties(message_passing_test PROPERTIES TIMEOUT 30)
set_tests_properties(transitivity_test PROPERTIES TIMEOUT 30)
set_tests_properties(acq_rel_test PROPERTIES TIMEOUT 30)
set_tests_properties(comprehensive_stress_test PROPERTIES TIMEOUT 60)
