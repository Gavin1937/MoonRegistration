cmake_minimum_required(VERSION 3.4-3.25)

project(demo)

# we need C++ >= 14 since we are using filesystem
set(CXX_VERSION 17)


# set cmake binary output directory to build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


# build MoonRegistration library as shared or static library (default is shared)
set(MR_BUILD_SHARED_LIBS ON)
# set(MR_BUILD_SHARED_LIBS OFF)


# adding MoonRegistration library as subdirectory
# since MoonRegistration library's primary CMakeLists.txt are in our parent directory,
# we need to use some tricks here for that to work
# You should place MoonRegistration under your primary CMakeLists.txt file
# and you can add this library using:
# add_subdirectory(path/to/MoonRegistration)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)

# setup include path for your project
# so you can use `#include "MoonRegistration/mrapi.h"`
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)


# compile & linking

add_executable(MoonDetector_basic ${CMAKE_CURRENT_SOURCE_DIR}/MoonDetector_basic.cpp)
set_property(TARGET MoonDetector_basic PROPERTY CXX_STANDARD ${CXX_VERSION})
target_link_libraries(MoonDetector_basic MoonRegistration)

add_executable(MoonDetector_advance ${CMAKE_CURRENT_SOURCE_DIR}/MoonDetector_advance.cpp)
set_property(TARGET MoonDetector_advance PROPERTY CXX_STANDARD ${CXX_VERSION})
target_link_libraries(MoonDetector_advance MoonRegistration)

