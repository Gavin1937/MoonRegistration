#pragma once

#include <pybind11/pybind11.h>
namespace py = pybind11;


// initialize submodule
void init_imgprocess(py::module &module);
