#pragma once

#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <string>

#include "ndarray_converter.h"
#include "cvvec3f_vector_ndarray.hpp"


template<typename TYPE>
std::string str_StreamOptOverload(const TYPE& obj)
{
    std::stringstream ss;
    ss << obj;
    return ss.str();
}

// initialize submodule
void init_shapes(py::module &module);
