#pragma once

#include <opencv2/features2d.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include <vector>


// custom pybind11 caster for cv::KeyPoint and cv2.KeyPoint
namespace pybind11 { namespace detail {

template <> struct type_caster<cv::KeyPoint> {
public:
    
    PYBIND11_TYPE_CASTER(cv::KeyPoint, _("cv2.KeyPoint"));
    
    // cv2.KeyPoint --> cv::KeyPoint
    bool load(handle src, bool /* convert */) {
        try
        {
            py::tuple pt = py::cast<py::tuple>(src.attr("pt"));
            value = cv::KeyPoint(
                py::cast<float>(pt[0]),
                py::cast<float>(pt[1]),
                py::cast<float>(src.attr("size")),
                py::cast<float>(src.attr("angle")),
                py::cast<float>(src.attr("response")),
                py::cast<int>(src.attr("octave")),
                py::cast<int>(src.attr("class_id"))
            );
        }
        catch (std::exception& err)
        {
            throw err;
        }
        
        return true;
    }
    
    // cv::KeyPoint --> cv2.KeyPoint
    static handle cast(const cv::KeyPoint &src, return_value_policy, handle defval) {
        auto cv2 = py::module::import("cv2");
        py::object result_obj = cv2.attr("KeyPoint")(
            src.pt.x,
            src.pt.y,
            src.size,
            src.angle,
            src.response,
            src.octave,
            src.class_id
        );
        
        handle result(result_obj);
        result.inc_ref(); // increase reference count so dst_array is visible to python
        return result;
    }
};

}} // namespace pybind11::detail


