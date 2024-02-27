#pragma once

#include <opencv2/core/mat.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "typecvt.hpp"
#include "MoonRegistration/imgprocess.hpp"


py::tuple wrap_resize_with_aspect_ratio(
    const cv::Mat& image_in,
    int width,
    int height,
    int longer_side_val,
    int inter
);

cv::Mat wrap_apply_brightness_contrast(
    const cv::Mat& image_in,
    int brightness,
    int contrast
);

float wrap_calc_img_brightness_perc(
    const cv::Mat& image_in
);

float wrap_calc_circle_brightness_perc(
    const cv::Mat& image_in,
    int center_x,
    int center_y,
    int radius
);

py::tuple wrap_cut_image_from_circle(
    const cv::Mat& image_in,
    int x,
    int y,
    int radius,
    int padding
);

cv::Mat wrap_sync_img_size(
    const cv::Mat& primary,
    cv::Mat& secondary
);

mr::ImageChannels& wrap_split_img_channel(
    const cv::Mat& image_in
);

cv::Mat wrap_merge_img_channel(
    const mr::ImageChannels& channels
);

cv::Mat wrap_stack_imgs(
    const cv::Mat& background,
    const py::object& background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency = 1.0f,
    const py::object& filter_px = py::none()
);

cv::Mat wrap_stack_imgs_in_place(
    cv::Mat& background,
    const py::object& background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency = 1.0f,
    const py::object& filter_px = py::none()
);


// initialize submodule
void init_imgprocess(py::module &module);
