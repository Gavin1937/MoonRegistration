#include "imgprocess_pywrapper.hpp"


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

cv::Mat wrap_sync_img_size(
    const cv::Mat& primary,
    cv::Mat& secondary
)
{
    mr::sync_img_size(
        primary, secondary
    );
    
    return secondary;
}

mr::ImageChannels& wrap_split_img_channel(
    const cv::Mat& image_in
)
{
    mr::ImageChannels* channels = new mr::ImageChannels;
    mr::split_img_channel(
        image_in, *channels
    );
    
    return *channels;
}

cv::Mat wrap_merge_img_channel(
    const mr::ImageChannels& channels
)
{
    cv::Mat image_out;
    
    mr::merge_img_channel(
        channels, image_out
    );
    
    return image_out;
}

cv::Mat wrap_stack_imgs(
    const cv::Mat& background,
    const py::object& background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency,
    const py::object& filter_px
)
{
    if (!py::isinstance<py::iterable>(background_roi))
        throw std::runtime_error("Input background_roi must be an Iterable");
    if (py::len(background_roi) != 4)
        throw std::runtime_error("Input background_roi must be length of 4");
    
    int idx = 0;
    int buffer[4] = {0,0,0,0};
    
    for (auto it : background_roi)
    {
        buffer[idx] = py::cast<int>(*it);
        idx++;
    }
    cv::Rect rect_background_roi(
        buffer[0],
        buffer[1],
        buffer[2],
        buffer[3]
    );
    
    cv::Vec4b* vec4b_filter_px = NULL;
    if (py::isinstance<py::iterable>(filter_px))
    {
        if (py::len(filter_px) != 4)
            throw std::runtime_error("Input background_roi must be length of 4");
        
        idx = 0;
        for (auto it : filter_px)
        {
            buffer[idx] = py::cast<int>(*it);
            idx++;
        }
        vec4b_filter_px = new cv::Vec4b(
            buffer[0],
            buffer[1],
            buffer[2],
            buffer[3]
        );
    }
    
    cv::Mat image_out;
    mr::stack_imgs(
        background,
        rect_background_roi,
        foreground,
        image_out,
        foreground_transparency,
        vec4b_filter_px
    );
    
    delete vec4b_filter_px;
    return image_out;
}

cv::Mat wrap_stack_imgs_in_place(
    cv::Mat& background,
    const py::object& background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency,
    const py::object& filter_px
)
{
    if (!py::isinstance<py::iterable>(background_roi))
        throw std::runtime_error("Input background_roi must be an Iterable");
    if (py::len(background_roi) != 4)
        throw std::runtime_error("Input background_roi must be length of 4");
    
    int idx = 0;
    int buffer[4] = {0,0,0,0};
    
    for (auto it : background_roi)
    {
        buffer[idx] = py::cast<int>(*it);
        idx++;
    }
    cv::Rect rect_background_roi(
        buffer[0],
        buffer[1],
        buffer[2],
        buffer[3]
    );
    
    cv::Vec4b* vec4b_filter_px = NULL;
    if (py::isinstance<py::iterable>(filter_px))
    {
        if (py::len(filter_px) != 4)
            throw std::runtime_error("Input background_roi must be length of 4");
        
        idx = 0;
        for (auto it : filter_px)
        {
            buffer[idx] = py::cast<int>(*it);
            idx++;
        }
        vec4b_filter_px = new cv::Vec4b(
            buffer[0],
            buffer[1],
            buffer[2],
            buffer[3]
        );
    }
    
    mr::stack_imgs_in_place(
        background,
        rect_background_roi,
        foreground,
        foreground_transparency,
        vec4b_filter_px
    );
    
    delete vec4b_filter_px;
    return background;
}


