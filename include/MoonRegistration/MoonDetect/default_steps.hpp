#pragma once

#include <opencv2/core/mat.hpp>

#include <vector>
#include <tuple>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"

#include "MoonRegistration/imgprocess.hpp"
#include "MoonRegistration/shapes.hpp"


// This header defined default detection steps
// for class MoonDetector differ by the
// HoughCirclesAlgorithms they use.
// Algorithm name to function prefix translation:
// HOUGH_GRADIENT        => HG
// HOUGH_GRADIENT_ALT    => HGA
// HOUGH_GRADIENT_MIX    => HGM

namespace mr
{

// HOUGH_GRADIENT (HG)

EXPORT_SYMBOL void HG_default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
);

EXPORT_SYMBOL void HG_default_param_init(
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL void HG_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL mr::Circle HG_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

EXPORT_SYMBOL mr::Circle HG_default_coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
);


// HOUGH_GRADIENT_ALT (HGA)

EXPORT_SYMBOL void HGA_default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
);

EXPORT_SYMBOL void HGA_default_param_init(
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL void HGA_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL mr::Circle HGA_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

EXPORT_SYMBOL mr::Circle HGA_default_coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
);

}