#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/shapes.hpp"

#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


namespace mr
{

EXPORT_SYMBOL typedef struct ImageShape
{
    int height;
    int width;
    int longer_side;
    int shorter_side;
    bool is_portrait;
    bool is_landscape;
    bool is_square;
} ImageShape;

EXPORT_SYMBOL ImageShape calc_image_shape(const cv::Mat& image_in);

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

// Cut a square image using input circle and output a cv::Mat reference to input
// 
// Parameters:
//   - image_in: input image
//   - image_out: output image, reference of input image
//   - rect_out: mr::Rectangle, top_left & bottom_right coordinate of output square in image_in
//   - x: circle center x
//   - y: circle center y
//   - radius: circle radius
//   - padding: padding pixels to the radius (default 15)
EXPORT_SYMBOL void cut_ref_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    mr::Rectangle& rect_out,
    int x,
    int y,
    int radius,
    int padding         = 15
);

// Cut a square image using input circle and output it as a cv::Mat copy of input
// 
// Parameters:
//   - image_in: input image
//   - image_out: output image, copy of input image
//   - rect_out: mr::Rectangle, top_left & bottom_right coordinate of output square in image_in
//   - x: circle center x
//   - y: circle center y
//   - radius: circle radius
//   - padding: padding pixels to the radius (default 15)
EXPORT_SYMBOL void cut_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    Rectangle& rect_out,
    int x,
    int y,
    int radius,
    int padding         = 15
);

// Sync the width & height of secondary image to primary image.
// So secondary image can fit inside primary image.
// If two images are in the same shape (landscape or portrait),
// resize secondary image with primary image's longer side.
// If two images are not in the same shape (landscape or portrait),
// resize secondary image with primary image's shorter side.
// 
// Parameters:
//   - primary: image for sync reference
//   - secondary: image to sync with primary
EXPORT_SYMBOL void sync_img_size(const cv::Mat& primary, cv::Mat& secondary);

// Sync the width & height of secondary image to primary image.
// So secondary image can fit inside primary image.
// If two images are in the same shape (landscape or portrait),
// resize secondary image with primary image's longer side.
// If two images are not in the same shape (landscape or portrait),
// resize secondary image with primary image's shorter side.
// 
// Parameters:
//   - primary_width: image width for sync reference
//   - primary_height: image height for sync reference
//   - secondary: image to sync with primary
EXPORT_SYMBOL void sync_img_size(const int primary_width, const int primary_height, cv::Mat& secondary);

// Sync the number of channels of secondary image to primary image
// 
// Parameters:
//   - primary: image for sync reference
//   - secondary: image to sync with primary
EXPORT_SYMBOL void sync_img_channel(const cv::Mat& primary, cv::Mat& secondary);

// Sync the number of channels of secondary image to primary image
// 
// Parameters:
//   - primary_channel: image channel for sync reference
//   - secondary: image to sync with primary
EXPORT_SYMBOL void sync_img_channel(const int primary_channel, cv::Mat& secondary);

EXPORT_SYMBOL typedef struct ImageChannels
{
    std::vector<cv::Mat> channels;
    
    EXPORT_SYMBOL ImageChannels() {}
    EXPORT_SYMBOL ImageChannels(const cv::Mat& alpha);
    EXPORT_SYMBOL ImageChannels(const cv::Mat& blue, const cv::Mat& green, const cv::Mat& red);
    EXPORT_SYMBOL ImageChannels(const cv::Mat& blue, const cv::Mat& green, const cv::Mat& red, const cv::Mat& alpha);
    
    EXPORT_SYMBOL const cv::Mat& blue() const;
    EXPORT_SYMBOL const cv::Mat& green() const;
    EXPORT_SYMBOL const cv::Mat& red() const;
    EXPORT_SYMBOL const cv::Mat& alpha() const;
    
    EXPORT_SYMBOL const size_t channel_num() const;
    
} ImageChannels;

EXPORT_SYMBOL void split_img_channel(const cv::Mat& image_in, mr::ImageChannels& channels);

EXPORT_SYMBOL void merge_img_channel(const mr::ImageChannels& channels, cv::Mat& image_out);

// Stack two images together respecting transparency
// 
// Parameters:
//   - background: background image
//   - background_roi: Region of Interest (ROI) of background image
//   - foreground: foreground image
//   - image_out: output image buffer
//   - foreground_transparency: a 0~1 float percentage changing foreground image's transparency,
//     default 1.0
//   - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the foreground image.
//     A pixel will be ignore when all of its values is <= filter_px.
//     Set it to NULL if you don't need it, default NULL.
// 
// Note:
//   - this function is designed to work with different number of color channels
//   - if foreground size is bigger then background_roi size, this function will call mr::sync_img_size()
//     to sync it with background_roi. After sync, this function will place foreground at the center of
//     background_roi automatically.
//   - image_out will follow the maximum number of channels between two input images,
//     thus if one input is grayscale image and another is not, then it will become
//     the blue channel of image_out, as OpenCV uses BGRA pixel order.
//   - when using filter_px, we only read number of elements corresponding to
//     number of channels in foreground. if foreground image has 3 channels, we only read
//     first 3 elements of filter_px. rest of elements will be ignored.
EXPORT_SYMBOL void stack_imgs(
    const cv::Mat& background,
    const cv::Rect background_roi,
    const cv::Mat& foreground,
    cv::Mat& image_out,
    const float foreground_transparency = 1.0f,
    const cv::Vec4b* filter_px = NULL
);

// Stack two images together respecting transparency,
// and write pixel value in-place to the background.
// This function is really similar to mr::stack_imgs(),
// we just write pixel value directly to background image to avoid copy.
// 
// Parameters:
//   - background: background image
//   - background_roi: Region of Interest (ROI) of background image
//   - foreground: foreground image
//   - foreground_transparency: a 0~1 float percentage changing foreground image's transparency,
//     default 1.0
//   - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the foreground image.
//     A pixel will be ignore when all of its values is <= filter_px.
//     Set it to NULL if you don't need it, default NULL.
// 
// Note:
//   - this function is designed to work with different number of color channels
//   - if foreground size is bigger then background_roi size, this function will call mr::sync_img_size()
//     to sync it with background_roi. After sync, this function will place foreground at the center of
//     background_roi automatically.
//   - if foreground number of channel is greater then background, this function will sync
//     its channel number with background.
//   - when using filter_px, we only read number of elements corresponding to
//     number of channels in foreground. if foreground image has 3 channels, we only read
//     first 3 elements of filter_px. rest of elements will be ignored.
EXPORT_SYMBOL void stack_imgs_in_place(
    cv::Mat& background,
    const cv::Rect background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency = 1.0f,
    const cv::Vec4b* filter_px = NULL
);

}
