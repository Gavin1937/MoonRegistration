#include "../include/ndarray_converter.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <string>

#include "MoonRegistration/mrapi.hpp"

#include "../include/MoonDetect_pywrapper.hpp"
#include "../include/preprocessing_pywrapper.hpp"


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
            ImageShape
            calc_image_shape
            resize_with_aspect_ratio
            apply_brightness_contrast
            calc_img_brightness_perc
            calc_circle_brightness_perc
            cut_image_from_circle
            default_preprocess_steps
            default_param_init
            default_iteration_param_update
            default_iteration_circle_select
            default_coordinate_remap
            MoonDetector
    )pbdoc";
    
    NDArrayConverter::init_numpy();
    
    
    // version.hpp
    module.def("version", mr::version, R"pbdoc(MoonRegistration Library Version)pbdoc");
    
    
    // shapes.hpp
    py::class_<mr::Circle>(module, "Circle")
        .def(py::init<int,int,int>())
        .def_readwrite("x", &mr::Circle::x)
        .def_readwrite("y", &mr::Circle::y)
        .def_readwrite("radius", &mr::Circle::radius)
        .def("__str__", str_StreamOptOverload<mr::Circle>)
        .def("__repr__", str_StreamOptOverload<mr::Circle>)
    ;
    
    py::class_<mr::Square>(module, "Square")
        .def(py::init<int,int,int>())
        .def_readwrite("x", &mr::Square::x)
        .def_readwrite("y", &mr::Square::y)
        .def_readwrite("width", &mr::Square::width)
        .def("__str__", str_StreamOptOverload<mr::Square>)
        .def("__repr__", str_StreamOptOverload<mr::Square>)
    ;
    
    py::class_<mr::Rectangle>(module, "Rectangle")
        .def(py::init<int,int,int,int>())
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
    
    
    // utils.hpp
    py::class_<mr::ImageShape>(module, "ImageShape")
        .def_readwrite("height", &mr::ImageShape::height)
        .def_readwrite("width", &mr::ImageShape::width)
        .def_readwrite("longer_side", &mr::ImageShape::longer_side)
        .def_readwrite("shorter_side", &mr::ImageShape::shorter_side)
    ;
    module.def("calc_image_shape", mr::calc_image_shape);
    
    
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
    module.def("default_preprocess_steps", wrap_default_preprocess_steps, R"pbdoc(
    Default function for preprocess_steps stage
    
    Parameters:
      - image_in: cv2.MatLike|numpy.ndarray
    
    Returns:
      - tuple[image_out:cv2.MatLike|numpy.ndarray, resize_ratio_out:float]
    )pbdoc");
    module.def("default_param_init", wrap_default_param_init, R"pbdoc(
    Default function for param_init stage
    
    Parameters:
      - image_shape: mr.ImageShape
      - max_iteration: int
      - circle_threshold: int
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
    
    Returns:
      - tuple[
        max_iteration:int, circle_threshold:int, dp:float, minDist:float,
        minRadiusRate:float, minRadius:int, maxRadiusRate:float, maxRadius:int,
        param1:float, param2:float
      ]
    )pbdoc");
    module.def("default_iteration_param_update", wrap_default_iteration_param_update, R"pbdoc(
    Default function for iteration_param_update stage
    
    Parameters:
      - iteration: int
      - image_brightness_perc: float
      - image_shape: mr.ImageShape
      - max_iteration: int
      - circle_threshold: int
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
    
    Returns:
      - tuple[
        max_iteration:int, circle_threshold:int, dp:float, minDist:float,
        minRadiusRate:float, minRadius:int, maxRadiusRate:float, maxRadius:int,
        param1:float, param2:float
      ]
    )pbdoc");
    module.def("default_iteration_circle_select", wrap_default_iteration_circle_select, R"pbdoc(
    Default function for iteration_circle_select stage
    
    Parameters:
      - iteration: int
      - image_in: cv2.MatLike|numpy.ndarray
      - detected_circles: numpy.ndarray, return value of cv2.HoughCircles
    
    Returns:
      - mr.Circle
    )pbdoc");
    module.def("default_coordinate_remap", wrap_default_coordinate_remap, R"pbdoc(
    Default function for coordinate_remap stage
    
    Parameters:
      - result_list: list[tuple(int, mr.Circle, mr.Rectangle)]
      - resize_ratio: float
    
    Returns:
      - mr.Circle
    )pbdoc");
    
    // class MoonDetector
    py::class_<mr::MoonDetector>(module, "MoonDetector")
        .def(py::init<>())
        // handle bytes & str param
        .def(py::init(&new_MoonDetector), R"pbdoc(
    Handling str image_filepath or bytes image_binary.
    
    Parameters:
      - parameter can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
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
        // set stage function pointers
        .def("set_preprocess_steps", set_MoonDetect_preprocess_steps, R"pbdoc(
    Set mr::MoonDetector::preprocess_steps
    
    Parameters:
      - func: callable function (cv2.MatLike|numpy.ndarray) -> tuple
        )pbdoc")
        .def("set_param_init", set_MoonDetect_param_init, R"pbdoc(
    Set mr::MoonDetector::param_init
    
    Parameters:
      - func: callable function (mr.ImageShape, int, int, float, float, float, int, float, int, float, float) -> tuple
        )pbdoc")
        .def("set_iteration_param_update", set_MoonDetect_iteration_param_update, R"pbdoc(
    Set mr::MoonDetector::iteration_param_update
    
    Parameters:
      - func: callable function (int, float, mr.ImageShape, int, int, float, float, float, int, float, int, float, float) -> tuple
        )pbdoc")
        .def("set_iteration_circle_select", set_MoonDetect_iteration_circle_select, R"pbdoc(
    Set mr::MoonDetector::iteration_circle_select
    
    Parameters:
      - func: callable function (int, cv2.MatLike|numpy.ndarray, numpy.ndarray) -> mr.Circle
        )pbdoc")
        .def("set_coordinate_remap", set_MoonDetect_coordinate_remap, R"pbdoc(
    Set mr::MoonDetector::coordinate_remap
    
    Parameters:
      - func: callable function (list, float) -> mr.Circle
        )pbdoc")
    ;
}

