#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>


#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#elif __has_include(<boost/filesystem.hpp>)
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#endif
#endif


// MoonRegistration library api MoonDetect module
#include "MoonRegistration/MoonDetect.hpp"
#include "MoonRegistration/imgprocess.hpp"


// Following functions are different steps in function mr::MoonDetector::moon_detect()
// You can modify their behavior by rewriting them and setting them to mr::MoonDetector object.
// Checkout "default_steps.cpp" for detailed implementation of these step functions
// ==================================================

void preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
)
{
    mr::HG_default_preprocess_steps(image_in, image_out, resize_ratio_out);
}

void param_init(
    const mr::ImageShape& image_shape,
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
    double& param2,
    int& cut_circle_padding
)
{
    mr::HG_default_param_init(
        image_shape, max_iteration, circle_threshold, hough_circles_algorithm,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
}

void iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const cv::Size& initial_image_size,
    const mr::ImageShape& image_shape,
    const mr::Circle& curr_circle_found,
    const int max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    cv::Mat& process_image,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding)
{
    mr::HG_default_iteration_param_update(
        iteration, image_brightness_perc, initial_image_size,
        image_shape, curr_circle_found, max_iteration, circle_threshold,
        hough_circles_algorithm, process_image, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
}

mr::Circle iteration_circle_select(
    const int iteration,
    const int max_iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::HG_default_iteration_circle_select(
        iteration, max_iteration,
        image_in, detected_circles
    );
}

mr::Circle coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
)
{
    return mr::HG_default_coordinate_remap(result_list, resize_ratio);
}

// ==================================================


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: ./MoonDetect_advance [IMAGE_FOLDER]\n";
        return 0;
    }
    
    std::cout << "MoonRegistration Library Version: " << mr::version() << "\n";
    
    fs::path folder(argv[1]);
    std::cout << "Folder Path: " << folder << "\n";
    for (auto dirEntry : fs::recursive_directory_iterator(folder))
    {
        if (!fs::is_regular_file(dirEntry))
            continue;
        try
        {
            // read image directly from filepath
            mr::MoonDetector detector(dirEntry.path().string());
            
            // // read image from bytes
            // std::ifstream file_in(dirEntry.path().string(), std::ios::binary);
            // file_in.seekg(0, std::ios::end);
            // size_t file_size = file_in.tellg();
            // std::vector<unsigned char> buffer(file_size);
            // file_in.seekg(0);
            // file_in.read((char*)(buffer.data()), file_size);
            // file_in.close();
            // mr::MoonDetector detector(buffer);
            
            // // read image from cv::Mat object
            // cv::Mat cv_image;
            // // fill-in pixel data to cv_image...
            // cv_image = cv::imread(dirEntry.path().string());
            // mr::MoonDetector detector(cv_image);
            
            
            // Update hough circles algorithms and default step functions used
            // You can change the underlying algorithm used in cv::HoughCircles()
            // This function will overwrite all the step functions
            // HOUGH_GRADIENT_ALT and HOUGH_GRADIENT_MIX algorithm only available with OpenCV >= 4.8.1
            // If the library is compiled with OpenCV < 4.8.1, default algorithm is HOUGH_GRADIENT
            // If the library is compiled with OpenCV >= 4.8.1, default algorithm is HOUGH_GRADIENT_MIX
            detector.update_hough_circles_algorithm(mr::HoughCirclesAlgorithms::HOUGH_GRADIENT);
            
            // Following public members of mr::MoonDetector are function pointers
            // They are functions to handle different steps in mr::MoonDetector::detect_moon()
            // You can modify them to further customize how mr::MoonDetector::detect_moon() works
            // All the function pointers are set to ***_default_*** functions defined in "default_steps.hpp" by default
            detector.preprocess_steps = preprocess_steps;
            detector.param_init = param_init;
            detector.iteration_param_update = iteration_param_update;
            detector.iteration_circle_select = iteration_circle_select;
            detector.coordinate_remap = coordinate_remap;
            
            // calculate moon position
            mr::Circle final_circle = detector.detect_moon();
            if (!mr::is_valid_circle(final_circle))
                throw std::runtime_error("Cannot find moon circle.");
            
            
            // printing out result
            std::cout << "\n\n\n";
            std::cout << "file: \'" << dirEntry.path().string() << "\'" << "\n";
            std::cout << "Circle: " << final_circle << "\n";
            std::cout << "Square: " << mr::circle_to_square(final_circle) << "\n";
            std::cout << "Rectangle: " << mr::circle_to_rectangle(final_circle) << "\n";
        }
        catch (const std::exception& error)
        {
            std::cerr << "Exception: " << error.what() << "\n";
            return -1;
        }
    }
    
    return 0;
}
