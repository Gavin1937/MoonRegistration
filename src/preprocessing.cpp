#include "MoonRegistration/preprocessing.hpp"

namespace mr
{

EXPORT_SYMBOL void resize_with_aspect_ratio(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    float& ratio_out,
    int width,
    int height,
    int longer_side_val,
    int inter
)
{
    cv::Size dimension;
    int h = image_in.size[0];
    int w = image_in.size[1];
    float r = 1.0;
    
    if (longer_side_val > 0)
    {
        if (h > w && h > longer_side_val)
        {
            height = longer_side_val;
            width = -1;
        }
        else if (w >= h && w > longer_side_val)
        {
            height = -1;
            width = longer_side_val;
        }
        else // w & h are too short
        {
            ratio_out = r;
            image_in.copyTo(image_out);
            return;
        }
    }
    if (width <= 0 && height <= 0)
    {
        ratio_out = r;
        image_in.copyTo(image_out);
        return;
    }
    if (width <= 0)
    {
        r = static_cast<float>(height) / static_cast<float>(h);
        dimension = cv::Size(static_cast<int>(w * r), static_cast<int>(height));
    }
    else // height is None
    {
        r = static_cast<float>(width) / static_cast<float>(w);
        dimension = cv::Size(static_cast<int>(width), static_cast<int>(h * r));
    }
    
    cv::resize(image_in, image_out, dimension, (0.0), (0.0), inter);
    ratio_out = r;
}

EXPORT_SYMBOL void apply_brightness_contrast(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    int brightness,
    int contrast
)
{
    int shadow = 0;
    int highlight = 0;
    double alpha_b = 0.0;
    double gamma_b = 0.0;
    double f = 0.0;
    double alpha_c = 0.0;
    double gamma_c = 0.0;
    
    if (brightness != 0)
    {
        if (brightness > 0)
        {
            shadow = brightness;
            highlight = 255;
        }
        else
        {
            shadow = 0;
            highlight = 255 + brightness;
        }
        alpha_b = (double)(highlight - shadow)/255.0;
        gamma_b = (double)shadow;
        
        cv::addWeighted(image_in, alpha_b, image_in, 0, gamma_b, image_out);
    }
    else
        image_in.copyTo(image_out);
    
    if (contrast != 0)
    {
        f = 131.0*((double)contrast + 127.0)/(127.0*(131.0-(double)contrast));
        alpha_c = f;
        gamma_c = 127.0*(1.0-f);
        
        cv::addWeighted(image_out, alpha_c, image_out, 0, gamma_c, image_out);
    }
}

EXPORT_SYMBOL float calc_img_brightness_perc(
    const cv::Mat& image_in
)
{
    int pixel_sum = 0;
    int height = image_in.size[0];
    int width = image_in.size[1];
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            pixel_sum += static_cast<int>(image_in.at<uchar>(y, x));
        }
    }
    
    return static_cast<float>(
        static_cast<float>(pixel_sum) / 255.0 / static_cast<float>(width*height)
    );
}

EXPORT_SYMBOL float calc_circle_brightness_perc(
    const cv::Mat& image_in,
    int center_x,
    int center_y,
    int radius
)
{
    int height = image_in.size[0];
    int width = image_in.size[1];
    
    int xmin = center_x - radius;
    if (xmin <= 0) xmin = 0;
    int xmax = center_x + radius;
    if (xmax >= width) xmax = width;
    int ymin = center_y - radius;
    if (ymin <= 0) ymin = 0;
    int ymax = center_y + radius;
    if (ymax >= height) ymax = height;
    int radiusSquared = radius * radius;
    
    int pixel_sum = 0;
    int pixel_count = 0;
    for (int x = xmin; x < xmax; ++x)
    {
        for (int y = ymin; y < ymax; ++y)
        {
            int dx = x - center_x;
            int dy = y - center_y;
            int distanceSquared = dx * dx + dy * dy;
            
            // inside circle
            if (distanceSquared <= radiusSquared)
            {
                pixel_sum += static_cast<int>(image_in.at<uchar>(y, x));
                pixel_count += 1;
            }
        }
    }
    
    return static_cast<float>(
        static_cast<float>(pixel_sum) / 255.0 / static_cast<float>(width*height)
    );
}

EXPORT_SYMBOL void cut_ref_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    mr::Rectangle& rect_out,
    int x,
    int y,
    int radius,
    int padding
)
{
    int height = image_in.size[0];
    int width = image_in.size[1];
    
    int yStart = y - radius - padding;
    if (yStart < 0) yStart = 0;
    
    int yEnd = y + radius + padding;
    if (yEnd > height) yEnd = height;
    
    int xStart = x - radius - padding;
    if (xStart < 0) xStart = 0;
    
    int xEnd = x + radius + padding;
    if (xEnd > width) xEnd = width;
    
    // use cv::Mat::operator() to extract a sub-matrix as reference
    // and then copy the reference to image_out
    image_out = image_in(cv::Range(yStart, yEnd), cv::Range(xStart, xEnd));
    rect_out = {xStart, yStart, xEnd, yEnd};
}

EXPORT_SYMBOL void cut_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    mr::Rectangle& rect_out,
    int x,
    int y,
    int radius,
    int padding
)
{
    cut_ref_image_from_circle(image_in, image_out, rect_out, x, y, radius, padding);
    // copy content of image_out to a new cv::Mat
    image_out = image_out.clone();
}

}
