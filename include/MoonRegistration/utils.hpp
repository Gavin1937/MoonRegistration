#pragma once

#include <opencv2/core/mat.hpp>

#include "macros.h"


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

}
