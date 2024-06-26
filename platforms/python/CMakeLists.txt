cmake_minimum_required(VERSION 3.11-3.25)

project(MoonRegistration_pywrapper LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# setup dependencies

# python
# handle python virtual environment
if(DEFINED ENV{VIRTUAL_ENV})
    # change the context of the search
    set (Python3_FIND_VIRTUALENV FIRST)
endif()
set(Python_USE_STATIC_LIBS ON)
find_package(Python COMPONENTS Interpreter Development.Module NumPy)

# pybind
# add_compile_options(-DPYBIND11_DETAILED_ERROR_MESSAGES) # enable debug message
include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.11.1
)
FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()
include_directories(${pybind11_INCLUDE_DIRS})

# numpy
set(NUMPY_INCLUDE_DIR "" CACHE FILEPATH "Path to numpy header if cmake can't find them.")
if (NOT ${NUMPY_INCLUDE_DIR} STREQUAL "")
    message(STATUS "NUMPY_INCLUDE_DIR: ${NUMPY_INCLUDE_DIR}" )
    if(NOT EXISTS ${NUMPY_INCLUDE_DIR}/numpy/ndarrayobject.h)
        message(SEND_ERROR "Can't find numpy/ndarrayobject.h in ${NUMPY_INCLUDE_DIR}")
    endif()
    include_directories(${NUMPY_INCLUDE_DIR})
endif()

# opencv
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


# we need to add MoonRegistration library 2 directories above us
# and also compile it as static library
set(MR_BUILD_SHARED_LIBS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
include_directories(${MR_INCLUDE_DIR})

# force update MR_VERSION_STR if its not set
if (NOT MR_VERSION_STR)
    get_directory_property(
        MR_VERSION_STR
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        DEFINITION MR_VERSION_STR
    )
endif()


message(STATUS "============= MR Python Build Config =============")
message(STATUS "MR_VERSION_STR: ${MR_VERSION_STR}")
message(STATUS "PYTHON_LIBRARIES: ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "NUMPY_INCLUDE_DIR: ${NUMPY_INCLUDE_DIR}")
message(STATUS "MR_INCLUDE_DIR: ${MR_INCLUDE_DIR}")
message(STATUS "==================================================")


# src files
file(GLOB SRC
    "src/*.cpp"
    "src/typecvt/*.cpp"
)
include_directories("include")

# we need to use following function instead of pybind11_add_module
# in order to get a .pyd file with specific output name
PYTHON_ADD_LIBRARY(${CMAKE_PROJECT_NAME} ${SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Python::NumPy)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE pybind11::headers)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE MoonRegistration)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION ON
    CXX_VISIBILITY_PRESET ON
    VISIBILITY_INLINES_HIDDEN ON
    OUTPUT_NAME ${CMAKE_PROJECT_NAME}
)


install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION "${PROJECT_SOURCE_DIR}/dist")
# install VERSION
install(
    FILES "${CMAKE_CURRENT_LIST_DIR}/../../VERSION"
    DESTINATION "${PROJECT_SOURCE_DIR}/dist"
)
