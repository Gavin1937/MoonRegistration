#include <exception>

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
    try
    {
        cv::Mat* image_in_ptr = reinterpret_cast<cv::Mat*>(image_in);
        cv::Mat* image_out = new cv::Mat();
        mr::Rectangle rect_out;
        mr::Circle circle_in = {x, y, radius};
        
        mr::cut_image_from_circle(
            *image_in_ptr,
            *image_out, rect_out,
            circle_in,
            padding
        );
        
        return mrwasm_create_RectangleAndImageHandlerData(
            image_out,
            rect_out.top_left_x,
            rect_out.top_left_y,
            rect_out.bottom_right_x,
            rect_out.bottom_right_y
        );
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

void* mrwasm_stack_imgs(
    void* background,
    const int roi_x, const int roi_y,
    const int roi_width, const int roi_height,
    void* foreground,
    const float foreground_transparency,
    const int filter_px,
    const bool auto_resize
)
{
    try
    {
        cv::Mat* background_ptr = reinterpret_cast<cv::Mat*>(background);
        cv::Mat* foreground_ptr = reinterpret_cast<cv::Mat*>(foreground);
        
        cv::Rect background_roi(roi_x, roi_y, roi_width, roi_height);
        
        cv::Vec4b* vec4b_filter_px = NULL;
        if (filter_px)
        {
            const unsigned char* filter_px_ptr = reinterpret_cast<const unsigned char*>(&filter_px);
            vec4b_filter_px = new cv::Vec4b(
                filter_px_ptr[0],
                filter_px_ptr[1],
                filter_px_ptr[2],
                filter_px_ptr[3]
            );
        }
        
        cv::Mat* image_out = new cv::Mat();
        mr::stack_imgs(
            *background_ptr, background_roi,
            *foreground_ptr, *image_out,
            foreground_transparency,
            vec4b_filter_px,
            auto_resize
        );
        
        if (vec4b_filter_px)
        {
            vec4b_filter_px->~Vec();
            delete vec4b_filter_px;
        }
        
        return mrwasm_create_ImageHandlerData(image_out);
    }
    catch(const std::exception& error)
    {
        throw error;
    }
}

}

