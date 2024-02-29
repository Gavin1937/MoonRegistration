cmake_minimum_required(VERSION 3.11-3.25)

project(MoonRegistration_wasm LANGUAGES CXX)

# set cmake binary output directory to build/bin or build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# setup dependencies

# opencv
# Needed for opencv2/opencv.hpp
include_directories("${OPENCV_SRC}/include")

# Needed by opencv.hpp for opencv2/opencv_modules.hpp
include_directories("${OPENCV_SRC}/build_wasm")

# Needed by opencv_modules.hpp for every module
file(GLOB opencv_include_modules "${OPENCV_SRC}/modules/*/include")
include_directories(${opencv_include_modules})


# we need to add MoonRegistration library 2 directories above us
# and also compile it as static library
set(MR_BUILD_SHARED_LIBS OFF)
set(OPENCV_DIR "${OPENCV_SRC}/build_wasm")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)


# functions export to wasm
set(EXPORTED_FUNCTIONS "_mrwasm_create_image_buffer,_mrwasm_destroy_image_buffer,_mrwasm_detect_moon,_mrwasm_circle_to_square,_mrwasm_circle_to_rectangle")


# report
message(STATUS "============== MR WASM Build Config ==============")
message(STATUS "OPENCV_SRC: ${OPENCV_SRC}")
message(STATUS "OPENCV_DIR: ${OPENCV_DIR}")
message(STATUS "INCLUDE_DIRECTORIES: ${OPENCV_SRC}/include;${OPENCV_SRC}/build_wasm;${opencv_include_modules}")
message(STATUS "EXPORTED_FUNCTIONS: ${EXPORTED_FUNCTIONS}")
message(STATUS "==================================================")


# src files
include_directories("include")
file(GLOB SRC
    "src/*.cpp"
)


add_executable(${PROJECT_NAME}
    ${SRC}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINKER_LANGUAGE CXX
    OUTPUT_NAME "MoonRegistration"
)

# linking
# linking with manully built opencv_wasm

# Link to opencv.js precompiled libraries
file(GLOB opencv_js_libs "${OPENCV_SRC}/build_wasm/lib/*.a")
target_link_libraries(${PROJECT_NAME} ${opencv_js_libs})

# There is an issue regarding the order in which libraries
# are passed to the compiler - pass libopencv_core.a last
# https://answers.opencv.org/question/186124/undefined-reference-to-cvsoftdoubleoperator/
file(GLOB opencv_js_lib_core "${OPENCV_SRC}/build_wasm/lib/libopencv_core.a")
target_link_libraries(${PROJECT_NAME} ${opencv_js_lib_core})

# Specify linker arguments for Emscripten wasm-ld
# Note that, linker arguments MUST BE ONE STRING
# otherwise cmake will take multiple strings as multiple args
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
    " \
    -s EXPORTED_RUNTIME_METHODS=cwrap \
    -s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} \
    -s MODULARIZE=1 \
    -s EXPORT_ES6=1 \
    -s USE_ES6_IMPORT_META=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    "
)

target_link_libraries(${PROJECT_NAME} MoonRegistration)
