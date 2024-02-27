#pragma once

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <string>


template<typename TYPE>
std::string str_StreamOptOverload(const TYPE& obj)
{
    std::stringstream ss;
    ss << obj;
    return ss.str();
}

// initialize submodule
void init_shapes(py::module &module);
