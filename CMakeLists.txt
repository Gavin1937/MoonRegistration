cmake_minimum_required(VERSION 3.11-3.25)

project(MoonRegistration LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# src files
file(GLOB SRC
    "src/*.c"
    "src/*.cpp"
    "src/MoonDetect/*.c"
    "src/MoonDetect/*.cpp"
    "src/MoonRegistrar/*.c"
    "src/MoonRegistrar/*.cpp"
)
file(GLOB HEADER
    "include/*.h"
    "include/*.hpp"
    "include/MoonDetect/*.h"
    "include/MoonDetect/*.hpp"
    "include/MoonRegistrar/*.h"
    "include/MoonRegistrar/*.hpp"
)


# setup external libraries

# OpenCV
set(OpenCV_STATIC ON)
set(OpenCV_SHARED OFF)

# no OpenCV path provided
if (NOT DEFINED OPENCV_DIR AND NOT DEFINED ${OPENCV_DIR})
    find_package(OpenCV REQUIRED)

# manually provide OpenCV path
# please manaully setting -DOPENCV_DIR cmake flag,
# point to a path contains files like:
# "OpenCVConfig.cmake" or "opencv-config.cmake"
else()
    message(STATUS "Setting OPENCV_DIR: ${OPENCV_DIR}")
    find_package(OpenCV PATHS ${OPENCV_DIR})

endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )


# handle build options

# MR_BUILD_SHARED_LIBS
option(MR_BUILD_SHARED_LIBS "Build MoonRegistration into Shared Library" ON)
if (MR_BUILD_SHARED_LIBS)
    set(MR_BUILD_TYPE SHARED)
else()
    set(MR_BUILD_TYPE STATIC)
endif()


# report
message(STATUS "================= MR Build Config ================")
message(STATUS "MR_BUILD_SHARED_LIBS: ${MR_BUILD_SHARED_LIBS}")
message(STATUS "MR_BUILD_WASM: ${MR_BUILD_WASM}")
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
message(STATUS "==================================================")


# build & link
add_library(${PROJECT_NAME} ${MR_BUILD_TYPE}
    ${SRC}
    ${HEADER}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# add -fPIC compile flag for MoonRegistration for unix system
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

# linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
