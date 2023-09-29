#include "../include/MoonRegistration/shapes.hpp"


namespace mr
{

extern "C"
{

EXPORT_SYMBOL Square circle_to_square_s(const Circle& circle)
{
    int top_left_y = circle.y - circle.radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = circle.x - circle.radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * circle.radius;
    
    return {top_left_x, top_left_y, square_width};
}

EXPORT_SYMBOL Square circle_to_square_p(int x, int y, int radius)
{
    int top_left_y = y - radius;
    if (top_left_y < 0) top_left_y = 0;
    
    int top_left_x = x - radius;
    if (top_left_x < 0) top_left_x = 0;
    
    int square_width = 2 * radius;
    
    return {top_left_x, top_left_y, square_width};
}

EXPORT_SYMBOL Rectangle circle_to_rectangle_s(const Circle& circle)
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

EXPORT_SYMBOL Rectangle circle_to_rectangle_p(int x, int y, int radius)
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

}


EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Circle& in)
{
    out << "(x=" << in.x
        << ", y=" << in.y
        << ", radius=" << in.radius
        << ")"
    ;
    return out;
}

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Square& in)
{
    out << "(x=" << in.x
        << ", y=" << in.y
        << ", width=" << in.width
        << ")"
    ;
    return out;
}

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Rectangle& in)
{
    out << "(top_left_x=" << in.top_left_x
        << ", top_left_y=" << in.top_left_y
        << ", bottom_right_x=" << in.bottom_right_x
        << ", bottom_right_y=" << in.bottom_right_y
        << ")"
    ;
    return out;
}
