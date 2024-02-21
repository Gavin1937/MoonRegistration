#include "utils_pywrapper.hpp"

#include "MoonRegistration/utils.hpp"


// initialize submodule
void init_utils(py::module &module)
{
    // utils.hpp
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
}
