#include "../include/MoonDetect_pywrapper.hpp"

#include <pybind11/stl.h>

#include "../include/ndarray_converter.h"
#include "../include/cvvec3f_vector_ndarray.hpp"


std::vector<cv::Vec3f> wrap_find_circles_in_img(
    const cv::Mat& image_in,
    const int circle_threshold,
    const double dp,
    const double minDist,
    const int minRadius,
    const int maxRadius,
    const double param1,
    const double param2
)
{
    std::vector<cv::Vec3f> output;
    mr::find_circles_in_img(
        image_in, output, circle_threshold,
        dp, minDist,
        minRadius, maxRadius,
        param1, param2
    );
    return output;
}

// default stage functions

py::tuple wrap_default_preprocess_steps(
    const cv::Mat& image_in
)
{
    cv::Mat image_out;
    float resize_ratio_out = 0.0;
    mr::default_preprocess_steps(image_in, image_out, resize_ratio_out);
    return py::make_tuple(image_out, py::float_(resize_ratio_out));
}

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
)
{
    mr::default_param_init(
        image_shape, max_iteration, circle_threshold, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2
    );
    return py::make_tuple(
        py::int_(max_iteration), py::int_(circle_threshold),
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2)
    );
}

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
)
{
    mr::default_iteration_param_update(
        iteration, image_brightness_perc, image_shape,
        max_iteration, circle_threshold, dp, minDist,
        minRadiusRate, minRadius, maxRadiusRate, maxRadius,
        param1, param2
    );
    return py::make_tuple(
        py::int_(max_iteration), py::int_(circle_threshold),
        py::float_(dp), py::float_(minDist),
        py::float_(minRadiusRate), py::int_(minRadius),
        py::float_(maxRadiusRate), py::int_(maxRadius),
        py::float_(param1), py::float_(param2)
    );
}

mr::Circle wrap_default_iteration_circle_select(
    const int iteration,
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    return mr::default_iteration_circle_select(iteration, image_in, detected_circles);
}

mr::Circle wrap_default_coordinate_remap(
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
    return mr::default_coordinate_remap(cpp_result_list, resize_ratio);
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
    std::function<py::tuple(const mr::ImageShape&, int, int, double, double, double, int, double, int, double, double)> func
)
{
    self.param_init = [func](
        const mr::ImageShape& image_shape,
        int& max_iteration, int& circle_threshold,
        double& dp, double& minDist,
        double& minRadiusRate, int& minRadius,
        double& maxRadiusRate, int& maxRadius,
        double& param1, double& param2
    ){
        py::tuple ret = func(
            image_shape, max_iteration, circle_threshold,
            dp, minDist,
            minRadiusRate, minRadius, maxRadiusRate, maxRadius,
            param1, param2
        );
        max_iteration = ret[0].cast<int>();
        circle_threshold = ret[1].cast<int>();
        dp = ret[2].cast<double>();
        minDist = ret[3].cast<double>();
        minRadiusRate = ret[4].cast<double>();
        minRadius = ret[5].cast<int>();
        maxRadiusRate = ret[6].cast<double>();
        maxRadius = ret[7].cast<int>();
        param1 = ret[8].cast<double>();
        param2 = ret[9].cast<double>();
    };
}

void set_MoonDetect_iteration_param_update(
    mr::MoonDetector& self,
    std::function<py::tuple(const int, const float, const mr::ImageShape&, int, int, double, double, double, int, double, int, double, double)> func
)
{
    self.iteration_param_update = [func](
        const int iteration, const float image_brightness_perc,
        const mr::ImageShape& image_shape,
        int& max_iteration, int& circle_threshold,
        double& dp, double& minDist,
        double& minRadiusRate, int& minRadius,
        double& maxRadiusRate, int& maxRadius,
        double& param1, double& param2
    ){
        py::tuple ret = func(
            iteration, image_brightness_perc, image_shape,
            max_iteration, circle_threshold, dp, minDist,
            minRadiusRate, minRadius, maxRadiusRate, maxRadius,
            param1, param2
        );
        max_iteration = ret[0].cast<int>();
        circle_threshold = ret[1].cast<int>();
        dp = ret[2].cast<double>();
        minDist = ret[3].cast<double>();
        minRadiusRate = ret[4].cast<double>();
        minRadius = ret[5].cast<int>();
        maxRadiusRate = ret[6].cast<double>();
        maxRadius = ret[7].cast<int>();
        param1 = ret[8].cast<double>();
        param2 = ret[9].cast<double>();
    };
}

void set_MoonDetect_iteration_circle_select(
    mr::MoonDetector& self,
    std::function<mr::Circle(const int, const cv::Mat&, const std::vector<cv::Vec3f>&)> func
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
