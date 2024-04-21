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
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_compute_registration(
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
        
        cv::Mat flatten_matrix = registrar.get_homography_matrix().reshape(1, 1);
        double* heap_matrix = new double[9];
        for (int i = 0; i < 9; ++i)
            heap_matrix[i] = flatten_matrix.at<double>(i);
        
        return reinterpret_cast<void*>(heap_matrix);
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
        {
            vec4b_filter_px->~Vec();
            delete vec4b_filter_px;
        }
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_draw_layer_image_no_compute(
    void* user_image,
    void* model_image,
    void* layer_image,
    void* homography_matrix,
    const float layer_image_transparency,
    const int filter_px
)
{
    try
    {
        cv::Mat* user_image_ptr = reinterpret_cast<cv::Mat*>(user_image);
        cv::Mat* model_image_ptr = reinterpret_cast<cv::Mat*>(model_image);
        cv::Mat* layer_image_ptr = reinterpret_cast<cv::Mat*>(layer_image);
        cv::Mat* homography_matrix_ptr = reinterpret_cast<cv::Mat*>(homography_matrix);
        
        mr::MoonRegistrar registrar;
        registrar.update_images(*user_image_ptr, *model_image_ptr);
        registrar.update_f2d_detector(mr::RegistrationAlgorithms::EMPTY_ALGORITHM);
        registrar.update_homography_matrix(*homography_matrix_ptr);
        
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
        {
            vec4b_filter_px->~Vec();
            delete vec4b_filter_px;
        }
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}

