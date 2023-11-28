#pragma once

#include <opencv2/core/matx.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <vector>


// custom pybind11 caster for std::vector<cv::Vec3f> and numpy.ndarray
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
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


