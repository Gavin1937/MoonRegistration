#include "../include/MoonRegistration/c_mrapi.h"
#include "../include/MoonRegistration/MoonDetect/detector.hpp"
#include "../include/MoonRegistration/shapes.hpp"

#include <string>
#include <vector>

extern "C"
{

EXPORT_SYMBOL int* detect_moon_from_filepath(const char* img_filepath)
{
    std::string filepath(img_filepath);
    mr::MoonDetector detector(filepath);
    mr::Circle circle = detector.detect_moon();
    int* retval = (int*)malloc(3*sizeof(int));
    retval[0] = circle.x;
    retval[1] = circle.y;
    retval[2] = circle.radius;
    return retval;
}

EXPORT_SYMBOL int* detect_moon_from_binary(const unsigned char* img_binary, const int img_size)
{
    std::vector<unsigned char> binary(img_binary, img_binary+img_size);
    mr::MoonDetector detector(binary);
    mr::Circle circle = detector.detect_moon();
    int* retval = (int*)malloc(3*sizeof(int));
    retval[0] = circle.x;
    retval[1] = circle.y;
    retval[2] = circle.radius;
    return retval;
}

}