#pragma once

#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "ndarray_converter.h"
#include "cvvec3f_vector_ndarray.hpp"


// initialize submodule
void init_utils(py::module &module);
