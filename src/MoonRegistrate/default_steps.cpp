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
    return (
        mr::filter_by_ignore_edge_kp(user_kpt, model_kpt, user_image, model_image, 0.9f) &&
        mr::filter_by_lowes_ratio_test(m, n, 0.85f)
    );
}

EXPORT_SYMBOL bool default_is_good_match_lowes_ratio_only(
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

EXPORT_SYMBOL bool default_is_good_match_all(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
)
{
    return (
        mr::filter_by_ignore_edge_kp(user_kpt, model_kpt, user_image, model_image, 0.9f) &&
        mr::filter_by_lowes_ratio_test(m, n, 0.85f) &&
        mr::filter_by_ignore_close_kp(user_kpt, model_kpt, user_image, model_image, 0.03f)
    );
}

}
