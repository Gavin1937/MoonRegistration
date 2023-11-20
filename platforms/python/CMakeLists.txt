cmake_minimum_required(VERSION 3.11-3.25)

project(MoonRegistration_pywrapper LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# setup dependencies

# python
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
    message( " *** NUMPY_INCLUDE_DIR : ${NUMPY_INCLUDE_DIR}" )
    if(NOT EXISTS ${NUMPY_INCLUDE_DIR}/numpy/ndarrayobject.h)
        message(SEND_ERROR "Can't find numpy/ndarrayobject.h in ${NUMPY_INCLUDE_DIR}")
    endif()
    include_directories(${NUMPY_INCLUDE_DIR})
endif()

# opencv
set(OpenCV_STATIC ON)
set(OpenCV_SHARED OFF)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )


message(STATUS "============= MR Python Build Config =============")
message(STATUS "PYTHON_LIBRARIES: ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE: ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS: ${PYTHON_INCLUDE_DIRS}")
message(STATUS "NUMPY_INCLUDE_DIR: ${NUMPY_INCLUDE_DIR}")
message(STATUS "==================================================")


# we need to add MoonRegistration library 2 directories above us
# and also compile it as static library
set(MR_BUILD_SHARED_LIBS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)


# src files
include_directories("include")
file(GLOB SRC
    "src/*.cpp"
)

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
