#include <opencv2/core/mat.hpp>

#include "MoonRegistration/c_mrapi/MoonRegistrate_c_api.h"
#include "MoonRegistration/c_mrapi/internal_c_api.hpp"
#include "MoonRegistration/MoonRegistrate.hpp"


extern "C" {

EXPORT_SYMBOL mat_ptr mrc_registrar_draw_layer_image(
    mat_ptr user_image,
    mat_ptr model_image,
    mat_ptr layer_image,
    const int mrc_algorithm,
    const float layer_image_transparency,
    const unsigned char* filter_px,
    char** error_message
)
{
    try
    {
        cv::Mat& mat_user_image = mrc_ptr_to_mat(user_image);
        cv::Mat& mat_model_image = mrc_ptr_to_mat(model_image);
        cv::Mat& mat_layer_image = mrc_ptr_to_mat(layer_image);
        
        mr::MoonRegistrar registrar(
            mat_user_image, mat_model_image,
            static_cast<mr::RegistrationAlgorithms>(mrc_algorithm)
        );
        
        registrar.compute_registration();
        
        cv::Mat* image_out = new cv::Mat();
        cv::Vec4b* vec4b_filter_px = NULL;
        if (filter_px)
        {
            vec4b_filter_px = new cv::Vec4b(
                filter_px[0],
                filter_px[1],
                filter_px[2],
                filter_px[3]
            );
        }
        registrar.draw_layer_image(
            mat_layer_image, *image_out,
            layer_image_transparency,
            vec4b_filter_px
        );
        if (vec4b_filter_px)
            delete vec4b_filter_px;
        
        return mrc_matptr_to_ptr(image_out);
    }
    catch(const std::exception& error)
    {
        if (error_message != NULL)
            *error_message = (char*)error.what();
    }
    return NULL;
}

}
