#pragma once

#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


// keypoints descriptor filter functions

namespace mr
{

// Filter keypoints descriptor by Lowe's Ratio Test
// 
// Lower the ratio => larger the differences between keypoints descriptor => more keypoints get filter out
// 
// Algorithm Detail:
// https://stackoverflow.com/a/60343973
// https://www.cs.ubc.ca/~lowe/papers/ijcv04.pdf
// 
// Parameters:
//   - m: DMatch, from matches[0]
//   - n: DMatch, from matches[1]
//   - ratio: float ratio between 0~1
// 
// Returns:
//   - true or false to tell whether we need to filter current keypoints
EXPORT_SYMBOL bool filter_by_lowes_ratio_test(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float ratio = 0.7
);

// Filter keypoints descriptor by ignoring edge keypoints
// 
// Parameter:
//   - user_kpt: user cv::KeyPoint
//   - model_kpt: model cv::KeyPoint
//   - user_image: user image
//   - model_image: model image
//   - radius_guess_ratio: float ratio (0~1) to determine how much of edge to ignore
// 
// Returns:
//   - true or false to tell whether we need to filter current keypoints
EXPORT_SYMBOL bool filter_by_ignore_edge_kp(
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image,
    const float radius_guess_ratio = 0.9
);

// Filter keypoints descriptor by ignoring keypoints that are too close to each other
// 
// Parameter:
//   - user_kpt: user cv::KeyPoint
//   - model_kpt: model cv::KeyPoint
//   - user_image: user image
//   - model_image: model image
//   - hash_coordinate_ratio: float ratio (0~1) to determine how much we want to "blur" keypoints coordinates
//   - is_first_time: bool flag to determine whether we need to cleanup buffer hash map
// 
// Returns:
//   - true or false to tell whether we need to filter current keypoints
EXPORT_SYMBOL bool filter_by_ignore_close_kp(
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image,
    const float hash_coordinate_ratio = 0.3,
    const bool is_first_time = true
);

}
