cmake_minimum_required(VERSION 3.25)
project(SimpleClientServer)

# set c++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable strict warnings
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic -Wno-gnu-zero-variadic-macro-arguments)
endif()

# try to find boost system-wide first
find_package(Boost QUIET COMPONENTS system)

if(NOT Boost_FOUND)
    # if not found, use fetchcontent to download boost
    include(FetchContent)
    
    # fetch boost from sourceforge (more reliable than github releases)
    FetchContent_Declare(
        Boost
        URL https://archives.boost.io/release/1.84.0/source/boost_1_84_0.tar.bz2
        URL_HASH SHA256=cc4b893acf645c9d4b698e9a0f08ca8846aa5d6c68275c14c3e7949c24109454
    )
    
    # configure boost to build only what we need
    set(BOOST_INCLUDE_LIBRARIES system)
    set(BOOST_ENABLE_CMAKE ON)
    
    # make boost available
    FetchContent_MakeAvailable(Boost)
endif()

# create server executable
add_executable(server 
    src/server.cpp
)

# create client executable  
add_executable(client
    src/client.cpp
)

# create main example executable
add_executable(main
    src/main.cpp
)

# link boost to all executables
if(TARGET Boost::system)
    target_link_libraries(server PRIVATE Boost::system)
    target_link_libraries(client PRIVATE Boost::system)
    target_link_libraries(main PRIVATE Boost::system)
elseif(TARGET boost_system)
    target_link_libraries(server PRIVATE boost_system)
    target_link_libraries(client PRIVATE boost_system)
    target_link_libraries(main PRIVATE boost_system)
else()
    # fallback to system boost
    target_link_libraries(server PRIVATE ${Boost_LIBRARIES})
    target_link_libraries(client PRIVATE ${Boost_LIBRARIES})
    target_link_libraries(main PRIVATE ${Boost_LIBRARIES})
    target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})
endif()

# set output directory
set_target_properties(server client main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
