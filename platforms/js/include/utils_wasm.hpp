#pragma once


extern "C"
{

typedef struct ImageHandlerData
{
    int img_width = -1;
    int img_height = -1;
    int img_data_length = -1;
    int image_ptr = -1;
} ImageHandlerData;

void* mrwasm_create_ImageHandlerData(cv::Mat* image_ptr);

typedef struct RectangleAndImageHandlerData
{
    int handler = -1;
    int rectangle = -1;
} RectangleAndImageHandlerData;

void* mrwasm_create_RectangleAndImageHandlerData(
    cv::Mat* image_ptr,
    int top_left_x,
    int top_left_y,
    int bottom_right_x,
    int bottom_right_y
);

}
