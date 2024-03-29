#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

#include <cmath>
#include <exception>

#include "MoonRegistration/MoonDetect/detector.hpp"
#include "MoonRegistration/MoonDetect/selector.hpp"
#include "MoonRegistration/imgprocess.hpp"
#include "MoonRegistration/utils.hpp"


namespace mr
{

EXPORT_SYMBOL int convertHoughCirclesAlgorithm(const mr::HoughCirclesAlgorithm& algorith)
{
    switch (algorith)
    {
    case mr::HoughCirclesAlgorithm::HOUGH_GRADIENT:
        return cv::HOUGH_GRADIENT;
        break;
// Starting from OpenCV 4.8.1, algorithm HOUGH_GRADIENT_ALT is available for cv::HoughCircles().
// This enum will be enabled if OpenCV version >= 4.8.1
#ifdef MR_HAVE_HOUGH_GRADIENT_ALT
    case mr::HoughCirclesAlgorithm::HOUGH_GRADIENT_ALT:
        return cv::HOUGH_GRADIENT_ALT;
        break;
#endif
    case mr::HoughCirclesAlgorithm::EMPTY_ALGORITHM:
    case mr::HoughCirclesAlgorithm::INVALID_ALGORITHM:
        throw std::runtime_error("Cannot parse empty or invalid HoughCirclesAlgorithm");
        break;
    
    default:
        throw std::runtime_error("Cannot parse empty or invalid HoughCirclesAlgorithm");
        break;
    }
}

EXPORT_SYMBOL void find_circles_in_img(
    const cv::Mat& image_in,
    std::vector<cv::Vec3f>& detected_circles,
    const int circle_threshold,
    const double dp,
    const double minDist,
    const int minRadius,
    const int maxRadius,
    const double param1,
    const double param2,
    const mr::HoughCirclesAlgorithm& algorithm
)
{
    detected_circles.clear();
    
    cv::HoughCircles(
        image_in, detected_circles,
        mr::convertHoughCirclesAlgorithm(algorithm),
        dp, minDist, param1, param2, minRadius, maxRadius
    );
    
    if (detected_circles.empty())
    {
        detected_circles.push_back(cv::Vec3f(
            static_cast<float>((image_in.size[1] / 2)),   // x
            static_cast<float>((image_in.size[0] / 2)),   // y
            static_cast<float>((image_in.size[1] / 2)+3)  // radius
        ));
        return;
    }
    
    if (detected_circles.size() >= circle_threshold)
        throw std::runtime_error("Found Too Many Circles.");
}

EXPORT_SYMBOL void default_preprocess_steps(
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

EXPORT_SYMBOL mr::Circle default_iteration_circle_select(
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

EXPORT_SYMBOL mr::Circle default_coordinate_remap(
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


EXPORT_SYMBOL MoonDetector::MoonDetector()
    : resize_ratio(0.0),
    // default functions
    preprocess_steps(default_preprocess_steps),
    param_init(default_param_init),
    iteration_param_update(default_iteration_param_update),
    iteration_circle_select(default_iteration_circle_select),
    coordinate_remap(default_coordinate_remap)
{
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const std::string& image_filepath)
    : resize_ratio(0.0),
    // default functions
    preprocess_steps(default_preprocess_steps),
    param_init(default_param_init),
    iteration_param_update(default_iteration_param_update),
    iteration_circle_select(default_iteration_circle_select),
    coordinate_remap(default_coordinate_remap)
{
    this->init_by_path(image_filepath);
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const std::vector<unsigned char>& image_binary)
    : resize_ratio(0.0),
    // default functions
    preprocess_steps(default_preprocess_steps),
    param_init(default_param_init),
    iteration_param_update(default_iteration_param_update),
    iteration_circle_select(default_iteration_circle_select),
    coordinate_remap(default_coordinate_remap)
{
    this->init_by_byte(image_binary);
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const cv::Mat& cv_image)
    : resize_ratio(0.0),
    // default functions
    preprocess_steps(default_preprocess_steps),
    param_init(default_param_init),
    iteration_param_update(default_iteration_param_update),
    iteration_circle_select(default_iteration_circle_select),
    coordinate_remap(default_coordinate_remap)
{
    this->init_by_mat(cv_image);
}

EXPORT_SYMBOL bool MoonDetector::is_empty()
{
    return this->original_image.empty();
}

EXPORT_SYMBOL void MoonDetector::init_by_path(const std::string& image_filepath)
{
    if (!file_exists(image_filepath))
        throw std::runtime_error("Empty Input Image");
    this->original_image = cv::imread(image_filepath, cv::IMREAD_UNCHANGED);
    if (this->original_image.empty())
        throw std::runtime_error("Empty Input Image");
}

EXPORT_SYMBOL void MoonDetector::init_by_byte(const std::vector<unsigned char>& image_binary)
{
    this->original_image = cv::imdecode(cv::Mat(image_binary), cv::IMREAD_UNCHANGED);
    if (this->original_image.empty())
        throw std::runtime_error("Empty Input Image");
}

EXPORT_SYMBOL void MoonDetector::init_by_mat(const cv::Mat& image_in)
{
    this->original_image = image_in.clone();
    if (this->original_image.empty())
        throw std::runtime_error("Empty Input Image");
}

EXPORT_SYMBOL mr::Circle MoonDetector::detect_moon()
{
    if (this->is_empty())
        throw std::runtime_error("Empty Input Image");
    
    this->preprocess_steps(
        this->original_image,
        this->process_image,
        this->resize_ratio
    );
    
    mr::ImageShape image_shape = mr::calc_image_shape(this->process_image);
    
    int max_iteration;
    int circle_threshold;
    double dp;
    double minDist;
    double minRadiusRate;
    int minRadius;
    double maxRadiusRate;
    int maxRadius;
    double param1;
    double param2;
    
    this->param_init(
        image_shape,
        max_iteration,
        circle_threshold,
        dp,
        minDist,
        minRadiusRate, minRadius,
        maxRadiusRate, maxRadius,
        param1, param2
    );
    
    std::vector<std::tuple<int, mr::Circle, mr::Rectangle>> result_list(max_iteration);
    
    for (int iteration = 0; iteration < max_iteration; ++iteration)
    {
        float image_brightness_perc = mr::calc_img_brightness_perc(
            this->process_image
        );
        
        this->iteration_param_update(
            iteration,
            image_brightness_perc,
            image_shape,
            max_iteration,
            circle_threshold,
            dp, minDist,
            minRadiusRate, minRadius,
            maxRadiusRate, maxRadius,
            param1, param2
        );
        
        std::vector<cv::Vec3f> detected_circles;
        mr::find_circles_in_img(
            this->process_image,
            detected_circles,
            circle_threshold,
            dp, minDist,
            minRadius, maxRadius,
            param1, param2,
            hough_circles_algorithm
        );
        
        mr::Circle circle_found = this->iteration_circle_select(
            iteration,
            this->process_image,
            detected_circles
        );
        
        // cannot select circle, (select circle function failed)
        // maybe we didn't find any circle
        // in iteration 0, which means input image doesn't contain any circle, return {-1, -1, -1}
        if (iteration == 0 && !mr::is_valid_circle_s(circle_found))
            return circle_found;
        // use the center of image as the new circle
        else if (!mr::is_valid_circle_s(circle_found))
            circle_found = {image_shape.width/2, image_shape.height/2, (image_shape.width/2)+3};
        
        // cut out part of img from circle
        cv::Mat buff;
        mr::Rectangle rect_out;
        mr::cut_ref_image_from_circle(
            this->process_image,
            buff,
            rect_out,
            circle_found.x, circle_found.y, circle_found.radius,
            30
        );
        this->process_image = buff;
        result_list[iteration] = std::make_tuple(
            iteration, circle_found, rect_out
        );
        
        image_shape = mr::calc_image_shape(this->process_image);
    }
    
    mr::Circle final_circle = this->coordinate_remap(
        result_list, this->resize_ratio
    );
    
    return final_circle;
}

}

