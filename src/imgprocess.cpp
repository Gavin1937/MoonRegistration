#include "MoonRegistration/imgprocess.hpp"
#include "MoonRegistration/utils.hpp"

namespace mr
{

EXPORT_SYMBOL mr::ImageShape calc_image_shape(const cv::Mat& image_in)
{
    mr::ImageShape shape;
    shape.height = image_in.size[0];
    shape.width = image_in.size[1];
    if (shape.height >= shape.width)
    {
        shape.longer_side = shape.height;
        shape.shorter_side = shape.width;
    }
    else
    {
        shape.longer_side = shape.width;
        shape.shorter_side = shape.height;
    }
    shape.is_landscape = shape.width > shape.height;
    shape.is_portrait = shape.width < shape.height;
    shape.is_square = shape.width == shape.height;
    
    return shape;
}

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
            image_out = image_in.clone();
            return;
        }
    }
    if (width <= 0 && height <= 0)
    {
        ratio_out = r;
        image_out = image_in.clone();
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
        image_out = image_in.clone();
    
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
    const mr::Circle& circle_in
)
{
    int height = image_in.size[0];
    int width = image_in.size[1];
    
    int xmin = circle_in.x - circle_in.radius;
    if (xmin <= 0) xmin = 0;
    int xmax = circle_in.x + circle_in.radius;
    if (xmax >= width) xmax = width;
    int ymin = circle_in.y - circle_in.radius;
    if (ymin <= 0) ymin = 0;
    int ymax = circle_in.y + circle_in.radius;
    if (ymax >= height) ymax = height;
    int radius_squared = circle_in.radius * circle_in.radius;
    
    int pixel_sum = 0;
    int pixel_count = 0;
    for (int x = xmin; x < xmax; ++x)
    {
        for (int y = ymin; y < ymax; ++y)
        {
            int dx = x - circle_in.x;
            int dy = y - circle_in.y;
            int distance_squared = dx * dx + dy * dy;
            
            // inside circle
            if (distance_squared <= radius_squared)
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

EXPORT_SYMBOL void binarize_image(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    double thresh,
    double maxval
)
{
    cv::Mat buff = image_in;
    
    // gray scale image if it haven't
    if (image_in.channels() != 1)
        cv::cvtColor(buff, buff, cv::COLOR_BGR2GRAY);
    
    // set img to maximum contrast
    // only leave black & white pixels
    // usually, moon will be white after this conversion
    mr::apply_brightness_contrast(buff, buff, 0, 127);
    
    // set gray image to black & white only image by setting its threshold
    // opencv impl of threshold
    // dst[j] = src[j] > thresh ? maxval : 0;
    // using threshold to binarize img will rm all the branching when calculating img_brightness_perc
    // which make calculation much faster
    cv::threshold(buff, buff, thresh, maxval, cv::THRESH_BINARY);
    
    image_out = buff.clone();
}

EXPORT_SYMBOL void cut_ref_image_from_circle(
    const cv::Mat& image_in,
    cv::Mat& image_out,
    mr::Rectangle& rect_out,
    const mr::Circle& circle_in,
    const int padding
)
{
    int height = image_in.size[0];
    int width = image_in.size[1];
    
    int yStart = circle_in.y - circle_in.radius - padding;
    if (yStart < 0) yStart = 0;
    
    int yEnd = circle_in.y + circle_in.radius + padding;
    if (yEnd > height) yEnd = height;
    
    int xStart = circle_in.x - circle_in.radius - padding;
    if (xStart < 0) xStart = 0;
    
    int xEnd = circle_in.x + circle_in.radius + padding;
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
    const mr::Circle& circle_in,
    const int padding
)
{
    cut_ref_image_from_circle(image_in, image_out, rect_out, circle_in, padding);
    // copy content of image_out to a new cv::Mat
    image_out = image_out.clone();
}


EXPORT_SYMBOL void sync_img_size(const cv::Mat &primary, cv::Mat &secondary)
{
    mr::ImageShape primary_shape = mr::calc_image_shape(primary);
    mr::ImageShape secondary_shape = mr::calc_image_shape(secondary);
    double width_ratio = (
        static_cast<double>(primary_shape.width) /
        static_cast<double>(secondary_shape.width)
    );
    double height_ratio = (
        static_cast<double>(primary_shape.height) /
        static_cast<double>(secondary_shape.height)
    );
    
    float ratio_out;
    int param_width = -1;
    int param_height = -1;
    
    // sync size by width won't make height be out of bound
    if ((width_ratio * secondary_shape.height) <= primary_shape.height)
        param_width = primary_shape.width;
    // sync size by height won't make width be out of bound
    else if ((height_ratio * secondary_shape.width) <= primary_shape.width)
        param_height = primary_shape.height;
    
    mr::resize_with_aspect_ratio(
        secondary, secondary, ratio_out,
        param_width, param_height, -1
    );
}

EXPORT_SYMBOL void sync_img_size(const int primary_width, const int primary_height, cv::Mat& secondary)
{
    mr::ImageShape secondary_shape = mr::calc_image_shape(secondary);
    double width_ratio = (
        static_cast<double>(primary_width) /
        static_cast<double>(secondary_shape.width)
    );
    double height_ratio = (
        static_cast<double>(primary_height) /
        static_cast<double>(secondary_shape.height)
    );
    
    float ratio_out;
    int param_width = -1;
    int param_height = -1;
    
    // sync size by width won't make height be out of bound
    if ((width_ratio * secondary_shape.height) <= primary_height)
        param_width = primary_width;
    // sync size by height won't make width be out of bound
    else if ((height_ratio * secondary_shape.width) <= primary_width)
        param_height = primary_height;
    
    mr::resize_with_aspect_ratio(
        secondary, secondary, ratio_out,
        param_width, param_height, -1
    );
}

EXPORT_SYMBOL void sync_img_channel(const cv::Mat& primary, cv::Mat& secondary)
{
    // get number of channels
    int primary_channel = primary.channels();
    int secondary_channel = secondary.channels();
    cv::ColorConversionCodes color_code = cv::ColorConversionCodes::COLOR_COLORCVT_MAX;
    
    if (primary_channel == secondary_channel)
        return;
    
    // since image channel can only be 1, 3, & 4
    // all of them are less than 4 bits.
    // so we can merge two channel number into one int
    // and use it in following switch statement
    int choice = (secondary_channel << 4) | primary_channel;
    switch (choice)
    {
    // primary_channel == 1
    case 0x31: // secondary_channel == 3
        color_code = cv::COLOR_BGR2GRAY;
        break;
    case 0x41: // secondary_channel == 4
        color_code = cv::COLOR_BGRA2GRAY;
        break;
    
    // primary_channel == 3
    case 0x13: // secondary_channel == 1
        color_code = cv::COLOR_GRAY2BGR;
        break;
    case 0x43: // secondary_channel == 4
        color_code = cv::COLOR_BGRA2BGR;
        break;
    
    // primary_channel == 4
    case 0x14: // secondary_channel == 1
        color_code = cv::COLOR_GRAY2BGRA;
        break;
    case 0x34: // secondary_channel == 3
        color_code = cv::COLOR_BGR2BGRA;
        break;
    
    default:
        throw std::runtime_error("Cannot convert secondary channel with primary.");
        break;
    }
    
    cv::cvtColor(secondary, secondary, color_code);
}

EXPORT_SYMBOL void sync_img_channel(const int primary_channel, cv::Mat& secondary)
{
    // get number of channels
    int secondary_channel = secondary.channels();
    cv::ColorConversionCodes color_code = cv::ColorConversionCodes::COLOR_COLORCVT_MAX;
    
    if (primary_channel == secondary_channel)
        return;
    
    // since image channel can only be 1, 3, & 4
    // all of them are less than 4 bits.
    // so we can merge two channel number into one int
    // and use it in following switch statement
    int choice = (secondary_channel << 4) | primary_channel;
    switch (choice)
    {
    // primary_channel == 1
    case 0x31: // secondary_channel == 3
        color_code = cv::COLOR_BGR2GRAY;
        break;
    case 0x41: // secondary_channel == 4
        color_code = cv::COLOR_BGRA2GRAY;
        break;
    
    // primary_channel == 3
    case 0x13: // secondary_channel == 1
        color_code = cv::COLOR_GRAY2BGR;
        break;
    case 0x43: // secondary_channel == 4
        color_code = cv::COLOR_BGRA2BGR;
        break;
    
    // primary_channel == 4
    case 0x14: // secondary_channel == 1
        color_code = cv::COLOR_GRAY2BGRA;
        break;
    case 0x34: // secondary_channel == 3
        color_code = cv::COLOR_BGR2BGRA;
        break;
    
    default:
        throw std::runtime_error("Cannot convert secondary channel with primary.");
        break;
    }
    
    cv::cvtColor(secondary, secondary, color_code);
}


EXPORT_SYMBOL ImageChannels::ImageChannels(const cv::Mat& alpha)
{
    this->channels.resize(1);
    this->channels[0] = alpha.clone();
}
EXPORT_SYMBOL ImageChannels::ImageChannels(const cv::Mat& blue, const cv::Mat& green, const cv::Mat& red)
{
    this->channels.resize(3);
    this->channels[0] = blue.clone();
    this->channels[1] = green.clone();
    this->channels[2] = red.clone();
}
EXPORT_SYMBOL ImageChannels::ImageChannels(const cv::Mat& blue, const cv::Mat& green, const cv::Mat& red, const cv::Mat& alpha)
{
    this->channels.resize(4);
    this->channels[0] = blue.clone();
    this->channels[1] = green.clone();
    this->channels[2] = red.clone();
    this->channels[3] = alpha.clone();
}

EXPORT_SYMBOL const cv::Mat& ImageChannels::blue() const
{
    if (this->channel_num() == 3 || this->channel_num() == 4)
        return this->channels[0];
    else
        throw std::runtime_error("Channel Number Not Matching");
}
EXPORT_SYMBOL const cv::Mat& ImageChannels::green() const
{
    if (this->channel_num() == 3 || this->channel_num() == 4)
        return this->channels[1];
    else
        throw std::runtime_error("Channel Number Not Matching");
}
EXPORT_SYMBOL const cv::Mat& ImageChannels::red() const
{
    if (this->channel_num() == 3 || this->channel_num() == 4)
        return this->channels[2];
    else
        throw std::runtime_error("Channel Number Not Matching");
}
EXPORT_SYMBOL const cv::Mat& ImageChannels::alpha() const
{
    if (this->channel_num() == 1)
        return this->channels[0];
    else if (this->channel_num() == 4)
        return this->channels[3];
    else
        throw std::runtime_error("Channel Number Not Matching");
}

EXPORT_SYMBOL const size_t ImageChannels::channel_num() const
{
    return this->channels.size();
}

EXPORT_SYMBOL void split_img_channel(const cv::Mat& image_in, mr::ImageChannels& channels)
{
    int channel_num = image_in.channels();
    channels.channels.resize(channel_num);
    cv::split(image_in, channels.channels);
}

EXPORT_SYMBOL void merge_img_channel(const mr::ImageChannels& channels, cv::Mat& image_out)
{
    cv::merge(channels.channels, image_out);
}

EXPORT_SYMBOL void stack_imgs(
    const cv::Mat& background,
    const cv::Rect background_roi,
    const cv::Mat& foreground,
    cv::Mat& image_out,
    const float foreground_transparency,
    const cv::Vec4b* filter_px,
    const bool auto_resize
)
{
    // setup background & foreground, sync their size if needed.
    cv::Mat foreground_copy = foreground;
    bool foreground_out_of_bound = (
        foreground_copy.size().width > background_roi.width ||
        foreground_copy.size().height > background_roi.height
    );
    if (!auto_resize && foreground_out_of_bound)
        throw std::runtime_error("foreground size excess background_roi.");
    else if (auto_resize)
    {
        foreground_copy = foreground.clone();
        mr::sync_img_size(background_roi.width, background_roi.height, foreground_copy);
    }
    
    // update background_roi so we can center foreground inside background_roi
    int new_width = foreground_copy.size[1];
    int new_height = foreground_copy.size[0];
    int new_x = -1;
    int new_y = -1;
    
    // background_roi and foreground have same width, center by height
    if (new_width == background_roi.width)
    {
        new_x = background_roi.x;
        new_y = ((background_roi.height - new_height) / 2) + background_roi.y;
    }
    // background_roi and foreground have same height, center by width
    else if (new_height == background_roi.height)
    {
        new_x = ((background_roi.width - new_width) / 2) + background_roi.x;
        new_y = background_roi.y;
    }
    
    cv::Rect updated_background_roi = cv::Rect(new_x, new_y, new_width, new_height);
    
    // get number of channels
    int background_channel = background.channels();
    int foreground_channel = foreground_copy.channels();
    int max_channel = std::max<int>(background_channel, foreground_channel);
    
    // make image_out having background's data with max_channel
    image_out = background.clone();
    mr::sync_img_channel(max_channel, image_out);
    // copy a reference to image_out with updated roi so we can draw foreground on top of it
    cv::Mat background_with_roi = image_out(updated_background_roi);
    
    // setup a function to select specific channel value from a foreground pixel,
    // so we can use that to determine whether we should show foreground or not.
    // we setup this function here so we don't need to add extra branching when
    // we loop through all the pixels.
    std::function<float(const uchar*)> select_fore_px_val;
    
    // 4 channel foreground, checking alpha 
    if (foreground_channel == 4)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return static_cast<float>(fore_px[3]) / 255.0f;
        };
    }
    // 3 channel foreground, checking if fore_px(b,g,r) == (0,0,0)
    else if (foreground_channel == 3)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return (
                (static_cast<float>(fore_px[0]) / 255.0f) + 
                (static_cast<float>(fore_px[1]) / 255.0f) + 
                (static_cast<float>(fore_px[2]) / 255.0f) 
            );
        };
    }
    // 1 channel foreground, checking if fore_px(b) == 0
    else if (foreground_channel == 1)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return static_cast<float>(fore_px[0]) / 255.0f;
        };
    }
    
    // fill-in pixel values
    float transparency_factor = mr::clamp<float>(foreground_transparency, 0.0, 1.0);
    background_with_roi.forEach<cv::Vec4b>(
    [
        &background_with_roi, background_channel,
        &foreground_copy, foreground_channel,
        max_channel, transparency_factor,
        filter_px, &select_fore_px_val
    ]
    (cv::Vec4b&, const int* position)
    {
        // accessing image pixel using uchar* to the underlying cv::Mat,
        // so we can handle images with unknown channel number easier
        const uchar* fore_px = foreground_copy.ptr<uchar>(position);
        uchar* back_px = background_with_roi.ptr<uchar>(position);
        cv::Vec4b default_px(0, 0, 0, 255);
        
        
        // use a binary float to enable/disable foreground pixel
        float show_fore = 0.0f;
        // select foreground pixel value to compute show_fore
        show_fore = mr::make_binary_num<float>(select_fore_px_val(fore_px), 0.0f, 0.0f, 1.0f);
        
        // when fore_px value <= filter_px value, don't draw foreground
        if (filter_px)
        {
            uchar fore_px_under_filter_px = 1;
            for (int i = 0; i < foreground_channel; ++i)
                fore_px_under_filter_px = fore_px_under_filter_px & uchar(fore_px[i] <= filter_px->val[i]);
            if (fore_px_under_filter_px)
                show_fore = 0.0;
        }
        
        
        // if show_fore is 1.0, adding foreground pixel on top of background pixel,
        // otherwise nothing will be add on top of background pixel.
        for (int channel_idx = 0; channel_idx < max_channel; ++channel_idx)
        {
            // since we are accessing pixels via uchar* to the underlying cv::Mat,
            // we don't want to mess up adjacent pixels. so we need to constantly
            // check number of channels in background & foreground with current channel_idx.
            // if current channel_idx excess number of channel in background or foreground,
            // we use default_px(0,0,0,255) for the channel value
            uchar back_px_ch_val = back_px[channel_idx];
            if (channel_idx >= background_channel)
                back_px_ch_val = default_px[channel_idx];
            uchar fore_px_ch_val = fore_px[channel_idx];
            if (channel_idx >= foreground_channel)
                fore_px_ch_val = default_px[channel_idx];
            
            // when applying foreground transparency, we use transparency factor (0~1 float) multiply
            // with the pixel, decrease its value, and then add it on top of background pixel,
            // so two pixels' color will blend together.
            // in this case, we also need to adjust background pixel's transparency.
            // it will use (1 - transparency_factor) as its transparency factor.
            back_px[channel_idx] = static_cast<uchar>(
                (
                    back_px_ch_val * (1.0 - (transparency_factor * show_fore))
                ) +
                (
                    fore_px_ch_val * transparency_factor * show_fore
                )
            );
        }
    });
}

