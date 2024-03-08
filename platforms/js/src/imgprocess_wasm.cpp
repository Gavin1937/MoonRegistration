#include "MoonRegistration/imgprocess.hpp"
#include "MoonRegistration/shapes.hpp"

#include "utils_wasm.hpp"


extern "C"
{

void* mrwasm_cut_image_from_circle(
    void* image_in,
    int x, int y, int radius,
    int padding
)
{
    cv::Mat* image_in_ptr = reinterpret_cast<cv::Mat*>(image_in);
    cv::Mat* image_out = new cv::Mat();
    mr::Rectangle rect_out;
    
    mr::cut_image_from_circle(
        *image_in_ptr,
        *image_out, rect_out,
        x, y, radius,
        padding
    );
    
    return mrwasm_create_RectangleAndImageHandlerData(
        static_cast<int>(image_out->size[1]),
        static_cast<int>(image_out->size[0]),
        static_cast<int>(image_out->total() * image_out->elemSize()),
        reinterpret_cast<int>(image_out->data),
        reinterpret_cast<int>(image_out),
        rect_out.top_left_x,
        rect_out.top_left_y,
        rect_out.bottom_right_x,
        rect_out.bottom_right_y
    );
}

}

