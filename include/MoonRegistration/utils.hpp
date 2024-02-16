#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <functional>

#include "MoonRegistration/macros.h"


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

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp(TYPE value, TYPE lower, TYPE higher)
{
    return (
        (value < lower) ? lower : ((value > higher) ? higher : value)
    );
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp_lower(TYPE value, TYPE lower)
{
    return ((value < lower) ? lower : value);
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp_higher(TYPE value, TYPE higher)
{
    return ((value > higher) ? higher : value);
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE make_binary_num(TYPE value, TYPE threshold, TYPE minval, TYPE maxval)
{
    return ((value <= threshold) ? minval : maxval);
}


template <typename COMP_TYPE, typename DATA_TYPE>
class Comparator
{
public:
    bool operator()(const std::pair<COMP_TYPE,DATA_TYPE>& lhs, const std::pair<COMP_TYPE,DATA_TYPE>& rhs)
    {
        return (lhs.first > rhs.first);
    }
};

// Find n circles from input circle vector
// 
// Parameters:
//   - n: int, number of circles to return
//   - detected_circles: input circles
//   - output: output buffer
//   - calc_value_func: a function pointer to calculate the value for circle comparison
//   - filter_value_func: a function pointer to filter circle base on the value from calc_value_func. (default return true)
template<typename COMP_TYPE> EXPORT_SYMBOL void find_n_circles(
    const int n,
    const std::vector<cv::Vec3f>& detected_circles,
    std::vector<cv::Vec3f>& output,
    std::function<COMP_TYPE(const cv::Vec3f&)> calc_value_func,
    std::function<bool(const COMP_TYPE&)> filter_value_func =
    [](const COMP_TYPE&) { return true; }
)
{
    output.reserve(n);
    
    // find n largest element in the list
    // https://stackoverflow.com/a/22654973
    
    std::priority_queue<
        std::pair<COMP_TYPE,cv::Vec3f>,
        std::vector<std::pair<COMP_TYPE,cv::Vec3f>>,
        Comparator<COMP_TYPE,cv::Vec3f>
    > minheap;
    
    for (auto vec : detected_circles)
    {
        COMP_TYPE value = calc_value_func(vec);
        
        // minheap.size() < n
        if (filter_value_func(value) && minheap.size() < n)
        {
            minheap.push(std::make_pair(value,vec));
        }
        // minheap.size() >= n
        else if (filter_value_func(value) && value > minheap.top().first)
        {
            minheap.pop();
            minheap.push(std::make_pair(value,vec));
        }
    }
    
    for (int i = 0; i < n; ++i)
    {
        output[i] = minheap.top().second;
        minheap.pop();
    }
}

EXPORT_SYMBOL bool file_exists(const std::string& filepath);

// Sync the width & height of secondary image to primary image
// 
// Parameters:
//   - primary: image for sync reference
//   - secondary: image to sync with primary
EXPORT_SYMBOL void sync_img_size(const cv::Mat& primary, cv::Mat& secondary);

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
//   - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the foreground image,
//     set it to NULL if you don't need it, default NULL
// 
// Note:
//   - this function is designed to work with different number of color channels
//   - if foreground size is bigger then background size, this function will call mr::sync_img_size()
//     to sync it with background
//   - image_out will follow the maximum number of channels between two input images,
//     thus if one input is grayscale image and another is not, then it will become
//     the blue channel of image_out, as OpenCV uses BGRA pixel order.
//   - when using filter_px, we only read number of elements corresponding to
//     number of channels in foreground. if foreground image has 3 channels, we only read
//     first 3 elements of filter_px. rest of elements will be ignored.
EXPORT_SYMBOL void stack_imgs(
    const cv::Mat& background,
    cv::Rect background_roi,
    const cv::Mat& foreground,
    cv::Mat& image_out,
    float foreground_transparency = 1.0f,
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
//   - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the foreground image,
//     set it to NULL if you don't need it, default NULL
// 
// Note:
//   - this function is designed to work with different number of color channels
//   - if foreground size is bigger then background size, this function will call mr::sync_img_size()
//     to sync it with background
//   - if foreground number of channel is greater then background, this function will sync
//     its number of channel with background
//   - when using filter_px, we only read number of elements corresponding to
//     number of channels in foreground. if foreground image has 3 channels, we only read
//     first 3 elements of filter_px. rest of elements will be ignored.
EXPORT_SYMBOL void stack_imgs_in_place(
    cv::Mat& background,
    cv::Rect background_roi,
    const cv::Mat& foreground,
    float foreground_transparency = 1.0f,
    const cv::Vec4b* filter_px = NULL
);

}