EXPORT_SYMBOL void stack_imgs_in_place(
    cv::Mat& background,
    const cv::Rect background_roi,
    const cv::Mat& foreground,
    const float foreground_transparency,
    const cv::Vec4b* filter_px,
    const bool auto_resize
)
{
    // setup background & foreground, sync their size if needed.
    cv::Mat foreground_copy = foreground;
    bool foreground_out_of_bound = (
        foreground_copy.size().width > background_roi.width ||
        foreground_copy.size().height > background_roi.height
    );
    if (!auto_resize && foreground_out_of_bound)
        throw std::runtime_error("foreground size excess background_roi.");
    else if (auto_resize)
    {
        foreground_copy = foreground.clone();
        mr::sync_img_size(background_roi.width, background_roi.height, foreground_copy);
    }
    
    // update background_roi so we can center foreground inside background_roi
    int new_width = foreground_copy.size[1];
    int new_height = foreground_copy.size[0];
    int new_x = -1;
    int new_y = -1;
    
    // background_roi and foreground have same width, center by height
    if (new_width == background_roi.width)
    {
        new_x = background_roi.x;
        new_y = ((background_roi.height - new_height) / 2) + background_roi.y;
    }
    // background_roi and foreground have same height, center by width
    else if (new_height == background_roi.height)
    {
        new_x = ((background_roi.width - new_width) / 2) + background_roi.x;
        new_y = background_roi.y;
    }
    
    cv::Rect updated_background_roi = cv::Rect(new_x, new_y, new_width, new_height);
    
    // get number of channels
    int background_channel = background.channels();
    int foreground_channel = foreground_copy.channels();
    
    // sync foreground channel with background if needed
    if (foreground_channel > background_channel)
    {
        mr::sync_img_channel(background_channel, foreground_copy);
        foreground_channel = background_channel;
    }
    // copy a reference to background with updated roi so we can draw foreground on top of it
    cv::Mat background_with_roi = background(updated_background_roi);
    
    // setup a function to select specific channel value from a foreground pixel,
    // so we can use that to determine whether we should show foreground or not.
    // we setup this function here so we don't need to add extra branching when
    // we loop through all the pixels.
    std::function<float(const uchar*)> select_fore_px_val;
    
    // 4 channel foreground, checking alpha 
    if (foreground_channel == 4)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return static_cast<float>(fore_px[3]) / 255.0f;
        };
    }
    // 3 channel foreground, checking if fore_px(b,g,r) == (0,0,0)
    else if (foreground_channel == 3)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return (
                (static_cast<float>(fore_px[0]) / 255.0f) + 
                (static_cast<float>(fore_px[1]) / 255.0f) + 
                (static_cast<float>(fore_px[2]) / 255.0f) 
            );
        };
    }
    // 1 channel foreground, checking if fore_px(b) == 0
    else if (foreground_channel == 1)
    {
        select_fore_px_val = [](const uchar* fore_px){
            return static_cast<float>(fore_px[0]) / 255.0f;
        };
    }
    
    // fill-in pixel values
    float transparency_factor = mr::clamp<float>(foreground_transparency, 0.0, 1.0);
    background_with_roi.forEach<cv::Vec4b>(
    [
        &background_with_roi, background_channel,
        &foreground_copy, foreground_channel,
        transparency_factor,
        filter_px, &select_fore_px_val
    ]
    (cv::Vec4b&, const int* position)
    {
        // accessing image pixel using uchar* to the underlying cv::Mat,
        // so we can handle images with unknown channel number easier
        const uchar* fore_px = foreground_copy.ptr<uchar>(position);
        uchar* back_px = background_with_roi.ptr<uchar>(position);
        cv::Vec4b default_px(0, 0, 0, 255);
        
        
        // use a binary float to enable/disable foreground pixel
        float show_fore = 0.0f;
        // select foreground pixel value to compute show_fore
        show_fore = mr::make_binary_num<float>(select_fore_px_val(fore_px), 0.0f, 0.0f, 1.0f);
        
        // when fore_px value <= filter_px value, don't draw foreground
        if (filter_px)
        {
            uchar fore_px_under_filter_px = 1;
            for (int i = 0; i < foreground_channel; ++i)
                fore_px_under_filter_px = fore_px_under_filter_px & uchar(fore_px[i] <= filter_px->val[i]);
            if (fore_px_under_filter_px)
                show_fore = 0.0;
        }
        
        
        // if show_fore is 1.0, adding foreground pixel on top of background pixel,
        // otherwise nothing will be add on top of background pixel.
        for (int channel_idx = 0; channel_idx < background_channel; ++channel_idx)
        {
            // since we are accessing pixels via uchar* to the underlying cv::Mat,
            // we don't want to mess up adjacent pixels. so we need to constantly
            // check number of channels in background & foreground with current channel_idx.
            // if current channel_idx excess number of channel in background or foreground,
            // we use default_px(0,0,0,255) for the channel value
            uchar back_px_ch_val = back_px[channel_idx];
            if (channel_idx >= background_channel)
                back_px_ch_val = default_px[channel_idx];
            uchar fore_px_ch_val = fore_px[channel_idx];
            if (channel_idx >= foreground_channel)
                fore_px_ch_val = default_px[channel_idx];
            
            // when applying foreground transparency, we use transparency factor (0~1 float) multiply
            // with the pixel, decrease its value, and then add it on top of background pixel,
            // so two pixels' color will blend together.
            // in this case, we also need to adjust background pixel's transparency.
            // it will use (1 - transparency_factor) as its transparency factor.
            back_px[channel_idx] = static_cast<uchar>(
                (
                    back_px_ch_val * (1.0 - (transparency_factor * show_fore))
                ) +
                (
                    fore_px_ch_val * transparency_factor * show_fore
                )
            );
        }
    });
}

}
