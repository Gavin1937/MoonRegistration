#pragma once

#include <opencv2/core/mat.hpp>

#include "MoonRegistration/c_mrapi/utils_c_api.h"


inline cv::Mat& mrc_ptr_to_mat(mat_ptr ptr)
{
    return *reinterpret_cast<cv::Mat*>(ptr);
}

inline mat_ptr mrc_mat_to_ptr(cv::Mat& mat)
{
    return reinterpret_cast<mat_ptr>(&mat);
}

inline mat_ptr mrc_matptr_to_ptr(cv::Mat* mat)
{
    return reinterpret_cast<mat_ptr>(mat);
}
