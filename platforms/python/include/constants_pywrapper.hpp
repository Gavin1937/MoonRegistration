#pragma once

#include <pybind11/pybind11.h>
namespace py = pybind11;


// initialize function
void init_constants(py::module &module);
