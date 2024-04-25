#include <opencv2/core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "typecvt.hpp"
#include "MoonDetect_pywrapper.hpp"

#include "MoonRegistration/MoonDetect.hpp"


std::vector<cv::Vec3f> wrap_find_circles_in_img(
    const cv::Mat& image_in,
    const int circle_threshold,
    const double dp,
    const double minDist,
    const int minRadius,
    const int maxRadius,
    const double param1,
    const double param2,
    const int algorithm
)
{
    std::vector<cv::Vec3f> output;
    mr::find_circles_in_img(
        image_in, output, circle_threshold,
        dp, minDist,
        minRadius, maxRadius,
        param1, param2,
        algorithm
    );
    return output;
}


// default stage functions
// for class MoonDetector differ by the
// HoughCirclesAlgorithms they use.
// Algorithm name to function prefix translation:
// HOUGH_GRADIENT        => HG
// HOUGH_GRADIENT_ALT    => HGA
// HOUGH_GRADIENT_MIX    => HGM

// HOUGH_GRADIENT (HG)
py::tuple wrap_HG_default_preprocess_steps(
    const cv::Mat& image_in
)
{
    cv::Mat image_out;
    float resize_ratio_out = 0.0;
    mr::HG_default_preprocess_steps(image_in, image_out, resize_ratio_out);
    return py::make_tuple(image_out, py::float_(resize_ratio_out));
}

