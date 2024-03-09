#include <exception>

#include "MoonRegistration/MoonDetect.hpp"


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
// 4. convert raw uint8_t array to cv::Mat object (mrwasm_create_image_ptr)
// 5. run moon_detection with C++ memory address (mrwasm_detect_moon)
// 6. free the memory when we are done from js side (mrwasm_destroy_image)


int* mrwasm_detect_moon(void* img_ptr)
{
    try
    {
        cv::Mat* cvtImage = reinterpret_cast<cv::Mat*>(img_ptr);
        
        // clone data from shared heap to c++ local heap
        // preventing potential modification from js side
        mr::MoonDetector detector(cvtImage->clone());
        mr::Circle circle = detector.detect_moon();
        
        // we must re-interpret mr::Circle into an int array
        // in order for js to understand it
        int* output = (int*)malloc(3*sizeof(int));
        output[0] = circle.x;
        output[1] = circle.y;
        output[2] = circle.radius;
        return output;
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}
