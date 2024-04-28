#include <opencv2/imgproc.hpp>

#include <fstream>
#include <functional>

#include "MoonRegistration/utils.hpp"


namespace mr
{

EXPORT_SYMBOL cv::Vec3i round_vec3f(const cv::Vec3f& vec3)
{
    return cv::Vec3i(
        static_cast<int>(round(vec3[0])),
        static_cast<int>(round(vec3[1])),
        static_cast<int>(round(vec3[2]))
    );
}

EXPORT_SYMBOL bool file_exists(const std::string& filepath)
{
    std::ifstream input(filepath);
    return input.good();
}

EXPORT_SYMBOL double norm_kp(
    const cv::KeyPoint& kp1,
    const cv::KeyPoint& kp2,
    const int method
)
{
    return cv::norm(
        cv::Mat(kp1.pt),
        cv::Mat(kp2.pt),
        method
    );
}

}
