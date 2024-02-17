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
add_executable(MoonDetector_basic ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetector_basic.cpp)
set_target_properties(MoonDetector_basic PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetector_basic MoonRegistration)

add_executable(MoonDetector_advance ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetector_advance.cpp)
set_target_properties(MoonDetector_advance PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetector_advance MoonRegistration)

# MoonRegistrar module
add_executable(MoonRegistrar_basic ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrar_basic.cpp)
set_target_properties(MoonRegistrar_basic PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrar_basic MoonRegistration)

add_executable(MoonRegistrar_advance ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrar_advance.cpp)
set_target_properties(MoonRegistrar_advance PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrar_advance MoonRegistration)

add_executable(MoonRegistrar_live_registration ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonRegistrar_live_registration.cpp)
set_target_properties(MoonRegistrar_live_registration PROPERTIES
    LANGUAGE         CXX
    CXX_STANDARD     ${CXX_VERSION}
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonRegistrar_live_registration MoonRegistration)



# demonstrate abstraction C api of the library
# since the library itself is a C++ library,
# we need to set the LINKER_LANGUAGE to CXX.
# And then we can set the LANGUAGE to C for this C program.
add_executable(MoonDetector_c_api ${CMAKE_CURRENT_SOURCE_DIR}/cpp_c/MoonDetector_c_api.c)
set_target_properties(MoonDetector_c_api PROPERTIES
    LANGUAGE         C
    LINKER_LANGUAGE  CXX
)
target_link_libraries(MoonDetector_c_api MoonRegistration)