// initialize submodule
void init_imgprocess(py::module &module)
{
    // imgprocess.hpp
    py::class_<mr::ImageShape>(module, "ImageShape")
        .def(py::init<int,int,int,int>(),
            py::arg("height"),
            py::arg("width"),
            py::arg("longer_side"),
            py::arg("shorter_side")
        )
        .def_readwrite("height", &mr::ImageShape::height)
        .def_readwrite("width", &mr::ImageShape::width)
        .def_readwrite("longer_side", &mr::ImageShape::longer_side)
        .def_readwrite("shorter_side", &mr::ImageShape::shorter_side)
    ;
    module.def("calc_image_shape", mr::calc_image_shape,
        py::arg("image_in")
    );
    
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
      - width: resize image to this width (-1 to ignore)
      - height: resize image to this height (-1 to ignore)
      - longer_side_val: resize image's longer side to this value (-1 to ignore)
      - inter: interpolation for cv::resize()
    
    Returns (tuple):
      - image_out: output image
      - ratio_out: output resize ratio
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
      - brightness: set image brightness value (-127 to 127) (default 0)
      - contrast: set image contrast value (-127 to 127) (default 0)
    
    Returns:
      - image_out: output image
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
      - x: circle center x
      - y: circle center y
      - radius: circle radius
      - padding: padding pixels to the radius (default 15)
    
    Returns (tuple):
      - image_out: output image, copy of input image
      - rect_out: mr::Rectangle, top_left & bottom_right coordinate of output square in image_in
        )pbdoc"
    );
    module.def("sync_img_size", &wrap_sync_img_size,
        py::arg("primary"),
        py::arg("secondary"),
        R"pbdoc(
    Sync the width & height of secondary image to primary image
    
    Parameters:
      - primary: image for sync reference
      - secondary: image to sync with primary
    
    Returns:
      - primary image after modification
        )pbdoc"
    );
    
    py::class_<mr::ImageChannels>(module, "ImageChannels")
        .def(py::init<>())
        .def(py::init<const cv::Mat&>())
        .def(py::init<const cv::Mat&,const cv::Mat&,const cv::Mat&>())
        .def(py::init<const cv::Mat&,const cv::Mat&,const cv::Mat&,const cv::Mat&>())
        .def_readwrite("channels", &mr::ImageChannels::channels)
        .def("blue", &mr::ImageChannels::blue)
        .def("green", &mr::ImageChannels::green)
        .def("red", &mr::ImageChannels::red)
        .def("alpha", &mr::ImageChannels::alpha)
        .def("channel_num", &mr::ImageChannels::channel_num)
    ;
    module.def("split_img_channel", &wrap_split_img_channel,
        py::arg("image_in"),
        R"pbdoc(
    Split image into ImageChannels
    
    Parameters:
      - image_in: input image
    
    Returns:
      - mr::ImageChannels object
        )pbdoc"
    );
    module.def("merge_img_channel", &wrap_merge_img_channel,
        py::arg("channels"),
        R"pbdoc(
    Merge ImageChannels into image
    
    Parameters:
      - channels: input mr::ImageChannels
    
    Returns:
      - merged image
        )pbdoc"
    );
    
    module.def("stack_imgs", &wrap_stack_imgs,
        py::arg("background"),
        py::arg("background_roi"),
        py::arg("foreground"),
        py::arg("foreground_transparency") = 1.0f,
        py::arg("filter_px") = py::none(),
        R"pbdoc(
    Stack two images together respecting transparency
    
    Parameters:
      - background: background image
      - background_roi: Region of Interest (ROI) of background image, list[int] length of 4
      - foreground: foreground image
      - foreground_transparency: a 0~1 float percentage changing foreground image's transparency,
        default 1.0
      - filter_px: list[int] length of 4, pixel with pixel value to filter in the foreground image,
        set it to None if you don't need it, default None
    
    Returns:
      - output image
    
    Note:
      - this function is designed to work with different number of color channels
      - if foreground size is bigger then background size, this function will call mr::sync_img_size()
        to sync it with background
      - image_out will follow the maximum number of channels between two input images,
        thus if one input is grayscale image and another is not, then it will become
        the blue channel of image_out, as OpenCV uses BGRA pixel order.
      - when using filter_px, we only read number of elements corresponding to
        number of channels in foreground. if foreground image has 3 channels, we only read
        first 3 elements of filter_px. rest of elements will be ignored.
        )pbdoc"
    );
    module.def("stack_imgs_in_place", &wrap_stack_imgs_in_place,
        py::arg("background"),
        py::arg("background_roi"),
        py::arg("foreground"),
        py::arg("foreground_transparency") = 1.0f,
        py::arg("filter_px") = py::none(),
        R"pbdoc(
    Stack two images together respecting transparency,
    and write pixel value in-place to the background.
    This function is really similar to mr::stack_imgs(),
    we just write pixel value directly to background image to avoid copy.
    
    Parameters:
      - background: background image
      - background_roi: Region of Interest (ROI) of background image, list[int] length of 4
      - foreground: foreground image
      - foreground_transparency: a 0~1 float percentage changing foreground image's transparency,
        default 1.0
      - filter_px: list[int] length of 4, pixel with pixel value to filter in the foreground image,
        set it to None if you don't need it, default None
    
    Returns:
      - output image
    
    Note:
      - this function is designed to work with different number of color channels
      - if foreground size is bigger then background size, this function will call mr::sync_img_size()
        to sync it with background
      - if foreground number of channel is greater then background, this function will sync
        its number of channel with background
      - when using filter_px, we only read number of elements corresponding to
        number of channels in foreground. if foreground image has 3 channels, we only read
        first 3 elements of filter_px. rest of elements will be ignored.
        )pbdoc"
    );
}
