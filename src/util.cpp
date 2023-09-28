#include "../include/MoonRegistration/util.hpp"


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
