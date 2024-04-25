#pragma once

#include <opencv2/core/types.hpp>

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
EXPORT_SYMBOL bool filter_by_lowes_ratio_test(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float ratio = 0.7
);

}
