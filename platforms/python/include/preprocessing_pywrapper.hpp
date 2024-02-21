#pragma once

#include <opencv2/core/mat.hpp>

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "ndarray_converter.h"
#include "cvvec3f_vector_ndarray.hpp"


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


// initialize submodule
void init_preprocessing(py::module &module);
