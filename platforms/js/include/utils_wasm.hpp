#pragma once


extern "C"
{

typedef struct ImageHandlerData
{
    int img_width = -1;
    int img_height = -1;
    int img_data_length = -1;
    int buffer_ptr = -1;
    int image_ptr = -1;
} ImageHandlerData;

void* mrwasm_create_ImageHandlerData(
    int img_width,
    int img_height,
    int img_data_length,
    int buffer_ptr,
    int image_ptr
);

}
