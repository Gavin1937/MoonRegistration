#include "MoonRegistration/mrapi.hpp"

#include "../include/MoonDetect_pywrapper.hpp"

#include <boost/python.hpp>

namespace py = boost::python;


BOOST_PYTHON_MODULE(MoonRegistration_pywrapper)
{
    py::class_<mr::Circle>("Circle")
        .def_readwrite("x", &mr::Circle::x)
        .def_readwrite("y", &mr::Circle::y)
        .def_readwrite("radius", &mr::Circle::radius)
    ;
    
    py::class_<mr::MoonDetector>("MoonDetector")
        .def("__init__", py::make_constructor(new_MoonDetector))
        .def("detect_moon", &mr::MoonDetector::detect_moon)
    ;
}

