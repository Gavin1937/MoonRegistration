#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include <exception>

#include "MoonRegistration/shapes.hpp"

#include "utils_wasm.hpp"


extern "C"
{

void* mrwasm_create_ImageHandlerData(cv::Mat* image_ptr)
{
    try
    {
        ImageHandlerData* ret = new ImageHandlerData();
        ret->img_width = static_cast<int>(image_ptr->size[1]);
        ret->img_height = static_cast<int>(image_ptr->size[0]);
        ret->img_data_length = static_cast<int>(image_ptr->total() * image_ptr->elemSize());
        ret->buffer_ptr = reinterpret_cast<int>(image_ptr->data);
        ret->image_ptr = reinterpret_cast<int>(image_ptr);
        
        return reinterpret_cast<void*>(ret);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_ImageHandlerData(void* ptr)
{
    try
    {
        if (ptr)
        {
            ImageHandlerData* tmp = reinterpret_cast<ImageHandlerData*>(ptr);
            delete tmp;
        }
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_create_RectangleAndImageHandlerData(
    cv::Mat* image_ptr,
    int top_left_x,
    int top_left_y,
    int bottom_right_x,
    int bottom_right_y
)
{
    try
    {
        void* handler = mrwasm_create_ImageHandlerData(image_ptr);
        
        int* rect = new int[4];
        rect[0] = top_left_x;
        rect[1] = top_left_y;
        rect[2] = bottom_right_x;
        rect[3] = bottom_right_y;
        
        RectangleAndImageHandlerData* ret = new RectangleAndImageHandlerData();
        ret->handler = reinterpret_cast<int>(handler);
        ret->rectangle = reinterpret_cast<int>(rect);
        return reinterpret_cast<void*>(ret);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_RectangleAndImageHandlerData(void* ptr)
{
    try
    {
        if (ptr)
        {
            RectangleAndImageHandlerData* tmp = reinterpret_cast<RectangleAndImageHandlerData*>(ptr);
            mrwasm_destroy_ImageHandlerData(
                reinterpret_cast<void*>(tmp->handler)
            );
            delete[] reinterpret_cast<int*>(tmp->rectangle);
            delete tmp;
        }
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

uint8_t* mrwasm_create_image_buffer(const int img_binary_length)
{
    try
    {
        return (new uint8_t[img_binary_length * sizeof(uint8_t)]);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_create_image_ptr(
    uint8_t* img_binary,
    const int img_binary_length,
    const int img_width,
    const int img_height
)
{
    try
    {
        // This function tries to convert 1D array pixel data into cv::Mat image
        // We assume input pixel data is in RGBA order
        // This conversion may be incorrect, which may generate incorrect output result
        
        // Determine number of image channels using: (number of blob elements) / (number of pixels)
        int num_of_channels = img_binary_length / (img_width * img_height);
        if (num_of_channels != 1 &&
            num_of_channels != 3 &&
            num_of_channels != 4)
        {
            num_of_channels = 4;
        }
        
        // cv::Mat constructor used
        // cv::Mat (int rows, int cols, int type, void *data, size_t step=AUTO_STEP)
        // rows => Number of rows in a 2D array. (height)
        // cols => Number of columns in a 2D array. (width)
        cv::Mat* output = new cv::Mat();
        cv::Mat image = cv::Mat(
            img_height, img_width,
            CV_MAKETYPE(CV_8U, num_of_channels),
            img_binary
        );
        cv::cvtColor(image, *output, cv::COLOR_RGBA2BGRA);
        
        return reinterpret_cast<void*>(output);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_image(uint8_t* img_binary, void* img_ptr)
{
    try
    {
        if (img_binary)
            delete[] img_binary;
        if (img_ptr)
            delete reinterpret_cast<cv::Mat*>(img_ptr);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_create_homography_matrix_ptr(
    uint8_t* matrix_binary
)
{
    try
    {
        float* matrix_binary_float = reinterpret_cast<float*>(matrix_binary);
        cv::Mat* ret = new cv::Mat(
            3, 3,
            // CV_MAKETYPE(1, CV_32F),
            CV_32F,
            matrix_binary_float
        );
        
        return reinterpret_cast<void*>(ret);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_homography_matrix_ptr(void* ptr)
{
    try
    {
        if (ptr)
        {
            cv::Mat* tmp = reinterpret_cast<cv::Mat*>(ptr);
            delete tmp;
        }
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}
