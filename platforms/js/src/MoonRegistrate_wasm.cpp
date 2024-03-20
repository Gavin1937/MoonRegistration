#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

#include <exception>

#include "MoonRegistration/MoonRegistrate.hpp"

#include "utils_wasm.hpp"


extern "C"
{

void* mrwasm_transform_user_image(
    void* user_image,
    void* model_image,
    const int algorithm
)
{
    try
    {
        cv::Mat* user_image_ptr = reinterpret_cast<cv::Mat*>(user_image);
        cv::Mat* model_image_ptr = reinterpret_cast<cv::Mat*>(model_image);
        
        mr::MoonRegistrar registrar(
            *user_image_ptr, *model_image_ptr,
            static_cast<mr::RegistrationAlgorithms>(algorithm)
        );
        
        registrar.compute_registration();
        
        cv::Mat* image_out = new cv::Mat();
        registrar.transform_user_image(*image_out);
        
        // convert image_out from BGRA to RGBA so the color don't go wrong
        cv::cvtColor(*image_out, *image_out, cv::COLOR_BGRA2RGBA);
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_transform_layer_image(
    void* user_image,
    void* model_image,
    void* layer_image,
    const int algorithm
)
{
    try
    {
        cv::Mat* user_image_ptr = reinterpret_cast<cv::Mat*>(user_image);
        cv::Mat* model_image_ptr = reinterpret_cast<cv::Mat*>(model_image);
        cv::Mat* layer_image_ptr = reinterpret_cast<cv::Mat*>(layer_image);
        
        mr::MoonRegistrar registrar(
            *user_image_ptr, *model_image_ptr,
            static_cast<mr::RegistrationAlgorithms>(algorithm)
        );
        
        registrar.compute_registration();
        
        cv::Mat* image_out = new cv::Mat();
        registrar.transform_layer_image(*layer_image_ptr, *image_out);
        
        // convert image_out from BGRA to RGBA so the color don't go wrong
        cv::cvtColor(*image_out, *image_out, cv::COLOR_BGRA2RGBA);
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_draw_layer_image(
    void* user_image,
    void* model_image,
    void* layer_image,
    const int algorithm,
    const float layer_image_transparency,
    const int filter_px
)
{
    try
    {
        cv::Mat* user_image_ptr = reinterpret_cast<cv::Mat*>(user_image);
        cv::Mat* model_image_ptr = reinterpret_cast<cv::Mat*>(model_image);
        cv::Mat* layer_image_ptr = reinterpret_cast<cv::Mat*>(layer_image);
        
        mr::MoonRegistrar registrar(
            *user_image_ptr, *model_image_ptr,
            static_cast<mr::RegistrationAlgorithms>(algorithm)
        );
        
        registrar.compute_registration();
        
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
        
        registrar.draw_layer_image(
            *layer_image_ptr, *image_out,
            layer_image_transparency,
            vec4b_filter_px
        );
        if (vec4b_filter_px)
            delete vec4b_filter_px;
        
        // convert image_out from BGRA to RGBA so the color don't go wrong
        cv::cvtColor(*image_out, *image_out, cv::COLOR_BGRA2RGBA);
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}

