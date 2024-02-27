cmake_minimum_required(VERSION 3.11-3.25)

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

# MoonDetect module
add_executable(MoonDetect_basic ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetect_basic.cpp)
set_target_properties(MoonDetect_basic PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetect_basic MoonRegistration)

add_executable(MoonDetect_advance ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetect_advance.cpp)
set_target_properties(MoonDetect_advance PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetect_advance MoonRegistration)

# MoonRegistrate module
add_executable(MoonRegistrate_basic ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrate_basic.cpp)
set_target_properties(MoonRegistrate_basic PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrate_basic MoonRegistration)

add_executable(MoonRegistrate_advance ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrate_advance.cpp)
set_target_properties(MoonRegistrate_advance PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrate_advance MoonRegistration)

add_executable(MoonRegistrate_live_registration ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrate_live_registration.cpp)
set_target_properties(MoonRegistrate_live_registration PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrate_live_registration MoonRegistration)



# demonstrate abstraction C api of the library
# since the library itself is a C++ library,
# we need to set the LINKER_LANGUAGE to CXX.
# And then we can set the LANGUAGE to C for this C program.
add_executable(MoonDetect_c_api ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetect_c_api.c)
set_target_properties(MoonDetect_c_api PROPERTIES
    LANGUAGE         C
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetect_c_api MoonRegistration)

add_executable(MoonRegistrate_c_api ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrate_c_api.c)
set_target_properties(MoonRegistrate_c_api PROPERTIES
    LANGUAGE         C
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrate_c_api MoonRegistration)

