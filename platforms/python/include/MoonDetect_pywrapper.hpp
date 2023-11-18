#pragma once

#include <string>
#include <vector>
#include <memory>

#include "MoonRegistration/mrapi.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;

std::unique_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data);

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, const py::bytes& data);

