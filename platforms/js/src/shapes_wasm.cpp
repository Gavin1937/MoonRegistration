#include "MoonRegistration/shapes.hpp"


extern "C"
{

int* mrwasm_circle_to_square(int x, int y, int radius)
{
    mr::Square square = mr::circle_to_square_p(x, y, radius);
    
    // we must re-interpret mr::Square into an int array
    // in order for js to understand it
    int* output = new int[3];
    output[0] = square.x;
    output[1] = square.y;
    output[2] = square.width;
    return output;
}

int* mrwasm_circle_to_rectangle(int x, int y, int radius)
{
    mr::Rectangle rectangle = mr::circle_to_rectangle_p(x, y, radius);
    
    // we must re-interpret mr::Rectangle into an int array
    // in order for js to understand it
    int* output = new int[4];
    output[0] = rectangle.top_left_x;
    output[1] = rectangle.top_left_y;
    output[2] = rectangle.bottom_right_x;
    output[3] = rectangle.bottom_right_y;
    return output;
}

}
