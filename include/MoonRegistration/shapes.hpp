#pragma once

#include <opencv2/core/mat.hpp>

#include <iostream>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


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


EXPORT_SYMBOL Square circle_to_square(const Circle& circle);

EXPORT_SYMBOL Square circle_to_square(int x, int y, int radius);

EXPORT_SYMBOL Rectangle circle_to_rectangle(const Circle& circle);

EXPORT_SYMBOL Rectangle circle_to_rectangle(int x, int y, int radius);

EXPORT_SYMBOL bool is_valid_circle(const Circle& circle);

EXPORT_SYMBOL bool is_valid_circle(int x, int y, int radius);


EXPORT_SYMBOL cv::Vec3f circle_to_vec3(const Circle& circle);

EXPORT_SYMBOL Circle vec3_to_circle(const cv::Vec3f& vec3);

EXPORT_SYMBOL cv::Rect rectangle_to_roi(const Rectangle& rectangle);

EXPORT_SYMBOL Rectangle roi_to_rectangle(const cv::Rect& roi);

}


EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Circle& in);

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Square& in);

EXPORT_SYMBOL std::ostream& operator<<(std::ostream& out, const mr::Rectangle& in);

