#include "../../include/MoonRegistration/MoonDetect/selector.hpp"
#include "../../include/MoonRegistration/preprocessing.hpp"
#include "../../include/MoonRegistration/utils.hpp"

#include <utility>
#include <queue>


// forward declaration will fail
template <typename COMP_TYPE, typename DATA_TYPE>
class Comparator
{
public:
    bool operator()(const std::pair<COMP_TYPE,DATA_TYPE>& lhs, const std::pair<COMP_TYPE,DATA_TYPE>& rhs)
    {
        return (lhs.first > rhs.first);
    }
};

namespace mr
{

EXPORT_SYMBOL mr::Circle select_circle_by_brightness_perc(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    float max_mean = -1.0;
    cv::Vec3i veci;
    
    if (!detected_circles.empty())
    {
        for (auto vec : detected_circles)
        {
            veci = mr::round_vec3f(vec);
            // calc circle brightness percentage (pixel mean)
            float mean = calc_circle_brightness_perc(
                image_in,
                veci[0], veci[1], veci[2]
            );
            // find the maximum mean thats below 0.98
            if (mean < 0.98 && mean > max_mean)
            {
                x = veci[0];
                y = veci[1];
                radius = veci[2];
                max_mean = mean;
            }
        }
    }
    
    return {x,y,radius};
}

EXPORT_SYMBOL std::vector<cv::Vec3f> select_n_circles_by_brightness_perc(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles,
    int n
)
{
    if (detected_circles.empty())
        return std::vector<cv::Vec3f>();
    else if (detected_circles.size() <= n)
        return detected_circles;
    
    // find n largest element in the list
    // https://stackoverflow.com/a/22654973
    
    std::priority_queue<
        std::pair<float,cv::Vec3f>,
        std::vector<std::pair<float,cv::Vec3f>>,
        Comparator<float,cv::Vec3f>
    > minheap;
    cv::Vec3i veci;
    
    for (auto vec : detected_circles)
    {
        veci = mr::round_vec3f(vec);
        // calc circle brightness percentage (pixel mean)
        float mean = calc_circle_brightness_perc(
            image_in,
            veci[0], veci[1], veci[2]
        );
        // find the maximum mean thats below 0.98
        if (mean < 0.98 && minheap.size() < n)
        {
            minheap.push(std::pair<float,cv::Vec3f>(mean,vec));
        }
        else if (mean < 0.98 && mean > minheap.top().first)
        {
            minheap.pop();
            minheap.push(std::pair<float,cv::Vec3f>(mean,vec));
        }
    }
    
    std::vector<cv::Vec3f> result(n);
    for (int i = 0; i < n; ++i)
    {
        result[i] = minheap.top().second;
        minheap.pop();
    }
    return result;
}

EXPORT_SYMBOL mr::Circle select_circle_by_largest_radius(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    cv::Vec3i veci;
    
    if (!detected_circles.empty())
    {
        // checks for circles then finds biggest circle with HoughCircle parameters
        for (auto vec : detected_circles)
        {
            veci = mr::round_vec3f(vec);
            if (veci[2] > radius)
            {
                x = veci[0];
                y = veci[1];
                radius = veci[2];
            }
        }
    }
    
    return {x,y,radius};
}

EXPORT_SYMBOL std::vector<cv::Vec3f> select_n_circles_by_largest_radius(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles,
    int n
)
{
    if (detected_circles.empty())
        return std::vector<cv::Vec3f>();
    else if (detected_circles.size() <= n)
        return detected_circles;
    
    // find n largest element in the list
    // https://stackoverflow.com/a/22654973
    
    std::priority_queue<
        std::pair<int,cv::Vec3f>,
        std::vector<std::pair<int,cv::Vec3f>>,
        Comparator<int,cv::Vec3f>
    > minheap;
    cv::Vec3i veci;
    int radius = -1;
    
    for (auto vec : detected_circles)
    {
        veci = mr::round_vec3f(vec);
        radius = veci[2];
        
        if (minheap.size() < n)
        {
            minheap.push(std::pair<int,cv::Vec3f>(radius,vec));
        }
        else if (radius > minheap.top().first)
        {
            minheap.pop();
            minheap.push(std::pair<int,cv::Vec3f>(radius,vec));
        }
    }
    
    std::vector<cv::Vec3f> result(n);
    for (int i = 0; i < n; ++i)
    {
        result[i] = minheap.top().second;
        minheap.pop();
    }
    return result;
}

EXPORT_SYMBOL mr::Circle select_circle_by_shape(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    int max_contour = -1;
    cv::Vec3i veci;
    
    // checks for circles then finds the circle contains a shape with largest number of sides
    if (!detected_circles.empty())
    {
        for (auto vec : detected_circles)
        {
            veci = mr::round_vec3f(vec);
            cv::Mat circle;
            mr::Rectangle rect_out;
            cut_ref_image_from_circle(
                image_in, circle, rect_out,
                veci[0], veci[1], veci[2]
            );
            
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(circle, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
            int max_approx_len = -1;
            std::vector<std::vector<cv::Point>> approx(contours.size());
            
            for (int k = 0; k < contours.size(); ++k)
            {
                cv::approxPolyDP(cv::Mat(contours[k]), approx[k], 0.01 * cv::arcLength(contours[k], true), true);
                int approx_len = static_cast<int>(approx[k].size());
                if (approx_len > max_approx_len)
                    max_approx_len = approx_len;
            }
            if (max_approx_len > max_contour)
            {
                max_contour = max_approx_len;
                x = veci[0];
                y = veci[1];
                radius = veci[2];
            }
        }
    }
    
    return {x,y,radius};
}

}
