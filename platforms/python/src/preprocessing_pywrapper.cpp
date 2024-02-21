#include "preprocessing_pywrapper.hpp"

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


// initialize submodule
void init_preprocessing(py::module &module)
{
    // preprocessing.hpp
    module.def("resize_with_aspect_ratio", &wrap_resize_with_aspect_ratio,
        py::arg("image_in"),
        py::arg("width")           = -1,
        py::arg("height")          = -1,
        py::arg("longer_side_val") = -1,
        py::arg("inter")           = static_cast<int>(cv::INTER_AREA),
        R"pbdoc(
    original algorithm
    https://stackoverflow.com/a/58126805
    
    Parameters:
      - image_in: input image
      - image_out: output image
      - ratio_out: output resize ratio
      - width: resize image to this width (-1 to ignore)
      - height: resize image to this height (-1 to ignore)
      - longer_side_val: resize image's longer side to this value (-1 to ignore)
      - inter: interpolation for cv::resize()
        )pbdoc"
    );
    module.def("apply_brightness_contrast", &wrap_apply_brightness_contrast,
        py::arg("image_in"),
        py::arg("brightness")      = 0,
        py::arg("contrast")        = 0,
        R"pbdoc(
    original algorithm
    https://stackoverflow.com/a/50053219
    
    Parameters:
      - image_in: input image
      - image_out: output image
      - brightness: set image brightness value (-127 to 127) (default 0)
      - contrast: set image contrast value (-127 to 127) (default 0)
        )pbdoc"
    );
    module.def("calc_img_brightness_perc", &wrap_calc_img_brightness_perc,
        py::arg("image_in"),
        R"pbdoc(
    Assuming all pixel values in image_in are black(0) / white(255)
    sum up all the pixels in given image, and then
    calculate its mean. The return value represents
    how many white pixels in the image, how bright it is.
    
    Parameters:
      - image_in: input image
    
    Returns:
      - float between 0 to 1
        )pbdoc"
    );
    module.def("calc_circle_brightness_perc", &wrap_calc_circle_brightness_perc,
        py::arg("image_in"),
        py::arg("center_x"),
        py::arg("center_y"),
        py::arg("radius"),
        R"pbdoc(
    Assuming all pixel values in image_in are black(0) / white(255)
    sum up all the pixels in given circle, and then
    calculate its mean. The return value represents
    how many white pixels in the image, how bright it is.
    
    part of algorithm
    https://stackoverflow.com/a/74813748
    
    Parameters:
      - image_in: input image
      - center_x: circle center x
      - center_y: circle center y
      - radius: circle radius
    
    Returns:
      - float between 0 to 1
        )pbdoc"
    );
    // cut_ref_image_from_circle won't be supported because we cannot properly convert
    // cv::Mat referring another cv::Mat to numpy.ndarray
    module.def("cut_image_from_circle", &wrap_cut_image_from_circle,
        py::arg("image_in"),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius"),
        py::arg("padding")         = 15,
        R"pbdoc(
    Cut a square image using input circle and output a cv2.MatLike|numpy.ndarray copy of input
    
    Parameters:
      - image_in: input image
      - image_out: output image, copy of input image
      - rect_out: mr::Rectangle, top_left & bottom_right coordinate of output square in image_in
      - x: circle center x
      - y: circle center y
      - radius: circle radius
      - padding: padding pixels to the radius (default 15)
        )pbdoc"
    );
}
