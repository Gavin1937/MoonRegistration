#include <opencv2/core/mat.hpp>

#include <iostream>
#include <fstream>
#include <vector>


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


// MoonRegistration library api
#include "MoonRegistration/mrapi.hpp"


// Following functions are different steps in function mr::MoonDetector::moon_detect()
// You can modify their behavior by rewriting them and setting them to mr::MoonDetector object.
// Checkout "detector.cpp" for detailed implementation of these step functions
// ==================================================

void preprocess_steps(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& resize_ratio_out
)
{
    mr::default_preprocess_steps(image_in, image_out, resize_ratio_out);
}

void param_init(
    const mr::ImageShape& image_shape,
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
    mr::default_param_init(
        image_shape, max_iteration, circle_threshold,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2
    );
}

void iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const mr::ImageShape& image_shape,
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
    mr::default_iteration_param_update(
        iteration, image_brightness_perc, image_shape, max_iteration, circle_threshold,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2
    );
}

mr::Circle iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::default_iteration_circle_select(iteration, image_in, detected_circles);
}

mr::Circle coordinate_remap(
    const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list,
    const float resize_ratio
)
{
    return mr::default_coordinate_remap(result_list, resize_ratio);
}

// ==================================================


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: ./MoonDetector_advance /path/to/image/folder\n";
        return 0;
    }
    
    
    fs::path folder(argv[1]);
    std::cout << "Folder Path: " << folder << std::endl;
    for (auto dirEntry : fs::recursive_directory_iterator(folder))
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
        // mr::MoonDetector detector(cv_image);
        
        
        // Following public members of mr::MoonDetector are function pointers
        // They are functions to handle different steps in mr::MoonDetector::detect_moon()
        // You can modify them to further customize how mr::MoonDetector::detect_moon() works
        // All the function pointers are set to default_*** functions defined in "detector.hpp" by default
        detector.preprocess_steps = preprocess_steps;
        detector.param_init = param_init;
        detector.iteration_param_update = iteration_param_update;
        detector.iteration_circle_select = iteration_circle_select;
        detector.coordinate_remap = coordinate_remap;
        
        
        // calculate moon position
        mr::Circle final_circle = detector.detect_moon();
        
        
        // printing out result
        std::cout << "\n\n\n";
        std::cout <<"file.name = \'" << dirEntry.path().filename().string() << "\'" << std::endl;
        std::cout << "Circle: " << final_circle << std::endl;
        std::cout << "Square: " << mr::circle_to_square_s(final_circle) << std::endl;
        std::cout << "Rectangle: " << mr::circle_to_rectangle_s(final_circle) << std::endl;
    }
    
    return 0;
}
