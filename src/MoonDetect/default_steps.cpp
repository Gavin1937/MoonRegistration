#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

#include <cmath>

#include "MoonRegistration/MoonDetect/default_steps.hpp"
#include "MoonRegistration/MoonDetect/selector.hpp"


namespace mr
{

EXPORT_SYMBOL void HG_default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
)
{
    // we need to keep the aspect ratio
    // so we can rescale the output x/y coordinate back to match original image
    mr::resize_with_aspect_ratio(image_in, image_out, resize_ratio_out, -1, -1, 500);
    cv::Mat buff;
    
    // creating gray scale version of image needed for HoughCircles
    cv::cvtColor(image_out, buff, cv::COLOR_BGR2GRAY);
    
    // set img to maximum contrast
    // only leave black & white pixels
    // usually, moon will be white after this conversion
    mr::apply_brightness_contrast(buff, image_out, 0, 127);
    
    // set gray image to black & white only image by setting its threshold
    // opencv impl of threshold
    // dst[j] = src[j] > thresh ? maxval : 0;
    // using threshold to binarize img will rm all the branching when calculating img_brightness_perc
    // which make calculation much faster
    cv::threshold(image_out, buff, 0, 255, cv::THRESH_BINARY);
    
    image_out = buff;
}

EXPORT_SYMBOL void HG_default_param_init(
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
)
{
    max_iteration = 3;
    circle_threshold = 100;
    
    dp = std::pow(2, 4);
    minDist = 50;
    if (minDist > (image_shape.shorter_side/2))
        minDist = (image_shape.shorter_side/2);
    minRadiusRate = 0.4;
    minRadius = static_cast<int>(image_shape.longer_side * minRadiusRate);
    maxRadiusRate = 0.6;
    maxRadius = static_cast<int>(image_shape.longer_side * maxRadiusRate);
    param1 = 20 * 2;
    param2 = 3 * param1;
}

EXPORT_SYMBOL void HG_default_iteration_param_update(
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
)
{
    dp = dp * 2;
    minDist = 50;
    minRadiusRate = minRadiusRate * 0.75;
    minRadius = static_cast<int>(image_shape.longer_side * minRadiusRate);
    maxRadiusRate = maxRadiusRate * 0.75;
    maxRadius = static_cast<int>(image_shape.longer_side * maxRadiusRate);
    param1 = param1 * 2;
    param2 = param2 * 2;
    
    if (image_brightness_perc < 0.01)
    {
        dp *= 4;
        minRadius /= 2;
    }
}

EXPORT_SYMBOL mr::Circle HG_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    mr::Circle output;
    if (iteration == 0)
    {
        // find 5 circles w/ largest radius
        // and then find the one with highest brightness perc
        std::vector<cv::Vec3f> candidate_circles = mr::select_n_circles_by_largest_radius(
            image_in, detected_circles, 5
        );
        output = mr::select_circle_by_brightness_perc(
            image_in, candidate_circles
        );
    }
    
    else if (iteration > 0)
    {
        // find 5 circles w/ highest brightness perc
        // and then find the one with highest number of shape side
        std::vector<cv::Vec3f> candidate_circles = mr::select_n_circles_by_brightness_perc(
            image_in, detected_circles, 5
        );
        output = mr::select_circle_by_shape(
            image_in, candidate_circles
        );
    }
    
    return output;
}

EXPORT_SYMBOL mr::Circle HG_default_coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
)
{
    // re-map final circle from a small part of image back to original image
    mr::Circle final_circle = {-1, -1, -1};
    mr::Rectangle last_rect = {0, 0, 0, 0};
    for (auto it : result_list)
    {
        if (std::get<0>(it) == 0)
        {
            final_circle = std::get<1>(it);
        }
        else
        {
            final_circle.x = std::get<1>(it).x - std::get<2>(it).top_left_x;
            final_circle.y = std::get<1>(it).y - std::get<2>(it).top_left_y;
            final_circle.radius = std::get<1>(it).radius;
            final_circle.x += last_rect.top_left_x;
        }
        final_circle.y += last_rect.top_left_y;
        last_rect.top_left_x += std::get<2>(it).top_left_x;
        last_rect.top_left_y += std::get<2>(it).top_left_y;
        last_rect.bottom_right_x += std::get<2>(it).bottom_right_x;
        last_rect.bottom_right_y += std::get<2>(it).bottom_right_y;
    }
    
    final_circle.x = static_cast<int>(static_cast<float>(final_circle.x) / resize_ratio);
    final_circle.y = static_cast<int>(static_cast<float>(final_circle.y) / resize_ratio);
    final_circle.radius = static_cast<int>(static_cast<float>(final_circle.radius) / resize_ratio);
    return final_circle;
}

}