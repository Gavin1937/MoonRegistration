#pragma once

#include <opencv2/core/types.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;



// custom pybind11 caster for cv::Size and Iterable
namespace pybind11 { namespace detail {

template <> struct type_caster<cv::Size> {
public:
    
    PYBIND11_TYPE_CASTER(cv::Size, _("Iterable"));
    
    // Iterable --> cv::Size
    bool load(handle src, bool /* convert */) {
        try
        {
            py::tuple src_iter = py::cast<py::tuple>(src);
            if (py::len(src_iter) != 2)
                return false;
            
            value = cv::Size(
                py::cast<int>(src_iter[0]),
                py::cast<int>(src_iter[1])
            );
        }
        catch (std::exception& err)
        {
            throw err;
        }
        
        return true;
    }
    
    // cv::Size --> Iterable
    static handle cast(const cv::Size &src, return_value_policy, handle defval) {
        py::tuple result_obj = py::make_tuple(
            src.width,
            src.height
        );
        
        handle result(py::cast<py::iterable>(result_obj));
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


