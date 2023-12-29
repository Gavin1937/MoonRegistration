#pragma once

#include "macros.h"

#include <opencv2/core/mat.hpp>

#include <string>


namespace mr
{

EXPORT_SYMBOL typedef struct ImageShape
{
    int height;
    int width;
    int longer_side;
    int shorter_side;
} ImageShape;

EXPORT_SYMBOL ImageShape calc_image_shape(const cv::Mat& image_in);

EXPORT_SYMBOL cv::Vec3i round_vec3f(const cv::Vec3f& vec3);

EXPORT_SYMBOL bool file_exists(const std::string& filepath);

}
