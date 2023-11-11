cmake_minimum_required(VERSION 3.4-3.25)

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
)
file(GLOB HEADER
    "include/*.h"
    "include/*.hpp"
    "include/MoonDetect/*.h"
    "include/MoonDetect/*.hpp"
)


# setup external libraries

# OpenCV
option(MR_BUILD_WASM "Build MoonRegistration into WebAssembly" OFF)
set(OpenCV_STATIC ON)
if (MR_BUILD_WASM)
    # Needed for opencv2/opencv.hpp
    include_directories("${OPENCV_DIR}/include")
    
    # Needed by opencv.hpp for opencv2/opencv_modules.hpp
    include_directories("${OPENCV_DIR}/build_wasm")
    
    # Needed by opencv_modules.hpp for every module
    file(GLOB opencv_include_modules "${OPENCV_DIR}/modules/*/include")
    include_directories(${opencv_include_modules})
else()
    find_package(OpenCV REQUIRED)
    include_directories( ${OpenCV_INCLUDE_DIRS} )
endif()


# handle build options

# MR_BUILD_SHARED_LIBS
option(MR_BUILD_SHARED_LIBS "Build MoonRegistration into Shared Library" ON)
if (MR_BUILD_SHARED_LIBS)
    set(MR_BUILD_TYPE SHARED)
else()
    set(MR_BUILD_TYPE STATIC)
endif()

# MR_BUILD_WASM
if (MR_BUILD_WASM)
    add_definitions(-DMR_BUILD_WASM)
endif()


# report
message(STATUS "================= MR Build Config ================")
message(STATUS "MR_BUILD_SHARED_LIBS: ${MR_BUILD_SHARED_LIBS}")
message(STATUS "MR_BUILD_WASM: ${MR_BUILD_WASM}")
message(STATUS "OPENCV_DIR: ${OPENCV_DIR}")
message(STATUS "==================================================")


# build & link

# normal version library
if(NOT MR_BUILD_WASM)
    
    add_library(${PROJECT_NAME} ${MR_BUILD_TYPE}
        ${SRC}
        ${HEADER}
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    # add -fPIC compile flag for MoonRegistration for linux system
    set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    
    # linking
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
    
# Emscripten WebAssembly
else()
    
    add_executable(${PROJECT_NAME}
        ${SRC}
        ${HEADER}
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    
    # linking
    # linking with manully built opencv_wasm
    
    # Link to opencv.js precompiled libraries
    file(GLOB opencv_js_libs "${OPENCV_DIR}/build_wasm/lib/*.a")
    target_link_libraries(${PROJECT_NAME} ${opencv_js_libs})
    
    # There is an issue regarding the order in which libraries
    # are passed to the compiler - pass libopencv_core.a last
    # https://answers.opencv.org/question/186124/undefined-reference-to-cvsoftdoubleoperator/
    file(GLOB opencv_js_lib_core "${OPENCV_DIR}/build_wasm/lib/libopencv_core.a")
    target_link_libraries(${PROJECT_NAME} ${opencv_js_lib_core})
    
    # Specify linker arguments for Emscripten wasm-ld
    # Note that, linker arguments MUST BE ONE STRING
    # otherwise cmake will take multiple strings as multiple args
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
        " \
        -s EXPORTED_RUNTIME_METHODS=cwrap \
        -s EXPORTED_FUNCTIONS=_mrwasm_create_image_buffer,_mrwasm_destroy_image_buffer,_mrwasm_detect_moon,_mrwasm_circle_to_square,_mrwasm_circle_to_rectangle \
        -s MODULARIZE=1 \
        -s EXPORT_ES6=1 \
        -s USE_ES6_IMPORT_META=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        "
    )
    
endif()

