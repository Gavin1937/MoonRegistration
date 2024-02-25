#pragma once

#include <opencv2/core/matx.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <vector>

#include "typecvt/ndarray_converter.h"


// custom pybind11 caster for std::vector<cv::Mat> and list[numpy.ndarray]
namespace pybind11 { namespace detail {

template <> struct type_caster<std::vector<cv::Mat>> {
public:
    
    PYBIND11_TYPE_CASTER(std::vector<cv::Mat>, _("list"));
    
    // list[numpy.ndarray] --> std::vector<cv::Mat>
    bool load(handle src, bool /* convert */) {
        try
        {
            value = py::cast<std::vector<cv::Mat>>(src);
        }
        catch (std::exception& err)
        {
            throw err;
        }
        
        return true;
    }
    
    // std::vector<cv::Mat> --> list[numpy.ndarray]
    static handle cast(const std::vector<cv::Mat> &src, return_value_policy, handle defval) {
        py::list dst_list(src.size());
        
        // copy src to dst
        for (int i = 0; i < src.size(); ++i)
        {
            dst_list[i] = src[i];
        }
        
        handle result(dst_list);
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


