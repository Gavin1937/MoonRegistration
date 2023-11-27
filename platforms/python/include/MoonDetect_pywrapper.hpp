#pragma once

#include "ndarray_converter.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "MoonRegistration/mrapi.hpp"


// add custom caster for std::vector<cv::Vec3f> and numpy.ndarray
// this caster is specific to handle return value of cv::HoughCircles
namespace pybind11 { namespace detail {

template <> struct type_caster<std::vector<cv::Vec3f>> {
public:
    
    PYBIND11_TYPE_CASTER(std::vector<cv::Vec3f>, _("numpy.ndarray"));
    
    bool load(handle src, bool /* convert */) {
        py::array_t<float> src_array = py::cast<py::array_t<float>>(src);
        py::buffer_info buff = src_array.request();
        
        // we are expecting 3d array of shape (1, x, 3)
        // where x is the size of output vector
        if (buff.ndim != 3)
            return false;
        
        int flat_array_size = static_cast<int>(buff.size);
        float* flat_array = static_cast<float*>(buff.ptr);
        
        // copy src to output value
        value = std::vector<cv::Vec3f>(flat_array_size/3);
        for (int counter = 0, i = 0; i < flat_array_size; i+=3, counter++)
        {
            value[counter] = cv::Vec3f(
                flat_array[i+0],
                flat_array[i+1],
                flat_array[i+2]
            );
        }
        
        return true;
    }
    
    static handle cast(const std::vector<cv::Vec3f> &src, return_value_policy, handle defval) {
        py::array_t<float> dst_array(src.size()*3);
        py::buffer_info dst_buff_info = dst_array.request();
        float* dst_buff_ptr = static_cast<float*>(dst_buff_info.ptr);
        
        // copy src to dst
        int idx = 0;
        for (auto src_it : src)
        {
            dst_buff_ptr[idx++] = src_it[0];
            dst_buff_ptr[idx++] = src_it[1];
            dst_buff_ptr[idx++] = src_it[2];
        }
        // same as numpy.ndarray.resize, update dst_array's shape
        dst_array.resize(py::array::ShapeContainer({1,static_cast<int>(src.size()),3}));
        
        handle result = handle(dst_array);
        result.inc_ref(); // increase reference count so dst_array is visible by python
        return result;
    }
};

}} // namespace pybind11::detail


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
