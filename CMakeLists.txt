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
    "src/c_mrapi/*.c"
    "src/c_mrapi/*.cpp"
    "src/MoonDetect/*.c"
    "src/MoonDetect/*.cpp"
    "src/MoonRegistrate/*.c"
    "src/MoonRegistrate/*.cpp"
)
include_directories("include")


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
    find_package(OpenCV PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)

endif()

include_directories( ${OpenCV_INCLUDE_DIRS} )


# handle build options

# MR_BUILD_SHARED_LIBS
option(MR_BUILD_SHARED_LIBS:BOOL "Build MoonRegistration into Shared Library" ON)
if(NOT DEFINED MR_BUILD_SHARED_LIBS) # check for no default value
    set(MR_BUILD_SHARED_LIBS ON)
endif()
if (MR_BUILD_SHARED_LIBS)
    set(MR_BUILD_TYPE SHARED)
else()
    set(MR_BUILD_TYPE STATIC)
endif()

option(MR_ENABLE_OPENCV_NONFREE:BOOL "Build MoonRegistration Library with OpenCV nonfree modules enabled" OFF)


# report
message(STATUS "================= MR Build Config ================")
message(STATUS "MR_BUILD_SHARED_LIBS: ${MR_BUILD_SHARED_LIBS}")
message(STATUS "MR_ENABLE_OPENCV_NONFREE: ${MR_ENABLE_OPENCV_NONFREE}")
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
message(STATUS "==================================================")


# generate mrconfig.h
set(MR_CONFIG_FILE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include" CACHE PATH "Where to create the platform-dependant mrconfig.h")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/MoonRegistration/mrconfig.h.in"
    "${MR_CONFIG_FILE_INCLUDE_DIR}/MoonRegistration/mrconfig.h"
    @ONLY
)
# add newly generated mrconfig.h to include directories
include_directories("${MR_CONFIG_FILE_INCLUDE_DIR}")


# build & link
add_library(${PROJECT_NAME} ${MR_BUILD_TYPE}
    ${SRC}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# add -fPIC compile flag for MoonRegistration for unix system
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
# expose mrconfig.h to external projects
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC "${MR_CONFIG_FILE_INCLUDE_DIR}"
)

# linking
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})


# install path setup from OpenCV
# https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT.html
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # native build
    if(NOT CMAKE_TOOLCHAIN_FILE)
        if(WIN32)
            set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
        else()
            set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Installation Directory" FORCE)
        endif()
    
    # any cross-compiling
    else()
        set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
    endif()
endif()

# install binary
install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
# install main include files
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
# install mrconfig.h
install(
    DIRECTORY "${MR_CONFIG_FILE_INCLUDE_DIR}/"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
