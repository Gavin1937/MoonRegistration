#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>
#include <tuple>
#include <functional>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"

#include "MoonRegistration/shapes.hpp"
#include "MoonRegistration/utils.hpp"


namespace mr
{

// A wrapper function around cv::HoughCircles
// 
// Parameters:
//   - image_in: gray scaled input image
//   - detected_circles: output detected circles vector
//   - circle_threshold: threshold on number of circles to search
//     if detected_circles > circle_threshold, function will throw runtime_error
//   - dp: OpenCV parameter, inverse ratio of the accumulator resolution to the image resolution
//   - minDist: OpenCV parameter, minimum distance between the centers of the detected circles
//   - minRadius: OpenCV parameter, minimum circle radius
//   - maxRadius: OpenCV parameter, maximum circle radius
//   - param1: OpenCV parameter, first method-specific parameter
//   - param2: OpenCV parameter, second method-specific parameter
EXPORT_SYMBOL void find_circles_in_img(
    const cv::Mat& image_in,
    std::vector<cv::Vec3f>& detected_circles,
    const int circle_threshold,
    const double dp,
    const double minDist,
    const int minRadius,
    const int maxRadius,
    const double param1,
    const double param2
);

EXPORT_SYMBOL void default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
);

EXPORT_SYMBOL void default_param_init(
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL void default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

EXPORT_SYMBOL mr::Circle default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

EXPORT_SYMBOL mr::Circle default_coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
);


EXPORT_SYMBOL typedef class MoonDetector
{
public:
    
    // default constructor
    // its here for better compatibility
    // you MUST initialize original_image later with init_by_path() or init_by_byte()
    // otherwise is_empty() will return false, and detect_moon will throw a runtime_error
    EXPORT_SYMBOL MoonDetector();
    
    EXPORT_SYMBOL MoonDetector(const std::string& image_filepath);
    
    EXPORT_SYMBOL MoonDetector(const std::vector<unsigned char>& image_binary);
    
    // taking raw cv::Mat image as input
    // this constructor will assume cv_image is a decoded pixel matrix ready to use
    // it will set cv_image to original_image directly
    // colors in cv_image MUST in BGR order
    EXPORT_SYMBOL MoonDetector(const cv::Mat& cv_image);
    
    
    EXPORT_SYMBOL bool is_empty();
    
    // (re)init mr::MoonDetector by image_filepath
    EXPORT_SYMBOL void init_by_path(const std::string& image_filepath);
    
    // (re)init mr::MoonDetector by image_binary
    EXPORT_SYMBOL void init_by_byte(const std::vector<unsigned char>& image_binary);
    
    // (re)init mr::MoonDetector by image_in
    EXPORT_SYMBOL void init_by_mat(const cv::Mat& image_in);
    
    // trying to find a circle from input image
    // thats most likely contains the moon.
    // 
    // Returns:
    //   - if success, return mr::Circle of the circle found
    //   - if fail (input doesn't contain circle), return mr::Circle of {-1, -1, -1}
    EXPORT_SYMBOL mr::Circle detect_moon();
    
    
    // Following public members of mr::MoonDetector are function pointers
    // They are functions handling different steps in mr::MoonDetector::detect_moon()
    // You can modify them to further customize how mr::MoonDetector::detect_moon() works
    // All the function pointers are default to default_... functions defined in "detector.hpp"
    
    std::function<void(const cv::Mat&, cv::Mat&, float&)> preprocess_steps = nullptr;
    std::function<void(const ImageShape&, int&, int&, double&, double&, double&, int&, double&, int&, double&, double&)> param_init = nullptr;
    std::function<void(const int, const float, const ImageShape&, int&, int&, double&, double&, double&, int&, double&, int&, double&, double&)> iteration_param_update = nullptr;
    std::function<mr::Circle(const int, const cv::Mat&, const std::vector<cv::Vec3f>&)> iteration_circle_select = nullptr;
    std::function<mr::Circle(const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>&, const float)> coordinate_remap = nullptr;
    
private:
    float resize_ratio;
    cv::Mat original_image;
    cv::Mat process_image;
    
} MoonDetector;

}
