#include <sstream>

#include "MoonRegistration/shapes.hpp"


namespace mr
{

EXPORT_SYMBOL Square circle_to_square(const Circle& circle)
{
    int top_left_y = circle.y - circle.radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = circle.x - circle.radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * circle.radius;
    
    return {top_left_x, top_left_y, square_width};
}

EXPORT_SYMBOL Square circle_to_square(int x, int y, int radius)
{
    int top_left_y = y - radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = x - radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * radius;
    
    return {top_left_x, top_left_y, square_width};
}

EXPORT_SYMBOL Rectangle circle_to_rectangle(const Circle& circle)
{
    int top_left_y = circle.y - circle.radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = circle.x - circle.radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * circle.radius;
    int bottom_right_x = top_left_x + square_width;
    int bottom_right_y = top_left_y + square_width;
    
    return {top_left_x, top_left_y, bottom_right_x, bottom_right_y};
}

EXPORT_SYMBOL Rectangle circle_to_rectangle(int x, int y, int radius)
{
    int top_left_y = y - radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = x - radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * radius;
    int bottom_right_x = top_left_x + square_width;
    int bottom_right_y = top_left_y + square_width;
    
    return {top_left_x, top_left_y, bottom_right_x, bottom_right_y};
}

EXPORT_SYMBOL bool is_valid_circle(const Circle& circle)
{
    return (circle.x >= 0 && circle.y >= 0 && circle.radius >= 0);
}

EXPORT_SYMBOL bool is_valid_circle(int x, int y, int radius)
{
    return (x >= 0 && y >= 0 && radius >= 0);
}

EXPORT_SYMBOL std::string circle_to_string(const Circle& circle)
{
    std::stringstream out;
    out << "(x=" << circle.x
        << ", y=" << circle.y
        << ", radius=" << circle.radius
        << ")"
    ;
    return out.str();
}

EXPORT_SYMBOL std::string square_to_string(const Square& square)
{
    std::stringstream out;
    out << "(x=" << square.x
        << ", y=" << square.y
        << ", width=" << square.width
        << ")"
    ;
    return out.str();
}

EXPORT_SYMBOL std::string rectangle_to_string(const Rectangle& rectangle)
{
    std::stringstream out;
    out << "(top_left_x=" << rectangle.top_left_x
        << ", top_left_y=" << rectangle.top_left_y
        << ", bottom_right_x=" << rectangle.bottom_right_x
        << ", bottom_right_y=" << rectangle.bottom_right_y
        << ")"
    ;
    return out.str();
}


EXPORT_SYMBOL cv::Vec3f circle_to_vec3(const Circle& circle)
{
    return cv::Vec3f(
        static_cast<float>(circle.x),
        static_cast<float>(circle.y),
        static_cast<float>(circle.radius)
    );
}

EXPORT_SYMBOL Circle vec3_to_circle(const cv::Vec3f& vec3)
{
    return {
        static_cast<int>(vec3[0]),
        static_cast<int>(vec3[1]),
        static_cast<int>(vec3[2])
    };
}

EXPORT_SYMBOL cv::Rect rectangle_to_roi(const Rectangle& rectangle)
{
    return cv::Rect(
        rectangle.top_left_x, rectangle.top_left_y,
        std::abs(rectangle.bottom_right_x - rectangle.top_left_x),
        std::abs(rectangle.bottom_right_y - rectangle.top_left_y)
    );
}

EXPORT_SYMBOL Rectangle roi_to_rectangle(const cv::Rect& roi)
{
    return {
        roi.x, roi.y, roi.x+roi.width, roi.y+roi.height
    };
}

}


EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Circle& in)
{
    out << mr::circle_to_string(in);
    return out;
}

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Square& in)
{
    out << mr::square_to_string(in);
    return out;
}

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Rectangle& in)
{
    out << mr::rectangle_to_string(in);
    return out;
}

