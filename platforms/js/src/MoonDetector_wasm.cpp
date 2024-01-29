#include "MoonRegistration/mrapi.hpp"

#include <emscripten/emscripten.h>


extern "C"
{

// Known issue with following wasm wrapper:
// Following wrapper function may produce different moon detection result compare to pure C++.
// This will happens for some file and will not for other files.
// This is probably due to the way we pass image data from js to C++.
// opencv.js (wasm) has no way to decode image from binary
// (mode detail in here: https://github.com/opencv/opencv/issues/17535)
// 
// Therefore, we have to use HTML5 Canvas API as a workaround.
// (detailed explanation: https://web.dev/emscripting-a-c-library/#get-an-image-from-javascript-into-wasm)
// Here is a brief explain of the whole process:
// 1. allocate a block of memory in C++ side & return its starting memory address (mrwasm_create_image_buffer)
// 2. in js side, load image data to a HTML5 Canvas (Canvas will decode image for us)
// 3. then, use js WebAssembly API to write canvas data (decoded raw pixels) to C++ memory address
// 4. run moon_detection with C++ memory address (mrwasm_detect_moon)
// 5. free the memory when we are done from js side (mrwasm_destroy_image_buffer)

EMSCRIPTEN_KEEPALIVE
uint8_t* mrwasm_create_image_buffer(int img_width, int img_height)
{
    return (uint8_t*)malloc(img_width * img_height * 4 * sizeof(uint8_t));
}

EMSCRIPTEN_KEEPALIVE
void mrwasm_destroy_image_buffer(uint8_t* p)
{
    free(p);
}

EMSCRIPTEN_KEEPALIVE
int* mrwasm_detect_moon(uint8_t* img_binary, int img_width, int img_height)
{
    // Following lines tries to convert 1D array pixel data into cv::Mat image
    // We assume input pixel data is in RGBA order
    // This conversion may be incorrect, which may generate incorrect output result
    // 
    // cv::Mat constructor used
    // cv::Mat (int rows, int cols, int type, void *data, size_t step=AUTO_STEP)
    // rows => Number of rows in a 2D array. (height)
    // cols => Number of columns in a 2D array. (width)
    cv::Mat image = cv::Mat(img_height, img_width, CV_8UC4, img_binary);
    cv::Mat cvtImage;
    cv::cvtColor(image, cvtImage, cv::COLOR_RGBA2BGR);
    
    // clone data from shared heap to c++ local heap
    // preventing potential modification from js side
    mr::MoonDetector detector(cvtImage.clone());
    mr::Circle circle = detector.detect_moon();
    
    // we must re-interpret mr::Circle into an int array
    // in order for js to understand it
    int* output = (int*)malloc(3*sizeof(int));
    output[0] = circle.x;
    output[1] = circle.y;
    output[2] = circle.radius;
    return output;
}

}

