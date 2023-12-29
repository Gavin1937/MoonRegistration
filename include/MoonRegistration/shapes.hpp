#pragma once

#include <opencv2/core/mat.hpp>

#include <iostream>

#include "macros.h"


namespace mr
{

EXPORT_SYMBOL typedef struct Circle {
    int x;
    int y;
    int radius;
} Circle;

EXPORT_SYMBOL typedef struct Square {
    int x;
    int y;
    int width;
} Square;

EXPORT_SYMBOL typedef struct Rectangle {
    int top_left_x;
    int top_left_y;
    int bottom_right_x;
    int bottom_right_y;
} Rectangle;


EXPORT_SYMBOL Square circle_to_square_s(const Circle& circle);

EXPORT_SYMBOL Square circle_to_square_p(int x, int y, int radius);

EXPORT_SYMBOL Rectangle circle_to_rectangle_s(const Circle& circle);

EXPORT_SYMBOL Rectangle circle_to_rectangle_p(int x, int y, int radius);


EXPORT_SYMBOL cv::Vec3f circle_to_vec3(const Circle& circle);

EXPORT_SYMBOL Circle vec3_to_circle(const cv::Vec3f& vec3);

}


EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Circle& in);

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Square& in);

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Rectangle& in);

