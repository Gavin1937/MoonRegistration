#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include <emscripten/emscripten.h>


extern "C"
{

EMSCRIPTEN_KEEPALIVE
uint8_t* mrwasm_create_image_buffer(const int img_binary_length)
{
    return (new uint8_t[img_binary_length * sizeof(uint8_t)]);
}

EMSCRIPTEN_KEEPALIVE
void* mrwasm_create_image_ptr(
    uint8_t* img_binary,
    const int img_binary_length,
    const int img_width,
    const int img_height
)
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

EMSCRIPTEN_KEEPALIVE
void mrwasm_destroy_image(uint8_t* img_binary, void* img_ptr)
{
    delete[] img_binary;
    delete reinterpret_cast<cv::Mat*>(img_ptr);
}

}
