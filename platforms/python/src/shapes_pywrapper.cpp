#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "typecvt.hpp"
#include "shapes_pywrapper.hpp"

#include "MoonRegistration/shapes.hpp"


// initialize submodule
void init_shapes(py::module &module)
{
    // shapes.hpp
    py::class_<mr::Circle>(module, "Circle")
        .def(py::init<int,int,int>(),
            py::arg("x"),
            py::arg("y"),
            py::arg("radius")
        )
        .def_readwrite("x", &mr::Circle::x)
        .def_readwrite("y", &mr::Circle::y)
        .def_readwrite("radius", &mr::Circle::radius)
        .def("__str__", str_StreamOptOverload<mr::Circle>)
        .def("__repr__", str_StreamOptOverload<mr::Circle>)
    ;
    
    py::class_<mr::Square>(module, "Square")
        .def(py::init<int,int,int>(),
            py::arg("x"),
            py::arg("y"),
            py::arg("width")
        )
        .def_readwrite("x", &mr::Square::x)
        .def_readwrite("y", &mr::Square::y)
        .def_readwrite("width", &mr::Square::width)
        .def("__str__", str_StreamOptOverload<mr::Square>)
        .def("__repr__", str_StreamOptOverload<mr::Square>)
    ;
    
    py::class_<mr::Rectangle>(module, "Rectangle")
        .def(py::init<int,int,int,int>(),
            py::arg("top_left_x"),
            py::arg("top_left_y"),
            py::arg("bottom_right_x"),
            py::arg("bottom_right_y")
        )
        .def_readwrite("top_left_x", &mr::Rectangle::top_left_x)
        .def_readwrite("top_left_y", &mr::Rectangle::top_left_y)
        .def_readwrite("bottom_right_x", &mr::Rectangle::bottom_right_x)
        .def_readwrite("bottom_right_y", &mr::Rectangle::bottom_right_y)
        .def("__str__", str_StreamOptOverload<mr::Rectangle>)
        .def("__repr__", str_StreamOptOverload<mr::Rectangle>)
    ;
    
    module.def("circle_to_square",
        py::overload_cast<const mr::Circle&>(mr::circle_to_square),
        py::arg("circle")
    );
    module.def("circle_to_square",
        py::overload_cast<int, int, int>(mr::circle_to_square),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius")
    );
    module.def("circle_to_rectangle",
        py::overload_cast<const mr::Circle&>(mr::circle_to_rectangle),
        py::arg("circle")
    );
    module.def("circle_to_rectangle",
        py::overload_cast<int, int, int>(mr::circle_to_rectangle),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius")
    );
    module.def("is_valid_circle",
        py::overload_cast<const mr::Circle&>(mr::is_valid_circle),
        py::arg("circle")
    );
    module.def("is_valid_circle",
        py::overload_cast<int, int, int>(mr::is_valid_circle),
        py::arg("x"),
        py::arg("y"),
        py::arg("radius")
    );
    module.def("rectangle_to_roi", mr::rectangle_to_roi,
        py::arg("rectangle")
    );
    module.def("roi_to_rectangle", mr::roi_to_rectangle,
        py::arg("roi")
    );
}
