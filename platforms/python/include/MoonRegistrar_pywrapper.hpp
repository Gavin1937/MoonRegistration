#pragma once

#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
namespace py = pybind11;

#include <functional>

#include "MoonRegistration/MoonRegistrar.hpp"

#include "typecvt.hpp"


std::unique_ptr<mr::MoonRegistrar> new_MoonRegistrar(
    const py::object& user_data,
    const py::object& model_data,
    const mr::RegistrationAlgorithms& algorithm
);

void wrap_MoonRegistrar_update_images(
    mr::MoonRegistrar& self,
    const py::object& user_data,
    const py::object& model_data
);

void wrap_MoonRegistrar_update_f2d_detector(mr::MoonRegistrar& self, const mr::RegistrationAlgorithms& algorithm);

cv::Mat wrap_MoonRegistrar_transform_image(mr::MoonRegistrar& self, const cv::Mat& image_in);

cv::Mat wrap_MoonRegistrar_transform_image_inverse(mr::MoonRegistrar& self, const cv::Mat& image_in);

cv::Mat wrap_MoonRegistrar_transform_user_image(mr::MoonRegistrar& self);

cv::Mat wrap_MoonRegistrar_transform_layer_image(mr::MoonRegistrar& self, const cv::Mat& layer_image_in);

cv::Mat wrap_MoonRegistrar_draw_matched_keypoints(mr::MoonRegistrar& self);

cv::Mat wrap_MoonRegistrar_draw_red_transformed_user_image(mr::MoonRegistrar& self, const cv::Mat& transformed_image_in = cv::Mat());

cv::Mat wrap_MoonRegistrar_draw_green_model_image(mr::MoonRegistrar& self);

cv::Mat wrap_MoonRegistrar_draw_stacked_red_green_image(mr::MoonRegistrar& self, const cv::Mat& transformed_image_in = cv::Mat());

cv::Mat wrap_MoonRegistrar_draw_layer_image(
    mr::MoonRegistrar& self,
    const cv::Mat& layer_image_in,
    const float layer_image_transparency = 1.0f,
    const py::object& filter_px = py::none()
);

void set_MoonRegistrar_is_good_match(
    mr::MoonRegistrar& self,
    std::function<bool(
        const cv::DMatch&,
        const cv::DMatch&,
        const float,
        const cv::KeyPoint&,
        const cv::KeyPoint&,
        const cv::Mat&,
        const cv::Mat&
    )> func
);


// initialize submodule
void init_MoonRegistrar(py::module &module);
