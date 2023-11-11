#pragma once

#include <string>
#include <vector>

#include "MoonRegistration/mrapi.hpp"

#include <boost/python.hpp>

namespace py = boost::python;

boost::shared_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data);

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, py::object& data);

