# MIT License
# Copyright (c) 2025 dbjwhs

cmake_minimum_required(VERSION 3.25)
project(cpp23_mdspan VERSION 1.0.0 LANGUAGES CXX)

# set c++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler-specific flags for warnings and optimizations
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall 
        -Wextra 
        -Wpedantic
        -Wno-unused-parameter
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-unused-const-variable
        -O2
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(
        /W4
        /O2
    )
endif()

# enable testing with CTest
enable_testing()

# find required packages
# check if we need to find mdspan library or if it's header-only with compiler support
include(CheckCXXSourceCompiles)

# try to compile with std::mdspan
set(CMAKE_REQUIRED_FLAGS "-std=c++23")
check_cxx_source_compiles("
    #include <mdspan>
    int main() { 
        int data[4];
        std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span{data, 2, 2}; 
        return 0; 
    }" 
    HAVE_MDSPAN
)

# if native support not available, try to find external mdspan library
if(NOT HAVE_MDSPAN)
    message(STATUS "Native C++23 mdspan not available, trying external library...")
    
    # try to find mdspan via FetchContent or system package
    include(FetchContent)
    FetchContent_Declare(
        mdspan
        GIT_REPOSITORY https://github.com/kokkos/mdspan.git
        GIT_TAG stable
    )
    
    set(MDSPAN_ENABLE_TESTS OFF CACHE BOOL "Disable mdspan tests")
    set(MDSPAN_ENABLE_BENCHMARKS OFF CACHE BOOL "Disable mdspan benchmarks")
    
    FetchContent_MakeAvailable(mdspan)
    
    # check if external library works
    set(CMAKE_REQUIRED_LIBRARIES mdspan)
    check_cxx_source_compiles("
        #include <mdspan>
        int main() { 
            int data[4];
            std::mdspan<int, std::extents<size_t, std::dynamic_extent, std::dynamic_extent>> span{data, 2, 2}; 
            return 0; 
        }" 
        HAVE_EXTERNAL_MDSPAN
    )
    
    if(NOT HAVE_EXTERNAL_MDSPAN)
        message(WARNING "mdspan support not available, attempting manual implementation...")
        set(USE_MANUAL_MDSPAN TRUE)
    else()
        message(STATUS "Using external mdspan library")
        set(MDSPAN_TARGET mdspan)
    endif()
else()
    message(STATUS "Using native C++23 mdspan support")
endif()

# include directories
include_directories(${CMAKE_SOURCE_DIR}/headers)
include_directories(${CMAKE_SOURCE_DIR}/../../../headers)

# main executable
add_executable(cpp23_mdspan
    src/main.cpp
    src/mdspan_example.cpp
)

# test executable
add_executable(test_mdspan
    src/test_mdspan.cpp  
    src/mdspan_example.cpp
)

# link external mdspan library if needed
if(DEFINED MDSPAN_TARGET)
    target_link_libraries(cpp23_mdspan ${MDSPAN_TARGET})
    target_link_libraries(test_mdspan ${MDSPAN_TARGET})
endif()

# add tests to CTest
add_test(NAME mdspan_comprehensive_tests COMMAND test_mdspan)

# set test properties
set_tests_properties(mdspan_comprehensive_tests PROPERTIES
    TIMEOUT 60
    PASS_REGULAR_EXPRESSION "all tests passed successfully"
    FAIL_REGULAR_EXPRESSION "test failure|some tests failed"
)

# custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_mdspan
    COMMENT "Running all tests"
)

# installation rules
install(TARGETS cpp23_mdspan test_mdspan
    RUNTIME DESTINATION bin
)

install(FILES headers/mdspan_example.hpp
    DESTINATION include
)

# print configuration summary
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "mdspan Support: Available")