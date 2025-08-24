cmake_minimum_required(VERSION 3.25)
project(newtype_wrapper VERSION 1.0.0 LANGUAGES CXX)

# require c++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compiler-specific flags for strict compilation and warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-inaccessible-base")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

# find the project_utils header path
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(HEADERS_PATH "${PROJECT_ROOT}/headers")

# verify the headers directory exists
if(NOT EXISTS "${HEADERS_PATH}")
    message(FATAL_ERROR "Headers directory not found at: ${HEADERS_PATH}")
endif()

# verify project_utils.hpp exists
if(NOT EXISTS "${HEADERS_PATH}/project_utils.hpp")
    message(FATAL_ERROR "project_utils.hpp not found at: ${HEADERS_PATH}/project_utils.hpp")
endif()

# include directories
include_directories(${HEADERS_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)

# main executable
add_executable(newtype_wrapper_main
    src/main.cpp
)

# enable testing
enable_testing()
include(CTest)

# create comprehensive test executable
add_executable(newtype_wrapper_tests
    src/main.cpp
)

# set target properties
set_target_properties(newtype_wrapper_main PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set_target_properties(newtype_wrapper_tests PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add test cases using ctest
add_test(
    NAME newtype_wrapper_comprehensive_tests
    COMMAND newtype_wrapper_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# set test properties for timeout and labels
set_tests_properties(newtype_wrapper_comprehensive_tests PROPERTIES
    TIMEOUT 60
    LABELS "unit;comprehensive"
)

# add custom test for memory leaks if valgrind is available
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    add_test(
        NAME newtype_wrapper_memory_check
        COMMAND ${VALGRIND_EXECUTABLE} 
            --tool=memcheck 
            --leak-check=full 
            --show-leak-kinds=all 
            --track-origins=yes 
            --error-exitcode=1
            $<TARGET_FILE:newtype_wrapper_tests>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    set_tests_properties(newtype_wrapper_memory_check PROPERTIES
        TIMEOUT 120
        LABELS "memory;valgrind"
    )
endif()

# add custom test for sanitizers if supported
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    # address sanitizer build
    add_executable(newtype_wrapper_asan
        src/main.cpp
    )
    
    set_target_properties(newtype_wrapper_asan PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_FLAGS "-fsanitize=address -fno-omit-frame-pointer"
        LINK_FLAGS "-fsanitize=address"
    )
    
    add_test(
        NAME newtype_wrapper_address_sanitizer
        COMMAND newtype_wrapper_asan
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    set_tests_properties(newtype_wrapper_address_sanitizer PROPERTIES
        TIMEOUT 90
        LABELS "sanitizer;address"
    )
    
    # undefined behavior sanitizer build
    add_executable(newtype_wrapper_ubsan
        src/main.cpp
    )
    
    set_target_properties(newtype_wrapper_ubsan PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_FLAGS "-fsanitize=undefined -fno-omit-frame-pointer"
        LINK_FLAGS "-fsanitize=undefined"
    )
    
    add_test(
        NAME newtype_wrapper_undefined_behavior_sanitizer
        COMMAND newtype_wrapper_ubsan
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    
    set_tests_properties(newtype_wrapper_undefined_behavior_sanitizer PROPERTIES
        TIMEOUT 90
        LABELS "sanitizer;undefined"
    )
endif()

# custom target for running all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --parallel 4
    DEPENDS newtype_wrapper_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "running all newtype wrapper tests"
)

# clion-friendly target for development
add_custom_target(dev_build ALL
    DEPENDS newtype_wrapper_main newtype_wrapper_tests
    COMMENT "building main development targets"
)

# custom target for running only unit tests
add_custom_target(run_unit_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --label-regex "unit"
    DEPENDS newtype_wrapper_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "running unit tests only"
)

# custom target for running sanitizer tests
add_custom_target(run_sanitizer_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose --label-regex "sanitizer"
    DEPENDS newtype_wrapper_asan newtype_wrapper_ubsan
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "running sanitizer tests"
)

# installation rules
install(TARGETS newtype_wrapper_main
    RUNTIME DESTINATION bin
)

install(FILES headers/strong_type.hpp
    DESTINATION include
)

# cpack configuration for packaging
set(CPACK_PACKAGE_NAME "newtype_wrapper")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "comprehensive c++23 strong type wrapper implementation")
set(CPACK_PACKAGE_VENDOR "dbjwhs")
set(CPACK_GENERATOR "TGZ")

include(CPack)

# print configuration summary
message(STATUS "=== newtype wrapper configuration ===")
message(STATUS "cmake version: ${CMAKE_VERSION}")
message(STATUS "build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "c++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "c++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "headers path: ${HEADERS_PATH}")
message(STATUS "valgrind found: ${VALGRIND_EXECUTABLE}")
message(STATUS "===================================")