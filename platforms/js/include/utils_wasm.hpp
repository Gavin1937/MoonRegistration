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

typedef struct RectangleAndImageHandlerData
{
    int handler = -1;
    int rectangle = -1;
} RectangleAndImageHandlerData;

void* mrwasm_create_RectangleAndImageHandlerData(
    int img_width,
    int img_height,
    int img_data_length,
    int buffer_ptr,
    int image_ptr,
    int top_left_x,
    int top_left_y,
    int bottom_right_x,
    int bottom_right_y
);

}
