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

// default one for mr::MoonRegistrar class
// Filter by ignore_edge_kp first, and then
// filter by lowes_ratio_test
EXPORT_SYMBOL bool default_is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
);

// Filter by lowes_ratio_test only
EXPORT_SYMBOL bool default_is_good_match_lowes_ratio_only(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
);

// Filter by ignore_edge_kp first, and then
// filter by lowes_ratio_test, finally use
// ignore_close_kp to filter a last time
EXPORT_SYMBOL bool default_is_good_match_all(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
);

}
