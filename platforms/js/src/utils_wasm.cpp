#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include <exception>

#include "MoonRegistration/shapes.hpp"

#include "utils_wasm.hpp"


extern "C"
{

// Sometimes, C++ will throw its std::exception in the form of a pointer to js,
// so you will see it as a number print out on the console. This function will
// try to retrieve the exception message from pointer.
// See https://github.com/emscripten-core/emscripten/issues/6330
// Reference: https://github.com/emscripten-core/emscripten/issues/6330#issuecomment-568239092
const char* mrwasm_get_last_exception_msg(int exception_ptr)
{
    return reinterpret_cast<std::exception*>(exception_ptr)->what();
}

void* mrwasm_create_ImageHandlerData(cv::Mat* image_ptr)
{
    try
    {
        ImageHandlerData* ret = new ImageHandlerData();
        ret->img_width = static_cast<int>(image_ptr->size[1]);
        ret->img_height = static_cast<int>(image_ptr->size[0]);
        ret->img_data_length = static_cast<int>(image_ptr->total() * image_ptr->elemSize());
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
        uint8_t* output = new uint8_t[img_binary_length * sizeof(uint8_t)];
        return output;
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_image_buffer(void* ptr)
{
    try
    {
        uint8_t* buffer_ptr = reinterpret_cast<uint8_t*>(ptr);
        delete[] buffer_ptr;
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

// About image color format:
//   - All the cv::Mat* image_ptr and uint8_t* buffer_ptr are formatted in BGRA for opencv
//   - We do the initial conversion in mrwasm_create_image_ptr()
//   - If we need image formatted in RGBA, we call mrwasm_get_rgba_image_ptr() to convert it
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
            CV_MAKETYPE(CV_8U, num_of_channels)
        );
        // memcpy img_binary to ensure it will be hold by Mat
        memcpy(image.data, img_binary, img_binary_length);
        
        // convert color format from RGBA to BGRA
        // so its ready for opencv
        cv::cvtColor(image, *output, cv::COLOR_RGBA2BGRA);
        
        if (img_binary)
            delete[] img_binary;
        
        return reinterpret_cast<void*>(output);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void mrwasm_destroy_image(void* img_ptr)
{
    try
    {
        if (img_ptr)
        {
            cv::Mat* mat_img_ptr = reinterpret_cast<cv::Mat*>(img_ptr);
            mat_img_ptr->~Mat();
            delete mat_img_ptr;
        }
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_get_rgba_image_ptr(void* image_ptr)
{
    cv::Mat* mat_image_ptr = reinterpret_cast<cv::Mat*>(image_ptr);
    cv::Mat buffer;
    
    // convert color from BGRA to RGBA
    // so its ready for JS
    cv::cvtColor(*mat_image_ptr, buffer, cv::COLOR_BGRA2RGBA);
    
    size_t size = buffer.total() * buffer.elemSize();
    uchar* output = new uchar[size];
    memcpy(output, buffer.data, size);
    return reinterpret_cast<void*>(output);
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
            CV_32F
        );
        // memcpy matrix to ensure it will be hold by Mat
        memcpy(ret->data, matrix_binary_float, 9*sizeof(float));
        
        if (matrix_binary)
            delete[] matrix_binary;
        
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
            cv::Mat* mat_ptr = reinterpret_cast<cv::Mat*>(ptr);
            mat_ptr->~Mat();
            delete mat_ptr;
        }
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}
