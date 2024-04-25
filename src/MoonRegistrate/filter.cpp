#include "MoonRegistration/MoonRegistrate/filter.hpp"


namespace mr
{

EXPORT_SYMBOL bool filter_by_lowes_ratio_test(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float ratio
)
{
    return (m.distance < ratio * n.distance);
}

};