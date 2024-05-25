cmake_minimum_required(VERSION 3.11-3.25)

project(demo)

# we need C++ >= 14 because we are using std::filesystem for demo applications
set(CXX_VERSION 17)


# set cmake binary output directory to build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


# you need to manually include & link OpenCV
# handle manually provided OpenCV path via -DOPENCV_DIR cmake flag
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


# build MoonRegistration library as shared or static library (default is shared)
set(MR_BUILD_SHARED_LIBS ON)
# set(MR_BUILD_SHARED_LIBS OFF)


# ###############################################################################################
# adding MoonRegistration library as subdirectory
# ###############################################################################################
# since MoonRegistration library's primary CMakeLists.txt are in our parent directory,
# we need to use some tricks here for it to work
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
# You should place MoonRegistration under your primary CMakeLists.txt file
# and you can add this library using:
# add_subdirectory(/path/to/MoonRegistration)
# ###############################################################################################

# # ###############################################################################################
# # add MoonRegistration using find_package()
# # ###############################################################################################
# # this is useful when linking with a MoonRegistration install directory or release package
# # you need to supply path to MoonRegistration install directory
# set(MR_INSTALL_DIR "/path/to/MoonRegistration/install/dir")
# # Under Windows, you also need to manually copy .dll file to the exe path
# if(WIN32)
#     file(GLOB_RECURSE _MR_LIB_FILES "${MR_INSTALL_DIR}/*.dll")
#     file(
#         INSTALL ${_MR_LIB_FILES}
#         DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
#     )
# endif()
# # finally, find & configure MoonRegistration library
# find_package(MoonRegistration CONFIG REQUIRED PATHS ${MR_INSTALL_DIR})
# # ###############################################################################################


# setup include path for your project
include_directories(${MR_INCLUDE_DIR})


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
target_link_libraries(MoonRegistrate_live_registration
    MoonRegistration
    opencv_video
    opencv_videoio
    opencv_highgui
)



# demonstrate abstraction C api of the library
# since the library itself is a C++ library,
# we need to set the LINKER_LANGUAGE to CXX.
# And then we can set the LANGUAGE to C for these C program.
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

