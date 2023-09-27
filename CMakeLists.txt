cmake_minimum_required(VERSION 3.4-3.25)

project(MoonRegistration LANGUAGES CXX)


# src files
file(GLOB SRC
    "src/*.c"
    "src/*.cpp"
)
file(GLOB HEADER
    "include/*.h"
    "include/*.hpp"
)


# setup external libraries

# OpenCV
set(OpenCV_STATIC ON)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )


# handle build options

# MR_BUILD_SHARED_LIBS
option(MR_BUILD_SHARED_LIBS "Build MoonRegistration as Shared Library" ON)
if (MR_BUILD_SHARED_LIBS)
    set(MR_BUILD_TYPE SHARED)
else()
    set(MR_BUILD_TYPE STATIC)
endif()


# report
message("-- Build Options:")
message("-- MR_BUILD_SHARED_LIBS: ${MR_BUILD_SHARED_LIBS}")


# build library
add_library(${PROJECT_NAME} ${MR_BUILD_TYPE}
    ${SRC}
    ${HEADER}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

