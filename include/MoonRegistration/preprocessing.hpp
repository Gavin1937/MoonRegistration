#pragma once

#include <opencv2/core/mat.hpp>
#include "../include/MoonRegistration/macros.h"

namespace mr
{

EXPORT_SYMBOL void resize_with_aspect_ratio(cv::Mat image, int width, int height, int longer_side_val, int inter);


}