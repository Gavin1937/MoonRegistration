#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

#include <emscripten/emscripten.h>

#include "MoonRegistration/MoonRegistrate.hpp"

#include "utils_wasm.hpp"

#include <iostream>

extern "C"
{

EMSCRIPTEN_KEEPALIVE
void* mrwasm_draw_layer_image(
    void* user_image,
    void* model_image,
    void* layer_image,
    const int algorithm,
    const float layer_image_transparency,
    const int filter_px
)
{
    cv::Mat* user_image_ptr = reinterpret_cast<cv::Mat*>(user_image);
    cv::Mat* model_image_ptr = reinterpret_cast<cv::Mat*>(model_image);
    cv::Mat* layer_image_ptr = reinterpret_cast<cv::Mat*>(layer_image);
    
    mr::MoonRegistrar registrar(
        *user_image_ptr, *model_image_ptr,
        static_cast<mr::RegistrationAlgorithms>(algorithm)
    );
    
    registrar.compute_registration();
    std::cout << "after compute_registration\n";
    
    cv::Mat* image_out = new cv::Mat();
    cv::Vec4b* vec4b_filter_px = NULL;
    if (filter_px)
    {
        const unsigned char* filter_px_ptr = reinterpret_cast<const unsigned char*>(&filter_px);
        vec4b_filter_px = new cv::Vec4b(
            filter_px_ptr[0],
            filter_px_ptr[1],
            filter_px_ptr[2],
            filter_px_ptr[3]
        );
    }
    std::cout << "before draw_layer_image\n";
    
    registrar.draw_layer_image(
        *layer_image_ptr, *image_out,
        layer_image_transparency,
        vec4b_filter_px
    );
    if (vec4b_filter_px)
        delete vec4b_filter_px;
    
    // convert image_out from BGRA to RGBA so the color don't go wrong
    cv::cvtColor(*image_out, *image_out, cv::COLOR_BGRA2RGBA);
    
    
    // ImageHandlerData ret;
    // ret.img_width = static_cast<long>(image_out->size[1]);
    // ret.img_height = static_cast<long>(image_out->size[0]);
    // ret.img_data_length = static_cast<long>(image_out->total() * image_out->elemSize());
    // ret.buffer_ptr = reinterpret_cast<long>(image_out->data);
    // ret.image_ptr = reinterpret_cast<long>(image_out);
    return mrwasm_create_ImageHandlerData(
        static_cast<int>(image_out->size[1]),
        static_cast<int>(image_out->size[0]),
        static_cast<int>(image_out->total() * image_out->elemSize()),
        reinterpret_cast<long>(image_out->data),
        reinterpret_cast<long>(image_out)
    );
}

}

