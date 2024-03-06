#include <emscripten.h>
#include <emscripten/bind.h>

#include "utils_wasm.hpp"


EMSCRIPTEN_BINDINGS(MoonRegistration) {
    emscripten::class_<ImageHandlerData>("ImageHandlerData")
        .constructor<>()
        .property("img_width", &ImageHandlerData::img_width)
        .property("img_height", &ImageHandlerData::img_height)
        .property("img_data_length", &ImageHandlerData::img_data_length)
        .property("buffer_ptr", &ImageHandlerData::buffer_ptr)
        .property("image_ptr", &ImageHandlerData::image_ptr)
    ;
}
