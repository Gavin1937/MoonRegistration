#include "MoonRegistration/mrapi.hpp"

#include "../include/MoonDetect_pywrapper.hpp"

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
    
    
    // MoonDetect/detector.hpp
    
    // default stage functions
    
    // class MoonDetector
    py::class_<mr::MoonDetector>(module, "MoonDetector")
        .def(py::init<>())
        .def(py::init(&new_MoonDetector))
        .def("is_empty", &mr::MoonDetector::is_empty)
        .def("init_by_path", &mr::MoonDetector::init_by_path)
        .def("init_by_byte", wrap_MoonDetect_init_by_byte)
        .def("detect_moon", &mr::MoonDetector::detect_moon)
        // add stage function pointers
    ;
}

