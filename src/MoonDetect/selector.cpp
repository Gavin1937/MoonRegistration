#include "../../include/MoonRegistration/MoonDetect/selector.hpp"
#include "../../include/MoonRegistration/preprocessing.hpp"

#include <utility>
#include <queue>


// forward declaration will fail
class Comparator
{
public:
    bool operator()(const std::pair<float,cv::Vec3f>& lhs, const std::pair<float,cv::Vec3f>& rhs)
    {
        return (lhs.first > rhs.first);
    }
};

namespace mr
{

EXPORT_SYMBOL Circle select_circle_by_brightness_perc(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    float max_mean = -1.0;
    
    if (!detected_circles.empty())
    {
        for (auto vec : detected_circles)
        {
            // calc circle brightness percentage (pixel mean)
            float mean = calc_circle_brightness_perc(
                image_in,
                static_cast<int>(vec[0]),
                static_cast<int>(vec[1]),
                static_cast<int>(vec[2])
            );
            // find the maximum mean thats below 0.98
            if (mean < 0.98 && mean > max_mean)
            {
                x = static_cast<int>(vec[0]);
                y = static_cast<int>(vec[1]);
                radius = static_cast<int>(vec[2]);
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
    
    // find n largest element in list
    // https://stackoverflow.com/a/22654973
    
    std::priority_queue<std::pair<float,cv::Vec3f>, std::vector<std::pair<float,cv::Vec3f>>, Comparator> minheap;
    
    for (auto vec : detected_circles)
    {
        // calc circle brightness percentage (pixel mean)
        float mean = calc_circle_brightness_perc(
            image_in,
            static_cast<int>(vec[0]),
            static_cast<int>(vec[1]),
            static_cast<int>(vec[2])
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
    
    std::vector<cv::Vec3f> result;
    result.reserve(minheap.size());
    while (!minheap.empty())
    {
        std::pair<float,cv::Vec3f> item = minheap.top();
        minheap.pop();
        result.emplace_back(item.second);
    }
    return result;
}

EXPORT_SYMBOL Circle select_circle_by_largest_radius(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    
    if (!detected_circles.empty())
    {
        // checks for circles then finds biggest circle with HoughCircle parameters
        for (auto vec : detected_circles)
        {
            if (vec[2] > radius)
            {
                x = static_cast<int>(vec[0]);
                y = static_cast<int>(vec[1]);
                radius = static_cast<int>(vec[2]);
            }
        }
    }
    
    return {x,y,radius};
}

EXPORT_SYMBOL Circle select_circle_by_shape(
    const cv::Mat& image_in,
    const std::vector<cv::Vec3f>& detected_circles
)
{
    int x = -1;
    int y = -1;
    int radius = -1;
    int max_contour = -1;
    
    // checks for circles then finds the circle contains a shape with largest number of sides
    if (!detected_circles.empty())
    {
        for (auto vec : detected_circles)
        {
            cv::Mat circle;
            Rectangle rect_out;
            cut_image_from_circle(
                image_in, circle, rect_out,
                static_cast<int>(vec[0]),
                static_cast<int>(vec[1]),
                static_cast<int>(vec[2])
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
                x = static_cast<int>(vec[0]);
                y = static_cast<int>(vec[1]);
                radius = static_cast<int>(vec[2]);
            }
        }
    }
    
    return {x,y,radius};
}

}
