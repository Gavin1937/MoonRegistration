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

}
