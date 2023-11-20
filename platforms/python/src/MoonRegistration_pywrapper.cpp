#define PYBIND11_DETAILED_ERROR_MESSAGES

#include "MoonRegistration/mrapi.hpp"

#include "../include/ndarray_converter.h"

#include "../include/MoonDetect_pywrapper.hpp"
#include "../include/preprocessing_pywrapper.hpp"

#include <string>

#include <pybind11/pybind11.h>

namespace py = pybind11;


template<typename TYPE>
std::string str_StreamOptOverload(const TYPE& obj)
{
    std::stringstream ss;
    ss << obj;
    return ss.str();
}


PYBIND11_MODULE(MoonRegistration_pywrapper, module)
{
    module.doc() = R"pbdoc(
        Python wrapper for MoonRegistration Library.
        MoonRegistration Is A Cross Platform C++ library for Moon Location Detection & Moon Image Registration.
        --------------------
        
        .. currentmodule:: MoonRegistration
        
        .. autosummary::
           :toctree: _generate
        
            version
            Circle
            Square
            Rectangle
            circle_to_square_s
            circle_to_square_p
            circle_to_rectangle_s
            circle_to_rectangle_p
            resize_with_aspect_ratio
            apply_brightness_contrast
            calc_img_brightness_perc
            calc_circle_brightness_perc
            cut_image_from_circle
            MoonDetector
    )pbdoc";
    
    NDArrayConverter::init_numpy();
    
    // version.hpp
    module.def("version", mr::version, R"pbdoc(MoonRegistration Library Version)pbdoc");
    
    // shapes.hpp
    py::class_<mr::Circle>(module, "Circle")
        .def_readwrite("x", &mr::Circle::x)
        .def_readwrite("y", &mr::Circle::y)
        .def_readwrite("radius", &mr::Circle::radius)
        .def("__str__", str_StreamOptOverload<mr::Circle>)
        .def("__repr__", str_StreamOptOverload<mr::Circle>)
    ;
    
    py::class_<mr::Square>(module, "Square")
        .def_readwrite("x", &mr::Square::x)
        .def_readwrite("y", &mr::Square::y)
        .def_readwrite("width", &mr::Square::width)
        .def("__str__", str_StreamOptOverload<mr::Square>)
        .def("__repr__", str_StreamOptOverload<mr::Square>)
    ;
    
    py::class_<mr::Rectangle>(module, "Rectangle")
        .def_readwrite("top_left_x", &mr::Rectangle::top_left_x)
        .def_readwrite("top_left_y", &mr::Rectangle::top_left_y)
        .def_readwrite("bottom_right_x", &mr::Rectangle::bottom_right_x)
        .def_readwrite("bottom_right_y", &mr::Rectangle::bottom_right_y)
        .def("__str__", str_StreamOptOverload<mr::Rectangle>)
        .def("__repr__", str_StreamOptOverload<mr::Rectangle>)
    ;
    
    module.def("circle_to_square_s", mr::circle_to_square_s);
    module.def("circle_to_square_p", mr::circle_to_square_p);
    module.def("circle_to_rectangle_s", mr::circle_to_rectangle_s);
    module.def("circle_to_rectangle_p", mr::circle_to_rectangle_p);
    
    
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
    module.def("cut_image_from_circle", &wrap_cut_image_from_circle,
        py::arg("image_in"),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius"),
        py::arg("padding")         = 15,
        R"pbdoc(
    Cut a square image using input circle
    
    Parameters:
      - image_in: input image
      - image_out: output image
      - rect_out: mr::Rectangle, top_left & bottom_right coordinate of output square in image_in
      - x: circle center x
      - y: circle center y
      - radius: circle radius
      - padding: padding pixels to the radius (default 15)
        )pbdoc"
    );
    
    
    // MoonDetect/detector.hpp
    
    // default stage functions
    
    // class MoonDetector
    py::class_<mr::MoonDetector>(module, "MoonDetector")
        .def(py::init<>())
        .def(py::init<cv::Mat>(), R"pbdoc(
    taking raw cv::Mat image as input
    this constructor will assume cv_image is a decoded pixel matrix ready to use
    it will set cv_image to original_image directly
    colors in cv_image MUST in BGR order
        )pbdoc")
        // handle bytes & str param
        .def(py::init(&new_MoonDetector), R"pbdoc(
    Handling str image_filepath or bytes image_binary.
    
    Parameters:
      - object: can be either str for filepath, or bytes for raw image bytes
        )pbdoc")
        .def("is_empty", &mr::MoonDetector::is_empty)
        .def("init_by_path", &mr::MoonDetector::init_by_path)
        .def("init_by_byte", wrap_MoonDetect_init_by_byte)
        .def("detect_moon", &mr::MoonDetector::detect_moon, R"pbdoc(
    trying to find a circle from input image
    thats most likely contains the moon.
    
    Returns:
      - if success, return mr::Circle of the circle found
      - if fail (input doesn't contain circle), return mr::Circle of {-1, -1, -1}
        )pbdoc")
        // add stage function pointers
    ;
}

