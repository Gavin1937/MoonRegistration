#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <random>
#include <algorithm>
#include <functional>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


namespace mr
{

EXPORT_SYMBOL cv::Vec3i round_vec3f(const cv::Vec3f& vec3);

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp(TYPE value, TYPE lower, TYPE higher)
{
    return (
        (value < lower) ? lower : ((value > higher) ? higher : value)
    );
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp_lower(TYPE value, TYPE lower)
{
    return ((value < lower) ? lower : value);
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE clamp_higher(TYPE value, TYPE higher)
{
    return ((value > higher) ? higher : value);
}

template <class TYPE>
EXPORT_SYMBOL inline TYPE make_binary_num(TYPE value, TYPE threshold, TYPE minval, TYPE maxval)
{
    return ((value <= threshold) ? minval : maxval);
}


template <typename COMP_TYPE, typename DATA_TYPE>
class Comparator
{
public:
    bool operator()(const std::pair<COMP_TYPE,DATA_TYPE>& lhs, const std::pair<COMP_TYPE,DATA_TYPE>& rhs)
    {
        return (lhs.first > rhs.first);
    }
};

// Find n circles from input circle vector
// 
// Parameters:
//   - n: int, number of circles to return
//   - detected_circles: input circles
//   - output: output buffer
//   - calc_value_func: a function pointer to calculate the value for circle comparison
//   - filter_value_func: a function pointer to filter circle base on the value from calc_value_func. (default return true)
template<typename COMP_TYPE>
EXPORT_SYMBOL void find_n_circles(
    const int n,
    const std::vector<cv::Vec3f>& detected_circles,
    std::vector<cv::Vec3f>& output,
    std::function<COMP_TYPE(const cv::Vec3f&)> calc_value_func,
    std::function<bool(const COMP_TYPE&)> filter_value_func =
    [](const COMP_TYPE&) { return true; }
)
{
    output.reserve(n);
    
    // find n largest element in the list
    // https://stackoverflow.com/a/22654973
    
    std::priority_queue<
        std::pair<COMP_TYPE,cv::Vec3f>,
        std::vector<std::pair<COMP_TYPE,cv::Vec3f>>,
        Comparator<COMP_TYPE,cv::Vec3f>
    > minheap;
    
    for (auto vec : detected_circles)
    {
        COMP_TYPE value = calc_value_func(vec);
        bool filter_value = filter_value_func(value);
        
        // minheap.size() < n
        if (filter_value && minheap.size() < n)
        {
            minheap.push(std::make_pair(value,vec));
        }
        // minheap.size() >= n
        else if (filter_value && value > minheap.top().first)
        {
            minheap.pop();
            minheap.push(std::make_pair(value,vec));
        }
    }
    
    for (int i = 0; i < n; ++i)
    {
        output[i] = minheap.top().second;
        minheap.pop();
    }
}

EXPORT_SYMBOL bool file_exists(const std::string& filepath);

// Randomly sample n elements from input vector, and write them to output vector
// If input vector length <= n, copy input vector to output vector
// 
// Parameters:
//   - n: int, number of elements to sample
//   - vec_in: input vector
//   - vec_out: output vector
template <typename TYPE>
EXPORT_SYMBOL void sample_vector(
    const int n,
    const std::vector<TYPE>& vec_in,
    std::vector<TYPE>& vec_out
)
{
    if (vec_in.size() <= n)
    {
        vec_out = vec_in;
        return;
    }
    
    if (!vec_out.empty())
        vec_out.clear();
    vec_out.reserve(n);
    
    std::random_device device;
    std::mt19937 rng(device());
    
    int m = static_cast<int>(n * 1.5);
    int lbound = 0;
    int ubound = static_cast<int>(vec_in.size());
    int start = lbound + (rng() % ubound);
    int end = start + m;
    
    std::vector<int> indexes;
    indexes.reserve(m);
    
    // generate m indexes from start to end
    // where start is a random index selected from lbound (lower bound) to ubound (upper bound)
    // then shuffle those indexes
    int val = -1;
    for (int idx = start; idx < end; ++idx)
    {
        val = (idx > ubound) ? (idx % ubound) : idx;
        indexes.push_back(val);
    }
    std::shuffle(indexes.begin(), indexes.end(), rng);
    
    // finally using those indexes to copy n elements from vec_in to vec_out
    for (int i = 0; i < n; ++i)
        vec_out.push_back(vec_in[indexes[i]]);
}

// A wrapper function to run cv::norm with cv::Point2f
// 
// Parameters:
//   - ptf1: cv::Point2f, input left Point2f
//   - ptf2: cv::Point2f, input right Point2f
//   - method: int algorithm for cv::norm, default cv::NORM_L2
EXPORT_SYMBOL double norm_ptf(
    const cv::Point2f& ptf1,
    const cv::Point2f& ptf2,
    const int method
);

// A wrapper function to run cv::norm with cv::KeyPoint
// 
// Parameters:
//   - kp1: cv::KeyPoint, input left KeyPoint
//   - kp2: cv::KeyPoint, input right KeyPoint
//   - method: int algorithm for cv::norm, default cv::NORM_L2
EXPORT_SYMBOL double norm_kp(
    const cv::KeyPoint& kp1,
    const cv::KeyPoint& kp2,
    const int method = cv::NORM_L2
);

}
