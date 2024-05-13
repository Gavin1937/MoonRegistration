cmake_minimum_required(VERSION 3.11-3.25)

project(MoonRegistration LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# handle CMAKE_TOOLCHAIN_FILE from environment variable if exists
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
    set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()


# add "d" at the end of debug library
set(CMAKE_DEBUG_POSTFIX "d")


# read library version
set(MR_VERSION_STR CACHE STRING "MR_VERSION_STR" FORCE)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" MR_VERSION_STR)
string(REGEX MATCH "v([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ ${MR_VERSION_STR})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
message(STATUS "CMAKE_PROJECT_VERSION: ${CMAKE_PROJECT_VERSION}")


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

# manually provide OpenCV path
# please manaully setting -DOPENCV_DIR cmake flag,
# point to a path contains files like:
# "OpenCVConfig.cmake" or "opencv-config.cmake"
if(DEFINED OPENCV_DIR OR DEFINED ${OPENCV_DIR})
    message(STATUS "Setting OPENCV_DIR: ${OPENCV_DIR}")
    find_package(OpenCV PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)
elseif(DEFINED $ENV{OPENCV_DIR})
    message(STATUS "Setting OPENCV_DIR: $ENV{OPENCV_DIR}")
    find_package(OpenCV PATHS $ENV{OPENCV_DIR} NO_DEFAULT_PATH)
elseif(DEFINED $ENV{OpenCV_DIR})
    message(STATUS "Setting OPENCV_DIR: $ENV{OpenCV_DIR}")
    find_package(OpenCV PATHS $ENV{OpenCV_DIR} NO_DEFAULT_PATH)
# no OpenCV path provided
else()
    find_package(OpenCV REQUIRED)
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
message(STATUS "MR_VERSION_STR: ${MR_VERSION_STR}")
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
set(MR_MAIN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE PATH "Main include directory")
set(tmp_include_dirs "${MR_MAIN_INCLUDE_DIR}" "${MR_CONFIG_FILE_INCLUDE_DIR}")
set(MR_INCLUDE_DIR ${tmp_include_dirs} CACHE PATH "MR_INCLUDE_DIR" FORCE)
set(MR_LIBRARIES "${PROJECT_NAME}" CACHE STRING "MR_LIBRARIES" FORCE)


# setup .cmake config files
# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)
# cmake distribution config file dir
set(CMAKE_DIST_CONFIG_DIR "${CMAKE_BINARY_DIR}/cmake_dist_config")
# Configure "${PROJECT_NAME}ConfigVersion.cmake"
write_basic_package_version_file(
    "${CMAKE_DIST_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
    VERSION ${MR_VERSION_STR}
)
# Configure "${PROJECT_NAME}Config.cmake"
configure_package_config_file(
    "cmake/Config.cmake.in"
    "${CMAKE_DIST_CONFIG_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_DIST_CONFIG_DIR}/${PROJECT_NAME}"
)


# build & link
add_library(${PROJECT_NAME} ${MR_BUILD_TYPE}
    ${SRC}
)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
# add -fPIC compile flag for MoonRegistration for unix system
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
# expose target include directories for build-tree & install-tree
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
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

# install binary & export targets
if(WIN32)
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT "${PROJECT_NAME}Targets"
        CONFIGURATIONS Debug Release
        LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
        RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
    )
else()
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT "${PROJECT_NAME}Targets"
        CONFIGURATIONS Debug Release
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()
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
# install cmake configurations
install(
    DIRECTORY "${CMAKE_DIST_CONFIG_DIR}/"
    DESTINATION "."
    PATTERN "${CMAKE_DIST_CONFIG_DIR}/*.cmake"
)
install(
    EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    DESTINATION "."
)
# install VERSION
install(
    FILES
        "${CMAKE_CURRENT_LIST_DIR}/VERSION"
        "${CMAKE_CURRENT_LIST_DIR}/LICENSE"
    DESTINATION "."
)
