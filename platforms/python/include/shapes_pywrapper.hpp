#pragma once

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <string>


// initialize submodule
void init_shapes(py::module &module);
