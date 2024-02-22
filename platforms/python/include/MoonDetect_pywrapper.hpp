#pragma once

#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "MoonRegistration/MoonDetect.hpp"

#include "ndarray_converter.h"
#include "cvvec3f_vector_ndarray.hpp"


std::vector<cv::Vec3f> wrap_find_circles_in_img(
    const cv::Mat& image_in,
    const int circle_threshold,
    const double dp,
    const double minDist,
    const int minRadius,
    const int maxRadius,
    const double param1,
    const double param2,
    const mr::HoughCirclesAlgorithm& algorithm
);

// default stage functions

py::tuple wrap_default_preprocess_steps(
    const cv::Mat& image_in
);

py::tuple wrap_default_param_init(
    const mr::ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

py::tuple wrap_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const mr::ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2
);

mr::Circle wrap_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
);

mr::Circle wrap_default_coordinate_remap(
    const py::list& result_list,
    const float resize_ratio
);


// class MoonDetector

std::unique_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data);

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, const py::bytes& data);

void set_MoonDetect_preprocess_steps(mr::MoonDetector& self, std::function<py::tuple(const cv::Mat&)> func);

void set_MoonDetect_param_init(
    mr::MoonDetector& self,
    std::function<py::tuple(const mr::ImageShape&, int, int, double, double, double, int, double, int, double, double)> func
);

void set_MoonDetect_iteration_param_update(
    mr::MoonDetector& self,
    std::function<py::tuple(const int, const float, const mr::ImageShape&, int, int, double, double, double, int, double, int, double, double)> func
);

void set_MoonDetect_iteration_circle_select(
    mr::MoonDetector& self,
    std::function<mr::Circle(const int, const cv::Mat&, const std::vector<cv::Vec3f>&)> func
);

void set_MoonDetect_coordinate_remap(
    mr::MoonDetector& self,
    std::function<mr::Circle(const py::list&, const float)> func
);


// initialize submodule
void init_MoonDetect(py::module &module);
