#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

#include <exception>

#include "MoonRegistration/MoonDetect/detector.hpp"
#include "MoonRegistration/MoonDetect/selector.hpp"
#include "MoonRegistration/MoonDetect/default_steps.hpp"
#include "MoonRegistration/imgprocess.hpp"
#include "MoonRegistration/utils.hpp"


namespace mr
{

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
    const int algorithm
)
{
    detected_circles.clear();
    
    cv::HoughCircles(
        image_in, detected_circles,
        algorithm,
        dp, minDist, param1, param2, minRadius, maxRadius
    );
    
    if (detected_circles.size() >= circle_threshold)
        throw std::runtime_error("Found Too Many Circles.");
}


EXPORT_SYMBOL MoonDetector::MoonDetector()
{
    this->update_hough_circles_algorithm(this->hough_circles_algorithm);
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const std::string& image_filepath)
{
    this->init_by_path(image_filepath);
    this->update_hough_circles_algorithm(this->hough_circles_algorithm);
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const std::vector<unsigned char>& image_binary)
{
    this->init_by_byte(image_binary);
    this->update_hough_circles_algorithm(this->hough_circles_algorithm);
}

EXPORT_SYMBOL MoonDetector::MoonDetector(const cv::Mat& cv_image)
{
    this->init_by_mat(cv_image);
    this->update_hough_circles_algorithm(this->hough_circles_algorithm);
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

EXPORT_SYMBOL void MoonDetector::update_hough_circles_algorithm(const mr::HoughCirclesAlgorithm& algorithm)
{
    switch (algorithm)
    {
    case mr::HoughCirclesAlgorithm::HOUGH_GRADIENT:
        this->preprocess_steps = mr::HG_default_preprocess_steps;
        this->param_init = mr::HG_default_param_init;
        this->iteration_param_update = mr::HG_default_iteration_param_update;
        this->iteration_circle_select = mr::HG_default_iteration_circle_select;
        this->coordinate_remap = mr::HG_default_coordinate_remap;
        break;
    
// Starting from OpenCV 4.8.1, algorithm HOUGH_GRADIENT_ALT is available for cv::HoughCircles().
// This enum will be enabled if OpenCV version >= 4.8.1
#ifdef MR_HAVE_HOUGH_GRADIENT_ALT
    case mr::HoughCirclesAlgorithm::HOUGH_GRADIENT_ALT:
        this->preprocess_steps = mr::HGA_default_preprocess_steps;
        this->param_init = mr::HGA_default_param_init;
        this->iteration_param_update = mr::HGA_default_iteration_param_update;
        this->iteration_circle_select = mr::HGA_default_iteration_circle_select;
        this->coordinate_remap = mr::HGA_default_coordinate_remap;
        break;
    case mr::HoughCirclesAlgorithm::HOUGH_GRADIENT_MIX:
        this->preprocess_steps = mr::HGM_default_preprocess_steps;
        this->param_init = mr::HGM_default_param_init;
        this->iteration_param_update = mr::HGM_default_iteration_param_update;
        this->iteration_circle_select = mr::HGM_default_iteration_circle_select;
        this->coordinate_remap = mr::HGM_default_coordinate_remap;
        break;
#endif
    default:
        throw std::runtime_error("Invalid or empty HoughCirclesAlgorithm.");
        break;
    }
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
    
    cv::Size initial_image_size = this->process_image.size();
    mr::ImageShape image_shape = mr::calc_image_shape(this->process_image);
    
    int max_iteration;
    int circle_threshold;
    int hough_circles_algorithm;
    double dp;
    double minDist;
    double minRadiusRate;
    int minRadius;
    double maxRadiusRate;
    int maxRadius;
    double param1;
    double param2;
    int cut_circle_padding;
    
    this->param_init(
        image_shape,
        max_iteration,
        circle_threshold,
        hough_circles_algorithm,
        dp,
        minDist,
        minRadiusRate, minRadius,
        maxRadiusRate, maxRadius,
        param1, param2,
        cut_circle_padding
    );
    
    std::vector<std::tuple<int, mr::Circle, mr::Rectangle>> result_list(max_iteration);
    mr::Circle circle_found = {-1, -1, -1};
    
    for (int iteration = 0; iteration < max_iteration; ++iteration)
    {
        cv::Mat curr_process_image = this->process_image.clone();
        float image_brightness_perc = mr::calc_img_brightness_perc(
            curr_process_image
        );
        
        this->iteration_param_update(
            iteration,
            image_brightness_perc,
            initial_image_size,
            image_shape,
            circle_found,
            max_iteration,
            circle_threshold,
            hough_circles_algorithm,
            curr_process_image,
            dp, minDist,
            minRadiusRate, minRadius,
            maxRadiusRate, maxRadius,
            param1, param2,
            cut_circle_padding
        );
        
        std::vector<cv::Vec3f> detected_circles;
        mr::find_circles_in_img(
            curr_process_image,
            detected_circles,
            circle_threshold,
            dp, minDist,
            minRadius, maxRadius,
            param1, param2,
            hough_circles_algorithm
        );
        
        circle_found = this->iteration_circle_select(
            iteration,
            max_iteration,
            curr_process_image,
            detected_circles
        );
        
        // cannot select circle, (select circle function failed)
        // maybe we didn't find any circle
        // in iteration 0, which means input image doesn't contain any circle, return {-1, -1, -1}
        if (iteration == 0 && !mr::is_valid_circle(circle_found))
            return circle_found;
        // use the center of image as the new circle
        else if (!mr::is_valid_circle(circle_found))
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

