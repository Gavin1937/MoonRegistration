#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include <cmath>

#include "MoonRegistration/MoonDetect/default_steps.hpp"
#include "MoonRegistration/MoonDetect/selector.hpp"


namespace mr
{

// HOUGH_GRADIENT (HG)

EXPORT_SYMBOL void HG_default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
)
{
    cv::Mat buff;
    
    // we need to keep the aspect ratio
    // so we can rescale the output x/y coordinate back to match original image
    mr::resize_with_aspect_ratio(image_in, buff, resize_ratio_out, -1, -1, 500);
    
    // creating gray scale version of image needed for HoughCircles
    cv::cvtColor(buff, buff, cv::COLOR_BGR2GRAY);
    
    // rm detail texture
    cv::bilateralFilter(buff, buff, 10, 50, 50);
    
    // add erosion to blur/remove small noices
    // https://docs.opencv.org/3.4/db/df6/tutorial_erosion_dilatation.html
    int erosion_size = 1;
    cv::MorphShapes erosion_shape = cv::MorphShapes::MORPH_CROSS;
    cv::Mat element = cv::getStructuringElement(
        erosion_shape,
        cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        cv::Point(erosion_size, erosion_size)
    );
    cv::erode(buff, buff, element);
    
    // further blur color blocks w/ gaussian blur
    cv::Size gaussian_ksize(9,9);
    double gaussian_sigmaX = 2.0;
    double gaussian_sigmaY = 2.0;
    cv::GaussianBlur(buff, buff, gaussian_ksize, gaussian_sigmaX, gaussian_sigmaY);
    // turn 0-3% white pixel to black
    cv::threshold(buff, buff, static_cast<int>(255*0.03), 255, cv::THRESH_TOZERO);
    // turn 80-100% white pixel to 80% white
    cv::threshold(buff, buff, static_cast<int>(255*0.8), 255, cv::THRESH_TRUNC);
    
    // make image black & white only
    mr::binarize_image(buff, image_out);
}

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
)
{
    max_iteration = 3;
    circle_threshold = 100;
    hough_circles_algorithm = cv::HOUGH_GRADIENT;
    
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
    int& hough_circles_algorithm,
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


// HOUGH_GRADIENT_ALT (HGA)

EXPORT_SYMBOL void HGA_default_preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
)
{
    return mr::HG_default_preprocess_steps(
        image_in, image_out, resize_ratio_out
    );
}

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
)
{
    max_iteration = 1;
    circle_threshold = 100;
    hough_circles_algorithm = cv::HOUGH_GRADIENT_ALT;
    
    dp = 1.5;
    minDist = 50;
    if (minDist > (image_shape.shorter_side/2))
        minDist = (image_shape.shorter_side/2);
    minRadiusRate = 0.1;
    minRadius = static_cast<int>(image_shape.shorter_side * minRadiusRate);
    maxRadiusRate = 4;
    maxRadius = static_cast<int>(image_shape.shorter_side * maxRadiusRate);
    param1 = 300;
    param2 = 0.9;
}

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
)
{
    dp = 1.5;
    minDist = 50;
    if (minDist > (image_shape.shorter_side/2))
        minDist = (image_shape.shorter_side/2);
    minRadiusRate = 0.1;
    minRadius = static_cast<int>(image_shape.shorter_side * minRadiusRate);
    maxRadiusRate = 4;
    maxRadius = static_cast<int>(image_shape.shorter_side * maxRadiusRate);
    param1 = 300;
    param2 = 0.9;
}

EXPORT_SYMBOL mr::Circle HGA_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::HG_default_iteration_circle_select(
        iteration, image_in, detected_circles
    );
}

EXPORT_SYMBOL mr::Circle HGA_default_coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
)
{
    return mr::HG_default_coordinate_remap(
        result_list, resize_ratio
    );
}

}