#include "../include/ndarray_converter.h"
#include "../include/preprocessing_pywrapper.hpp"

#include "MoonRegistration/preprocessing.hpp"


py::tuple wrap_resize_with_aspect_ratio(
    const cv::Mat& image_in,
    int width,
    int height,
    int longer_side_val,
    int inter
)
{
    cv::Mat image_out;
    float ratio_out = 0.0;
    
    mr::resize_with_aspect_ratio(
        image_in, image_out,
        ratio_out,
        width, height,
        longer_side_val,
        inter
    );
    
    return py::make_tuple(py::float_(ratio_out), image_out);
}

cv::Mat wrap_apply_brightness_contrast(
    const cv::Mat& image_in,
    int brightness,
    int contrast
)
{
    cv::Mat image_out;
    
    mr::apply_brightness_contrast(
        image_in, image_out,
        brightness, contrast
    );
    
    return image_out;
}

float wrap_calc_img_brightness_perc(
    const cv::Mat& image_in
)
{
    return mr::calc_img_brightness_perc(image_in);
}

float wrap_calc_circle_brightness_perc(
    const cv::Mat& image_in,
    int center_x,
    int center_y,
    int radius
)
{
    return mr::calc_circle_brightness_perc(image_in, center_x, center_y, radius);
}

py::tuple wrap_cut_image_from_circle(
    const cv::Mat& image_in,
    int x,
    int y,
    int radius,
    int padding
)
{
    cv::Mat image_out;
    mr::Rectangle rect_out;
    
    mr::cut_image_from_circle(
        image_in, image_out,
        rect_out,
        x, y, radius,
        padding
    );
    
    return py::make_tuple(rect_out, image_out);
}
