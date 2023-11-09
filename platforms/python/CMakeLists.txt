cmake_minimum_required(VERSION 3.4-3.25)

project(MoonRegistration_pywrapper LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# setup python & boost::python libraries
find_package(PythonInterp)
# disable boost warning
# New Boost version may have incorrect or missing dependencies and imported targets
set(Boost_NO_WARN_NEW_VERSIONS ON)

if (PYTHONINTERP_FOUND)
    if (UNIX AND NOT APPLE) # linux
        if (PYTHON_VERSION_MAJOR EQUAL 3)
            find_package(Boost COMPONENTS python${PYTHON_VERSION_SUFFIX})
            find_package(PythonInterp 3)
            find_package(PythonLibs 3 REQUIRED)
        else()
            find_package(Boost COMPONENTS python)
            find_package(PythonInterp)
            find_package(PythonLibs REQUIRED)
        endif()
    else() # windows & macos
        if (PYTHON_VERSION_MAJOR EQUAL 3)
            find_package(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
            find_package(PythonInterp 3)
            find_package(PythonLibs 3 REQUIRED)
        else()
            find_package(Boost COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
            find_package(PythonInterp)
            find_package(PythonLibs REQUIRED)
        endif()
    endif()
else()
    message(FATAL_ERROR "Python not found")
endif()

message(STATUS "============= MR Python Build Config =============")
message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
message(STATUS "==================================================")

include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})


# we need to add MoonRegistration library 2 directories above us
# and also compile it as static library
set(MR_BUILD_SHARED_LIBS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)


# src files
file(GLOB SRC
    "src/*.cpp"
)
include_directories("include")

# finally, use this function provide by PythonInterp & PythonLibs to compile a .pyd file
PYTHON_ADD_MODULE(${CMAKE_PROJECT_NAME} ${SRC})

target_link_libraries(${CMAKE_PROJECT_NAME} ${PYTHON_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${CMAKE_PROJECT_NAME} MoonRegistration)

