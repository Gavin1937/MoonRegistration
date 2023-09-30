#include "../include/MoonRegistration/utils.hpp"


namespace mr
{

EXPORT_SYMBOL ImageShape calc_image_shape(const cv::Mat& image_in)
{
    ImageShape shape;
    shape.height = image_in.size[0];
    shape.width = image_in.size[1];
    if (shape.height >= shape.width)
    {
        shape.longer_side = shape.height;
        shape.shorter_side = shape.width;
    }
    else
    {
        shape.longer_side = shape.width;
        shape.shorter_side = shape.height;
    }
    
    return shape;
}

EXPORT_SYMBOL cv::Vec3i round_vec3f(const cv::Vec3f& vec3)
{
    return cv::Vec3i(
        static_cast<int>(round(vec3[0])),
        static_cast<int>(round(vec3[1])),
        static_cast<int>(round(vec3[2]))
    );
}

}
