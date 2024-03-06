#pragma once


extern "C"
{

typedef struct ImageHandlerData
{
    int img_width = -1;
    int img_height = -1;
    int img_data_length = -1;
    long buffer_ptr = -1;
    long image_ptr = -1;
} ImageHandlerData;

EMSCRIPTEN_KEEPALIVE
void* mrwasm_create_ImageHandlerData(
    int img_width,
    int img_height,
    int img_data_length,
    long buffer_ptr,
    long image_ptr
);

}
