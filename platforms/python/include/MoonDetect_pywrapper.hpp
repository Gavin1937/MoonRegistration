#pragma once

#include <string>
#include <vector>

#include "MoonRegistration/mrapi.hpp"

#include <boost/python.hpp>

namespace py = boost::python;

boost::shared_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data);
