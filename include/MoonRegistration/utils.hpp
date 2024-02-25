#pragma once

#include <opencv2/core/mat.hpp>

#include <string>
#include <vector>
#include <utility>
#include <queue>
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
template<typename COMP_TYPE> EXPORT_SYMBOL void find_n_circles(
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
        
        // minheap.size() < n
        if (filter_value_func(value) && minheap.size() < n)
        {
            minheap.push(std::make_pair(value,vec));
        }
        // minheap.size() >= n
        else if (filter_value_func(value) && value > minheap.top().first)
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

}
