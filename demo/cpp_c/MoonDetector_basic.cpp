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


// MoonRegistration library api
#include "MoonRegistration/mrapi.hpp"


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: ./MoonDetector_basic [IMAGE_FOLDER]\n";
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
            
            
            // calculate moon position
            mr::Circle final_circle = detector.detect_moon();
            
            
            // printing out result
            std::cout << "\n\n\n";
            std::cout << "file: \'" << dirEntry.path().string() << "\'" << "\n";
            std::cout << "Circle: " << final_circle << "\n";
            std::cout << "Square: " << mr::circle_to_square_s(final_circle) << "\n";
            std::cout << "Rectangle: " << mr::circle_to_rectangle_s(final_circle) << "\n";
        }
        catch (const std::exception& error)
        {
            std::cerr << "Exception: " << error.what() << "\n";
            return -1;
        }
    }
    
    return 0;
}
