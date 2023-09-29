#pragma once

#include <opencv2/core/mat.hpp>

#include <vector>

#include "../macros.h"
#include "../shapes.hpp"


namespace mr
{

EXPORT_SYMBOL Circle select_circle_by_brightness_perc(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

EXPORT_SYMBOL std::vector<cv::Vec3f> select_n_circles_by_brightness_perc(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles,
    int n
);

EXPORT_SYMBOL Circle select_circle_by_largest_radius(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

EXPORT_SYMBOL Circle select_circle_by_shape(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

}
