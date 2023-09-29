#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>
#include <tuple>

#include "../macros.h"
#include "../shapes.hpp"
#include "../utils.hpp"


namespace mr
{

EXPORT_SYMBOL void find_circles_in_img(
    const cv::Mat& image_in,
    std::vector<cv::Vec3f>& detected_circles,
    const double dp,
    const double minDist,
    const double minRadiusRate,
    const int minRadius,
    const double maxRadiusRate,
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
    EXPORT_SYMBOL MoonDetector(const std::string& image_filepath);
    
    EXPORT_SYMBOL MoonDetector(const std::vector<unsigned char>& image_binary);
    
    EXPORT_SYMBOL mr::Circle detect_moon();
    
private:
    float resize_ratio;
    cv::Mat original_image;
    cv::Mat process_image;
    
    // functions
    void (*preprocess_steps)(const cv::Mat&, cv::Mat&, float&) = nullptr;
    void (*param_init)(const ImageShape&, int&, int&, double&, double&, double&, int&, double&, int&, double&, double&) = nullptr;
    void (*iteration_param_update)(const int, const float, const ImageShape&, int&, int&, double&, double&, double&, int&, double&, int&, double&, double&) = nullptr;
    mr::Circle (*iteration_circle_select)(const int, const cv::Mat&, const std::vector<cv::Vec3f>&) = nullptr;
    mr::Circle (*coordinate_remap)(const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>&, const float) = nullptr;
    
} MoonDetector;

}