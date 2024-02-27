#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;



// custom pybind11 caster for cv::Rect and tuple
namespace pybind11 { namespace detail {

template <> struct type_caster<cv::Rect> {
public:
    
    PYBIND11_TYPE_CASTER(cv::Rect, _("tuple"));
    
    // tuple --> cv::Rect
    bool load(handle src, bool /* convert */) {
        try
        {
            py::tuple src_tuple = py::cast<py::tuple>(src);
            if (py::len(src_tuple) != 4)
                return false;
            
            value = cv::Rect(
                py::cast<int>(src_tuple[0]),
                py::cast<int>(src_tuple[1]),
                py::cast<int>(src_tuple[2]),
                py::cast<int>(src_tuple[3])
            );
        }
        catch (std::exception& err)
        {
            throw err;
        }
        
        return true;
    }
    
    // cv::Rect --> tuple
    static handle cast(const cv::Rect &src, return_value_policy, handle defval) {
        py::tuple result_obj = py::make_tuple(
            src.x,
            src.y,
            src.width,
            src.height
        );
        
        handle result(result_obj);
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


