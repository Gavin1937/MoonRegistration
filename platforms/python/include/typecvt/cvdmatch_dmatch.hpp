#pragma once

#include <opencv2/features2d.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <vector>
#include <iostream>


// custom pybind11 caster for cv::DMatch and cv2.DMatch
namespace pybind11 { namespace detail {

template <> struct type_caster<cv::DMatch> {
public:
    
    PYBIND11_TYPE_CASTER(cv::DMatch, _("cv2.DMatch"));
    
    // cv2.DMatch --> cv::DMatch
    bool load(handle src, bool /* convert */) {
        try
        {
            value = cv::DMatch(
                py::cast<int>(src.attr("queryIdx")),
                py::cast<int>(src.attr("trainIdx")),
                py::cast<int>(src.attr("imgIdx")),
                py::cast<float>(src.attr("distance"))
            );
        }
        catch (std::exception& err)
        {
            throw err;
        }
        
        return true;
    }
    
    // cv::DMatch --> cv2.DMatch
    static handle cast(const cv::DMatch &src, return_value_policy, handle defval) {
        auto cv2 = py::module::import("cv2");
        py::object result_obj = cv2.attr("DMatch")(
            src.queryIdx,
            src.trainIdx,
            src.imgIdx,
            src.distance
        );
        
        handle result(result_obj);
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


