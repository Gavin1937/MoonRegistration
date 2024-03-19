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

# make sure opencv non-free is disabled
if (MR_ENABLE_OPENCV_NONFREE)
    message(SEND_ERROR "OpenCV JS doesn't support Non-Free modules and XFeatures2D module")
endif()
set(MR_ENABLE_OPENCV_NONFREE OFF)

# we need to add MoonRegistration library 2 directories above us
# and also compile it as static library
set(MR_BUILD_SHARED_LIBS OFF)
set(OpenCV_DIR "${OPENCV_SRC}/build_wasm")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/MoonRegistration)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)


# functions to export to wasm
set(MOONREGISTRATION_FUNCTION
    "_mrwasm_version"
)
set(UTILS_FUNCTION
    "_mrwasm_destroy_ImageHandlerData"
    "_mrwasm_create_image_buffer"
    "_mrwasm_create_image_ptr"
    "_mrwasm_destroy_image"
    "_mrwasm_destroy_RectangleAndImageHandlerData"
)
set(SHAPES_FUNCTION
    "_mrwasm_circle_to_square"
    "_mrwasm_circle_to_rectangle"
)
set(IMGPROCESS_FUNCTION
    "_mrwasm_cut_image_from_circle"
    "_mrwasm_stack_imgs"
)
set(MOONDETECT_FUNCTION
    "_mrwasm_detect_moon"
)
set(MOONREGISTRATE_FUNCTION
    "_mrwasm_draw_layer_image"
)

option(MRWASM_BUILD_MODULE:STRING "Configure which MoonRegistration WASM module to build" "All")
if (NOT DEFINED MRWASM_BUILD_MODULE)
    set(MRWASM_BUILD_MODULE "All")
endif()

if("${MRWASM_BUILD_MODULE}" STREQUAL "MoonDetect")
    string(JOIN "," EXPORTED_FUNCTIONS
        ${MOONREGISTRATION_FUNCTION}
        ${UTILS_FUNCTION}
        ${SHAPES_FUNCTION}
        ${IMGPROCESS_FUNCTION}
        ${MOONDETECT_FUNCTION}
    )
elseif("${MRWASM_BUILD_MODULE}" STREQUAL "MoonRegistrate")
    string(JOIN "," EXPORTED_FUNCTIONS
        ${MOONREGISTRATION_FUNCTION}
        ${UTILS_FUNCTION}
        ${SHAPES_FUNCTION}
        ${IMGPROCESS_FUNCTION}
        ${MOONDETECT_FUNCTION}
        ${MOONREGISTRATE_FUNCTION}
    )
elseif("${MRWASM_BUILD_MODULE}" STREQUAL "All")
    string(JOIN "," EXPORTED_FUNCTIONS
        ${MOONREGISTRATION_FUNCTION}
        ${UTILS_FUNCTION}
        ${SHAPES_FUNCTION}
        ${IMGPROCESS_FUNCTION}
        ${MOONDETECT_FUNCTION}
        ${MOONREGISTRATE_FUNCTION}
    )
else()
    message(SEND_ERROR "Cannot reconize build module type: ${MRWASM_BUILD_MODULE}")
endif()


# report
message(STATUS "============== MR WASM Build Config ==============")
message(STATUS "OPENCV_SRC: ${OPENCV_SRC}")
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
message(STATUS "INCLUDE_DIRECTORIES: ${OPENCV_SRC}/include;${OPENCV_SRC}/build_wasm;${opencv_include_modules}")
message(STATUS "MRWASM_BUILD_MODULE: ${MRWASM_BUILD_MODULE}")
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
    --bind \
    -s USE_ZLIB=1 \
    -s MEMORY64=0 \
    -s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} \
    -s MODULARIZE=1 \
    -s EXPORT_ES6=1 \
    -s USE_ES6_IMPORT_META=1 \
    -s NO_DISABLE_EXCEPTION_CATCHING=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MAXIMUM_MEMORY=2GB \
    "
)

target_link_libraries(${PROJECT_NAME} MoonRegistration)


# install as a js module package
# setup install directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation Directory" FORCE)
endif()

# install js wrapper layer
install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/moon-registration/"
    DESTINATION "moon-registration"
    FILES_MATCHING
    PATTERN "*.js"
    PATTERN "*.json"
    PATTERN "moon-registration/lib/*" EXCLUDE
)
# install wasm binary
install(
    FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MoonRegistration.js"
    DESTINATION "moon-registration/lib"
)
install(
    FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MoonRegistration.wasm"
    DESTINATION "moon-registration/lib"
)
