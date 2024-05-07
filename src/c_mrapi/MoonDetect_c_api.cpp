#include <string>
#include <vector>
#include <exception>

#include <opencv2/core.hpp>

#include "MoonRegistration/c_mrapi/MoonDetect_c_api.h"
#include "MoonRegistration/c_mrapi/utils_c_api.h"
#include "MoonRegistration/c_mrapi/internal_c_api.hpp"
#include "MoonRegistration/MoonDetect.hpp"


extern "C"
{

EXPORT_SYMBOL int* mrc_detect_moon(mat_ptr image, char** error_message)
{
    try
    {
        cv::Mat& mat_image = mrc_ptr_to_mat(image);
        mr::MoonDetector detector(mat_image);
        mr::Circle circle = detector.detect_moon();
        if (!mr::is_valid_circle(circle))
            throw std::runtime_error("Cannot find moon circle.");
        
        int* retval = new int[3];
        retval[0] = circle.x;
        retval[1] = circle.y;
        retval[2] = circle.radius;
        return retval;
    }
    catch(const std::exception& error)
    {
        if (error_message != NULL)
            *error_message = (char*)error.what();
        return NULL;
    }
}

}
