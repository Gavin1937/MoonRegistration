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
    NDArrayConverter::init_numpy();
    
    // version.hpp
    module.def("version", mr::version);
    
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
        py::arg("inter")           = static_cast<int>(cv::INTER_AREA)
    );
    module.def("apply_brightness_contrast", &wrap_apply_brightness_contrast,
        py::arg("image_in"),
        py::arg("brightness")      = 0,
        py::arg("contrast")        = 0
    );
    module.def("calc_img_brightness_perc", &wrap_calc_img_brightness_perc,
        py::arg("image_in")
    );
    module.def("calc_circle_brightness_perc", &wrap_calc_circle_brightness_perc,
        py::arg("image_in"),
        py::arg("center_x"),
        py::arg("center_y"),
        py::arg("radius")
    );
    module.def("cut_image_from_circle", &wrap_cut_image_from_circle,
        py::arg("image_in"),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius"),
        py::arg("padding")         = 15
    );
    
    
    // MoonDetect/detector.hpp
    
    // default stage functions
    
    // class MoonDetector
    py::class_<mr::MoonDetector>(module, "MoonDetector")
        .def(py::init<>())
        .def(py::init<cv::Mat>())
        .def(py::init(&new_MoonDetector)) // handle bytes & str param
        .def("is_empty", &mr::MoonDetector::is_empty)
        .def("init_by_path", &mr::MoonDetector::init_by_path)
        .def("init_by_byte", wrap_MoonDetect_init_by_byte)
        .def("detect_moon", &mr::MoonDetector::detect_moon)
        // add stage function pointers
    ;
}

