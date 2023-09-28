#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/MoonRegistration/macros.h"

namespace mr
{

// original algorithm
// https://stackoverflow.com/a/58126805
// 
// Parameters:
//   - image_in: input image
//   - image_out: output image
//   - ratio_out: output resize ratio
//   - width: resize image to this width (-1 to ignore)
//   - height: resize image to this height (-1 to ignore)
//   - longer_side_val: resize image's longer side to this value (-1 to ignore)
//   - inter: interpolation for cv::resize()
EXPORT_SYMBOL void resize_with_aspect_ratio(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& ratio_out,
    int width           = -1,
    int height          = -1,
    int longer_side_val = -1,
    int inter           = cv::INTER_AREA
);

// original algorithm
// https://stackoverflow.com/a/50053219
// 
// Parameters:
//   - image_in: input image
//   - image_out: output image
//   - brightness: set image brightness value (-127 to 127) (default 0)
//   - contrast: set image contrast value (-127 to 127) (default 0)
EXPORT_SYMBOL void apply_brightness_contrast(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    int brightness      = 0,
    int contrast        = 0
);

// Assuming all pixel values in image_in are black(0) / white(255)
// sum up all the pixels in given image, and then
// calculate its mean. The return value represents
// how many white pixels in the image, how bright it is.
// 
// Parameters:
//   - image_in: input image
// 
// Returns:
//   - float between 0 to 1
EXPORT_SYMBOL float calc_img_brightness_perc(
    const cv::Mat& image_in
);

// Assuming all pixel values in image_in are black(0) / white(255)
// sum up all the pixels in given circle, and then
// calculate its mean. The return value represents
// how many white pixels in the image, how bright it is.
// 
// part of algorithm
// https://stackoverflow.com/a/74813748
// 
// Parameters:
//   - image_in: input image
//   - center_x: circle center x
//   - center_y: circle center y
//   - radius: circle radius
// 
// Returns:
//   - float between 0 to 1
EXPORT_SYMBOL float calc_circle_brightness_perc(
    const cv::Mat& image_in,
    int center_x,
    int center_y,
    int radius
);

// Cut a square image using input circle
// 
// Parameters:
//   - image_in: input image
//   - image_out: output image
//   - top_left: cv::Point, top_left coordinate of output square in image_in
//   - bottom_right: cv::Point, bottom_right coordinate of output square in image_in
//   - x: circle center x
//   - y: circle center y
//   - radius: circle radius
//   - padding: padding pixels to the radius (default 15)
EXPORT_SYMBOL void cut_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    cv::Point& top_left,
    cv::Point& bottom_right,
    int x,
    int y,
    int radius,
    int padding         = 15
);

}