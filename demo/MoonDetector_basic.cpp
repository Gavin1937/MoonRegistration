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


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: ./MoonDetector_basic /path/to/image/folder\n";
        return 0;
    }
    
    std::cout << "MoonRegistration Library Version: " << mr::version() << std::endl;
    
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
