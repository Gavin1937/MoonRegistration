#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


// This header defined default detection steps
// for class MoonRegistrar

namespace mr
{

EXPORT_SYMBOL bool default_is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
);

}