py::tuple wrap_HG_default_param_init(
    const mr::ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HG_default_param_init(
        image_shape, max_iteration, circle_threshold, hough_circles_algorithm,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(max_iteration), py::int_(circle_threshold),
        py::int_(hough_circles_algorithm),
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

py::tuple wrap_HG_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const cv::Size& initial_image_size,
    const mr::ImageShape& image_shape,
    const mr::Circle& curr_circle_found,
    const int max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    cv::Mat& process_image,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HG_default_iteration_param_update(
        iteration, image_brightness_perc, initial_image_size,
        image_shape, curr_circle_found, max_iteration, circle_threshold,
        hough_circles_algorithm, process_image, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(circle_threshold), py::int_(hough_circles_algorithm),
        process_image,
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

mr::Circle wrap_HG_default_iteration_circle_select(
    const int iteration,
    const int max_iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::HG_default_iteration_circle_select(iteration, max_iteration, image_in, detected_circles);
}

mr::Circle wrap_HG_default_coordinate_remap(
    const py::list& result_list,
    const float resize_ratio
)
{
    int length = static_cast<int>(py::len(result_list));
    std::vector<std::tuple<int, mr::Circle, mr::Rectangle>> cpp_result_list(length);
    for (int i = 0; i < length; ++i)
    {
        auto tmp = py::cast<py::tuple>(result_list[i]);
        cpp_result_list[i] = std::tuple<int, mr::Circle, mr::Rectangle>(
            py::cast<int>(tmp[0]),
            py::cast<mr::Circle>(tmp[1]),
            py::cast<mr::Rectangle>(tmp[2])
        );
    }
    return mr::HG_default_coordinate_remap(cpp_result_list, resize_ratio);
}

// HOUGH_GRADIENT_ALT (HGA)
py::tuple wrap_HGA_default_preprocess_steps(
    const cv::Mat& image_in
)
{
    cv::Mat image_out;
    float resize_ratio_out = 0.0;
    mr::HGA_default_preprocess_steps(image_in, image_out, resize_ratio_out);
    return py::make_tuple(image_out, py::float_(resize_ratio_out));
}

py::tuple wrap_HGA_default_param_init(
    const mr::ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HGA_default_param_init(
        image_shape, max_iteration, circle_threshold, hough_circles_algorithm,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(max_iteration), py::int_(circle_threshold),
        py::int_(hough_circles_algorithm),
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

py::tuple wrap_HGA_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const cv::Size& initial_image_size,
    const mr::ImageShape& image_shape,
    const mr::Circle& curr_circle_found,
    const int max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    cv::Mat& process_image,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HGA_default_iteration_param_update(
        iteration, image_brightness_perc, initial_image_size,
        image_shape, curr_circle_found, max_iteration, circle_threshold,
        hough_circles_algorithm, process_image, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(circle_threshold), py::int_(hough_circles_algorithm),
        process_image,
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

mr::Circle wrap_HGA_default_iteration_circle_select(
    const int iteration,
    const int max_iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::HGA_default_iteration_circle_select(iteration, max_iteration, image_in, detected_circles);
}

mr::Circle wrap_HGA_default_coordinate_remap(
    const py::list& result_list,
    const float resize_ratio
)
{
    int length = static_cast<int>(py::len(result_list));
    std::vector<std::tuple<int, mr::Circle, mr::Rectangle>> cpp_result_list(length);
    for (int i = 0; i < length; ++i)
    {
        auto tmp = py::cast<py::tuple>(result_list[i]);
        cpp_result_list[i] = std::tuple<int, mr::Circle, mr::Rectangle>(
            py::cast<int>(tmp[0]),
            py::cast<mr::Circle>(tmp[1]),
            py::cast<mr::Rectangle>(tmp[2])
        );
    }
    return mr::HGA_default_coordinate_remap(cpp_result_list, resize_ratio);
}

// HOUGH_GRADIENT_MIX (HGM)
py::tuple wrap_HGM_default_preprocess_steps(
    const cv::Mat& image_in
)
{
    cv::Mat image_out;
    float resize_ratio_out = 0.0;
    mr::HGM_default_preprocess_steps(image_in, image_out, resize_ratio_out);
    return py::make_tuple(image_out, py::float_(resize_ratio_out));
}

py::tuple wrap_HGM_default_param_init(
    const mr::ImageShape& image_shape,
    int& max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HGM_default_param_init(
        image_shape, max_iteration, circle_threshold, hough_circles_algorithm,
        dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(max_iteration), py::int_(circle_threshold),
        py::int_(hough_circles_algorithm),
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

py::tuple wrap_HGM_default_iteration_param_update(
    const int iteration,
    const float image_brightness_perc,
    const cv::Size& initial_image_size,
    const mr::ImageShape& image_shape,
    const mr::Circle& curr_circle_found,
    const int max_iteration,
    int& circle_threshold,
    int& hough_circles_algorithm,
    cv::Mat& process_image,
    double& dp,
    double& minDist,
    double& minRadiusRate,
    int& minRadius,
    double& maxRadiusRate,
    int& maxRadius,
    double& param1,
    double& param2,
    int& cut_circle_padding
)
{
    mr::HGM_default_iteration_param_update(
        iteration, image_brightness_perc, initial_image_size,
        image_shape, curr_circle_found, max_iteration, circle_threshold,
        hough_circles_algorithm, process_image, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2, cut_circle_padding
    );
    return py::make_tuple(
        py::int_(circle_threshold), py::int_(hough_circles_algorithm),
        process_image,
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2),
        py::int_(cut_circle_padding)
    );
}

mr::Circle wrap_HGM_default_iteration_circle_select(
    const int iteration,
    const int max_iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::HGM_default_iteration_circle_select(iteration, max_iteration, image_in, detected_circles);
}

mr::Circle wrap_HGM_default_coordinate_remap(
    const py::list& result_list,
    const float resize_ratio
)
{
    int length = static_cast<int>(py::len(result_list));
    std::vector<std::tuple<int, mr::Circle, mr::Rectangle>> cpp_result_list(length);
    for (int i = 0; i < length; ++i)
    {
        auto tmp = py::cast<py::tuple>(result_list[i]);
        cpp_result_list[i] = std::tuple<int, mr::Circle, mr::Rectangle>(
            py::cast<int>(tmp[0]),
            py::cast<mr::Circle>(tmp[1]),
            py::cast<mr::Rectangle>(tmp[2])
        );
    }
    return mr::HGM_default_coordinate_remap(cpp_result_list, resize_ratio);
}


// class MoonDetector

std::unique_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data)
{
    // https://stackoverflow.com/a/8142943
    // https://stackoverflow.com/a/62310838
    
    // input bytes
    if (py::isinstance<py::bytes>(data))
    {
        std::string val = py::cast<std::string>(data);
        return std::unique_ptr<mr::MoonDetector>(
            new mr::MoonDetector(
                std::vector<unsigned char>(val.begin(), val.end())
            )
        );
    }
    // input str
    else if (py::isinstance<py::str>(data))
    {
        std::string val = py::cast<std::string>(data);
        return std::unique_ptr<mr::MoonDetector>(
            new mr::MoonDetector(val)
        );
    }
    // if data is not None, assume data is a numpy.ndarray (cv2.MatLike)
    else if (!py::isinstance<py::none>(data) && py::isinstance<py::array>(data))
    {
        cv::Mat image = py::cast<cv::Mat>(data);
        return std::unique_ptr<mr::MoonDetector>(
            new mr::MoonDetector(image)
        );
    }
    
    throw std::runtime_error("Cannot create mr::MoonDetector with given parameter");
}

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, const py::bytes& data)
{
    std::string val = py::cast<std::string>(data);
    self.init_by_byte(std::vector<unsigned char>(val.begin(), val.end()));
}

void set_MoonDetect_preprocess_steps(mr::MoonDetector& self, std::function<py::tuple(const cv::Mat&)> func)
{
    self.preprocess_steps = [func](const cv::Mat& image_in, cv::Mat& image_out, float& resize_ratio_out){
        py::tuple ret = func(image_in);
        image_out = ret[0].cast<cv::Mat>();
        resize_ratio_out = ret[1].cast<float>();
    };
}

void set_MoonDetect_param_init(
    mr::MoonDetector& self,
    std::function<py::tuple(const mr::ImageShape&, int, int, int, double, double, double, int, double, int, double, double, int)> func
)
{
    self.param_init = [func](
        const mr::ImageShape& image_shape,
        int& max_iteration, int& circle_threshold,
        int& hough_circles_algorithm,
        double& dp, double& minDist,
        double& minRadiusRate, int& minRadius,
        double& maxRadiusRate, int& maxRadius,
        double& param1, double& param2,
        int& cut_circle_padding
    ){
        py::tuple ret = func(
            image_shape, max_iteration, circle_threshold,
            hough_circles_algorithm,
            dp, minDist,
            minRadiusRate, minRadius, maxRadiusRate, maxRadius,
            param1, param2, cut_circle_padding
        );
        max_iteration = ret[0].cast<int>();
        circle_threshold = ret[1].cast<int>();
        hough_circles_algorithm = ret[2].cast<int>();
        dp = ret[3].cast<double>();
        minDist = ret[4].cast<double>();
        minRadiusRate = ret[5].cast<double>();
        minRadius = ret[6].cast<int>();
        maxRadiusRate = ret[7].cast<double>();
        maxRadius = ret[8].cast<int>();
        param1 = ret[9].cast<double>();
        param2 = ret[10].cast<double>();
        cut_circle_padding = ret[11].cast<int>();
    };
}

void set_MoonDetect_iteration_param_update(
    mr::MoonDetector& self,
    std::function<py::tuple(const int, const float, const cv::Size&, const mr::ImageShape&, const mr::Circle&, const int, int, int, cv::Mat, double, double, double, int, double, int, double, double, int)> func
)
{
    self.iteration_param_update = [func](
        const int iteration, const float image_brightness_perc,
        const cv::Size& initial_image_size,
        const mr::ImageShape& image_shape,
        const mr::Circle& curr_circle_found,
        const int max_iteration, int& circle_threshold,
        int& hough_circles_algorithm,
        cv::Mat& process_image,
        double& dp, double& minDist,
        double& minRadiusRate, int& minRadius,
        double& maxRadiusRate, int& maxRadius,
        double& param1, double& param2,
        int& cut_circle_padding
    ){
        py::tuple ret = func(
            iteration, image_brightness_perc,
            initial_image_size, image_shape, curr_circle_found,
            max_iteration, circle_threshold, hough_circles_algorithm, process_image,
            dp, minDist, minRadiusRate, minRadius, maxRadiusRate, maxRadius,
            param1, param2, cut_circle_padding
        );
        circle_threshold = ret[0].cast<int>();
        hough_circles_algorithm = ret[1].cast<int>();
        process_image = ret[2].cast<cv::Mat>();
        dp = ret[3].cast<double>();
        minDist = ret[4].cast<double>();
        minRadiusRate = ret[5].cast<double>();
        minRadius = ret[6].cast<int>();
        maxRadiusRate = ret[7].cast<double>();
        maxRadius = ret[8].cast<int>();
        param1 = ret[9].cast<double>();
        param2 = ret[10].cast<double>();
        cut_circle_padding = ret[11].cast<int>();
    };
}

void set_MoonDetect_iteration_circle_select(
    mr::MoonDetector& self,
    std::function<mr::Circle(const int, const int, const cv::Mat&, const std::vector<cv::Vec3f>&)> func
)
{
    self.iteration_circle_select = func;
}

void set_MoonDetect_coordinate_remap(
    mr::MoonDetector& self,
    std::function<mr::Circle(const py::list&, const float)> func
)
{
    self.coordinate_remap = [func](const std::vector<std::tuple<int, mr::Circle, mr::Rectangle>>& result_list, const float resize_ratio){
        int length = static_cast<int>(result_list.size());
        py::list py_result_list(length);
        for (int i = 0; i < length; ++i)
        {
            py_result_list[i] = py::make_tuple(
                std::get<0>(result_list[i]),
                std::get<1>(result_list[i]),
                std::get<2>(result_list[i])
            );
        }
        return func(py_result_list, resize_ratio);
    };
}


// initialize submodule
void init_MoonDetect(py::module &module)
{
    // MoonDetect/selector.hpp
    
    module.def("select_circle_by_brightness_perc", mr::select_circle_by_brightness_perc,
        py::arg("image_in"),
        py::arg("detected_circles")
    );
    module.def("select_n_circles_by_brightness_perc", mr::select_n_circles_by_brightness_perc,
        py::arg("image_in"),
        py::arg("detected_circles"),
        py::arg("n")
    );
    module.def("select_circle_by_largest_radius", mr::select_circle_by_largest_radius,
        py::arg("image_in"),
        py::arg("detected_circles")
    );
    module.def("select_n_circles_by_largest_radius", mr::select_n_circles_by_largest_radius,
        py::arg("image_in"),
        py::arg("detected_circles"),
        py::arg("n")
    );
    module.def("select_circle_by_shape", mr::select_circle_by_shape,
        py::arg("image_in"),
        py::arg("detected_circles")
    );
    
    
    // MoonDetect/default_steps.hpp
    
    // default stage functions
    // for class MoonDetector differ by the
    // HoughCirclesAlgorithms they use.
    // Algorithm name to function prefix translation:
    // HOUGH_GRADIENT        => HG
    // HOUGH_GRADIENT_ALT    => HGA
    // HOUGH_GRADIENT_MIX    => HGM
    
    // HOUGH_GRADIENT (HG)
    module.def("HG_default_preprocess_steps", wrap_HG_default_preprocess_steps,
    py::arg("image_in"),
    R"pbdoc(
    Default function for preprocess_steps stage
    
    Parameters:
      - image_in: cv2.MatLike|numpy.ndarray
    
    Returns:
      - tuple[image_out:cv2.MatLike|numpy.ndarray, resize_ratio_out:float]
    )pbdoc");
    module.def("HG_default_param_init", wrap_HG_default_param_init,
    py::arg("image_shape"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for param_init stage
    
    Parameters:
      - image_shape: mr.utils.ImageShape
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        max_iteration:int, circle_threshold:int, hough_circles_algorithm:int,
        dp:float, minDist:float, minRadiusRate:float, minRadius:int,
        maxRadiusRate:float, maxRadius:int, param1:float, param2:float,
        cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HG_default_iteration_param_update", wrap_HG_default_iteration_param_update,
    py::arg("iteration"),
    py::arg("image_brightness_perc"),
    py::arg("initial_image_size"),
    py::arg("image_shape"),
    py::arg("curr_circle_found"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("process_image"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for iteration_param_update stage
    
    Parameters:
      - iteration: int
      - image_brightness_perc: float
      - initial_image_size: tuple
      - image_shape: mr.utils.ImageShape
      - curr_circle_found: mr.shapes.Circle
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - process_image: numpy.ndarray
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        circle_threshold:int, hough_circles_algorithm:int,
        process_image:numpy.ndarray, dp:float, minDist:float,
        minRadiusRate:float, minRadius:int, maxRadiusRate:float, maxRadius:int,
        param1:float, param2:float, cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HG_default_iteration_circle_select", wrap_HG_default_iteration_circle_select,
    py::arg("iteration"),
    py::arg("max_iteration"),
    py::arg("image_in"),
    py::arg("detected_circles"),
    R"pbdoc(
    Default function for iteration_circle_select stage
    
    Parameters:
      - iteration: int
      - max_iteration: int
      - image_in: cv2.MatLike|numpy.ndarray
      - detected_circles: numpy.ndarray, return value of cv2.HoughCircles
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    module.def("HG_default_coordinate_remap", wrap_HG_default_coordinate_remap,
    py::arg("result_list"),
    py::arg("resize_ratio"),
    R"pbdoc(
    Default function for coordinate_remap stage
    
    Parameters:
      - result_list: list[tuple(int, mr.shapes.Circle, mr.shapes.Rectangle)]
      - resize_ratio: float
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    
    // HOUGH_GRADIENT_ALT (HGA)
    module.def("HGA_default_preprocess_steps", wrap_HGA_default_preprocess_steps,
    py::arg("image_in"),
    R"pbdoc(
    Default function for preprocess_steps stage
    
    Parameters:
      - image_in: cv2.MatLike|numpy.ndarray
    
    Returns:
      - tuple[image_out:cv2.MatLike|numpy.ndarray, resize_ratio_out:float]
    )pbdoc");
    module.def("HGA_default_param_init", wrap_HGA_default_param_init,
    py::arg("image_shape"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for param_init stage
    
    Parameters:
      - image_shape: mr.utils.ImageShape
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        max_iteration:int, circle_threshold:int, hough_circles_algorithm:int,
        dp:float, minDist:float, minRadiusRate:float, minRadius:int,
        maxRadiusRate:float, maxRadius:int, param1:float, param2:float,
        cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HGA_default_iteration_param_update", wrap_HGA_default_iteration_param_update,
    py::arg("iteration"),
    py::arg("image_brightness_perc"),
    py::arg("initial_image_size"),
    py::arg("image_shape"),
    py::arg("curr_circle_found"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("process_image"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for iteration_param_update stage
    
    Parameters:
      - iteration: int
      - image_brightness_perc: float
      - initial_image_size: tuple
      - image_shape: mr.utils.ImageShape
      - curr_circle_found: mr.shapes.Circle
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - process_image: numpy.ndarray
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        circle_threshold:int, hough_circles_algorithm:int,
        process_image:numpy.ndarray, dp:float, minDist:float,
        minRadiusRate:float, minRadius:int, maxRadiusRate:float, maxRadius:int,
        param1:float, param2:float, cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HGA_default_iteration_circle_select", wrap_HGA_default_iteration_circle_select,
    py::arg("iteration"),
    py::arg("max_iteration"),
    py::arg("image_in"),
    py::arg("detected_circles"),
    R"pbdoc(
    Default function for iteration_circle_select stage
    
    Parameters:
      - iteration: int
      - max_iteration: int
      - image_in: cv2.MatLike|numpy.ndarray
      - detected_circles: numpy.ndarray, return value of cv2.HoughCircles
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    module.def("HGA_default_coordinate_remap", wrap_HGA_default_coordinate_remap,
    py::arg("result_list"),
    py::arg("resize_ratio"),
    R"pbdoc(
    Default function for coordinate_remap stage
    
    Parameters:
      - result_list: list[tuple(int, mr.shapes.Circle, mr.shapes.Rectangle)]
      - resize_ratio: float
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    
    // HOUGH_GRADIENT_MIX (HGM)
    module.def("HGM_default_preprocess_steps", wrap_HGM_default_preprocess_steps,
    py::arg("image_in"),
    R"pbdoc(
    Default function for preprocess_steps stage
    
    Parameters:
      - image_in: cv2.MatLike|numpy.ndarray
    
    Returns:
      - tuple[image_out:cv2.MatLike|numpy.ndarray, resize_ratio_out:float]
    )pbdoc");
    module.def("HGM_default_param_init", wrap_HGM_default_param_init,
    py::arg("image_shape"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for param_init stage
    
    Parameters:
      - image_shape: mr.utils.ImageShape
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        max_iteration:int, circle_threshold:int, hough_circles_algorithm:int,
        dp:float, minDist:float, minRadiusRate:float, minRadius:int,
        maxRadiusRate:float, maxRadius:int, param1:float, param2:float,
        cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HGM_default_iteration_param_update", wrap_HGM_default_iteration_param_update,
    py::arg("iteration"),
    py::arg("image_brightness_perc"),
    py::arg("initial_image_size"),
    py::arg("image_shape"),
    py::arg("curr_circle_found"),
    py::arg("max_iteration"),
    py::arg("circle_threshold"),
    py::arg("hough_circles_algorithm"),
    py::arg("process_image"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadiusRate"),
    py::arg("minRadius"),
    py::arg("maxRadiusRate"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("cut_circle_padding"),
    R"pbdoc(
    Default function for iteration_param_update stage
    
    Parameters:
      - iteration: int
      - image_brightness_perc: float
      - initial_image_size: tuple
      - image_shape: mr.utils.ImageShape
      - curr_circle_found: mr.shapes.Circle
      - max_iteration: int
      - circle_threshold: int
      - hough_circles_algorithm: int
      - process_image: numpy.ndarray
      - dp: float
      - minDist: float
      - minRadiusRate: float
      - minRadius: int
      - maxRadiusRate: float
      - maxRadius: int
      - param1: float
      - param2: float
      - cut_circle_padding: int
    
    Returns:
      - tuple[
        circle_threshold:int, hough_circles_algorithm:int,
        process_image:numpy.ndarray, dp:float, minDist:float,
        minRadiusRate:float, minRadius:int, maxRadiusRate:float, maxRadius:int,
        param1:float, param2:float, cut_circle_padding:int
      ]
    )pbdoc");
    module.def("HGM_default_iteration_circle_select", wrap_HGM_default_iteration_circle_select,
    py::arg("iteration"),
    py::arg("max_iteration"),
    py::arg("image_in"),
    py::arg("detected_circles"),
    R"pbdoc(
    Default function for iteration_circle_select stage
    
    Parameters:
      - iteration: int
      - max_iteration: int
      - image_in: cv2.MatLike|numpy.ndarray
      - detected_circles: numpy.ndarray, return value of cv2.HoughCircles
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    module.def("HGM_default_coordinate_remap", wrap_HGM_default_coordinate_remap,
    py::arg("result_list"),
    py::arg("resize_ratio"),
    R"pbdoc(
    Default function for coordinate_remap stage
    
    Parameters:
      - result_list: list[tuple(int, mr.shapes.Circle, mr.shapes.Rectangle)]
      - resize_ratio: float
    
    Returns:
      - mr.shapes.Circle
    )pbdoc");
    
    
    // MoonDetect/detector.hpp
    
    py::enum_<mr::HoughCirclesAlgorithm>(module, "HoughCirclesAlgorithm", py::arithmetic())
        .value("HOUGH_GRADIENT", mr::HoughCirclesAlgorithm::HOUGH_GRADIENT)
#ifdef MR_HAVE_HOUGH_GRADIENT_ALT
        .value("HOUGH_GRADIENT_ALT", mr::HoughCirclesAlgorithm::HOUGH_GRADIENT_ALT)
        .value("HOUGH_GRADIENT_MIX", mr::HoughCirclesAlgorithm::HOUGH_GRADIENT_MIX)
#endif
        .value("EMPTY_ALGORITHM", mr::HoughCirclesAlgorithm::EMPTY_ALGORITHM)
        .value("INVALID_ALGORITHM", mr::HoughCirclesAlgorithm::INVALID_ALGORITHM)
    ;
    
    module.def("find_circles_in_img", wrap_find_circles_in_img,
    py::arg("image_in"),
    py::arg("circle_threshold"),
    py::arg("dp"),
    py::arg("minDist"),
    py::arg("minRadius"),
    py::arg("maxRadius"),
    py::arg("param1"),
    py::arg("param2"),
    py::arg("algorithm") = static_cast<int>(cv::HOUGH_GRADIENT),
    R"pbdoc(
    A wrapper function around cv::HoughCircles
    
    Parameters:
      - image_in: gray scaled input image
      - circle_threshold: threshold on number of circles to search, set to negative number to disable threshold
        if detected_circles > circle_threshold, function will randomly sample N circles from detected_circles vector,
        where N is circle_threshold
      - dp: OpenCV parameter, inverse ratio of the accumulator resolution to the image resolution
      - minDist: OpenCV parameter, minimum distance between the centers of the detected circles
      - minRadius: OpenCV parameter, minimum circle radius
      - maxRadius: OpenCV parameter, maximum circle radius
      - param1: OpenCV parameter, first method-specific parameter
      - param2: OpenCV parameter, second method-specific parameter
      - algorithm: int hough circle algorithm for cv::HoughCircles().
        default algorithm is cv::HOUGH_GRADIENT.
    
    Returns:
      - detected_circles: numpy.ndarray representing output detected circles
    )pbdoc"
    );
    
    // class MoonDetector
    py::class_<mr::MoonDetector>(module, "MoonDetector")
        .def(py::init<>())
        // handle bytes & str param
        .def(py::init(&new_MoonDetector),
        py::arg("data"),
        R"pbdoc(
    Handling str image_filepath or bytes image_binary.
    
    Parameters:
      - parameter can be one of following:
          - str: a filepath to input image
          - bytes: raw image bytes
          - cv2.MatLike (numpy.ndarray), raw OpenCV image
            this constructor will assume cv_image is a decoded pixel matrix ready to use
            it will set cv_image to original_image directly
            colors in cv_image MUST in BGR order
        )pbdoc")
        .def("is_empty", &mr::MoonDetector::is_empty)
        .def("init_by_path", &mr::MoonDetector::init_by_path, py::arg("image_filepath"),
        R"pbdoc(
    (re)init mr.MoonDetect.MoonDetector by image_filepath
    
    Parameters:
      - image_filepath: str filepath
        )pbdoc"
        )
        .def("init_by_byte", wrap_MoonDetect_init_by_byte, py::arg("image_binary"),
        R"pbdoc(
    (re)init mr.MoonDetect.MoonDetector by image_binary
    
    Parameters:
      - image_binary: bytes raw image
        )pbdoc"
        )
        .def("init_by_mat", &mr::MoonDetector::init_by_mat, py::arg("image_in"),
        R"pbdoc(
    (re)init mr.MoonDetect.MoonDetector by image_in
    
    Parameters:
      - image_in: cv2.MatLike|numpy.ndarray image
        )pbdoc"
        )
        .def("update_hough_circles_algorithm", &mr::MoonDetector::update_hough_circles_algorithm, py::arg("algorithm"),
        R"pbdoc(
    update hough circle detection algorithm and default functions
    this function will overwrite the step function pointer base on the input algorithm
    If the library is compiled with OpenCV < 4.8.1, default we will use HOUGH_GRADIENT algorithm by default
    If the library is compiled with OpenCV >= 4.8.1, default we will use HOUGH_GRADIENT_ALT algorithm by default
        )pbdoc"
        )
        .def("detect_moon", &mr::MoonDetector::detect_moon, R"pbdoc(
    trying to find a circle from input image
    thats most likely contains the moon.
    
    Returns:
      - if success, return mr::Circle of the circle found
      - if fail (input doesn't contain circle), return mr::Circle of {-1, -1, -1}
        )pbdoc")
        // set stage function pointers
        .def("set_preprocess_steps", set_MoonDetect_preprocess_steps,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonDetector::preprocess_steps
    
    Parameters:
      - func: callable function (cv2.MatLike|numpy.ndarray) -> tuple
        )pbdoc")
        .def("set_param_init", set_MoonDetect_param_init,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonDetector::param_init
    
    Parameters:
      - func: callable function (mr.utils.ImageShape, int, int, float, float, float, int, float, int, float, float) -> tuple
        )pbdoc")
        .def("set_iteration_param_update", set_MoonDetect_iteration_param_update,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonDetector::iteration_param_update
    
    Parameters:
      - func: callable function (int, float, mr.utils.ImageShape, int, int, float, float, float, int, float, int, float, float) -> tuple
        )pbdoc")
        .def("set_iteration_circle_select", set_MoonDetect_iteration_circle_select,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonDetector::iteration_circle_select
    
    Parameters:
      - func: callable function (int, cv2.MatLike|numpy.ndarray, numpy.ndarray) -> mr.shapes.Circle
        )pbdoc")
        .def("set_coordinate_remap", set_MoonDetect_coordinate_remap,
        py::arg("func"),
        R"pbdoc(
    Set mr::MoonDetector::coordinate_remap
    
    Parameters:
      - func: callable function (list, float) -> mr.shapes.Circle
        )pbdoc")
    ;
}
