#include "MoonRegistration/MoonRegistrate/default_steps.hpp"
#include "MoonRegistration/MoonRegistrate/filter.hpp"


namespace mr
{

EXPORT_SYMBOL bool default_is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
)
{
    return mr::filter_by_lowes_ratio_test(m, n, good_match_ratio);
}

}