#include <opencv2/imgproc.hpp>

#include <fstream>
#include <functional>

#include "MoonRegistration/utils.hpp"


namespace mr
{

EXPORT_SYMBOL ImageShape calc_image_shape(const cv::Mat& image_in)
{
    ImageShape shape;
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
    
    return shape;
}

EXPORT_SYMBOL cv::Vec3i round_vec3f(const cv::Vec3f& vec3)
{
    return cv::Vec3i(
        static_cast<int>(round(vec3[0])),
        static_cast<int>(round(vec3[1])),
        static_cast<int>(round(vec3[2]))
    );
}

EXPORT_SYMBOL bool file_exists(const std::string& filepath)
{
    std::ifstream input(filepath);
    return input.good();
}


EXPORT_SYMBOL void sync_img_size(const cv::Mat &primary, cv::Mat &secondary)
{
    cv::resize(secondary, secondary, primary.size());
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
    cv::Rect background_roi,
    const cv::Mat& foreground,
    cv::Mat& image_out,
    float foreground_transparency,
    const cv::Vec4b* filter_px
)
{
    // setup background & foreground, sync their size if needed.
    const cv::Mat& background_with_roi = background(background_roi);
    cv::Mat foreground_copy;
    if (foreground.size().width > background.size().width ||
        foreground.size().height > background.size().height)
    {
        foreground_copy = foreground.clone();
        mr::sync_img_size(background_with_roi, foreground_copy);
    }
    else
        foreground_copy = foreground;
    
    // get number of channels
    int background_channel = background_with_roi.channels();
    int foreground_channel = foreground_copy.channels();
    int max_channel = std::max<int>(background_channel, foreground_channel);
    
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
    
    // setup image_out & fill-in pixel values
    cv::Vec4b default_px(0, 0, 0, 255);
    float transparency_factor = mr::clamp<float>(foreground_transparency, 0.0, 1.0);
    image_out = cv::Mat::zeros(background_with_roi.size(), CV_MAKETYPE(CV_8U, max_channel));
    image_out.forEach<cv::Vec4b>(
    [
        &image_out,
        &background_with_roi, background_channel,
        &foreground_copy, foreground_channel,
        max_channel, transparency_factor,
        filter_px, default_px, &select_fore_px_val
    ]
    (cv::Vec4b& unused_param, const int* position)
    {
        // accessing image pixel using uchar* to the underlying cv::Mat,
        // so we can handle images with unknown channel number easier
        uchar* pixel = image_out.ptr<uchar>(position);
        const uchar* fore_px = foreground_copy.ptr<uchar>(position);
        const uchar* back_px = background_with_roi.ptr<uchar>(position);
        
        
        // use a binary float to enable/disable foreground pixel
        float show_fore = 0.0f;
        // select foreground pixel value to compute show_fore
        show_fore = mr::make_binary_num<float>(select_fore_px_val(fore_px), 0.0f, 0.0f, 1.0f);
        
        // when filter_px value matches fore_px value, don't draw foreground
        if (filter_px && memcmp(filter_px->val, fore_px, foreground_channel) == 0)
            show_fore = 0.0;
        
        
        // if show_fore is 1.0, adding foreground pixel on top of background pixel,
        // otherwise nothing will be add on top of background pixel.
        // 
        // when applying foreground transparency, we use transparency factor (0~1 float) multiply
        // with the pixel, decrease its value, and then add it on top of background pixel,
        // so two pixels' color will blend together.
        // in this case, we also need to adjust background pixel's transparency.
        // it will use (1 - transparency_factor) as its transparency factor.
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
            
            pixel[channel_idx] = static_cast<uchar>(
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
    cv::Rect background_roi,
    const cv::Mat& foreground,
    float foreground_transparency,
    const cv::Vec4b* filter_px
)
{
    // setup background & foreground, sync their size if needed.
    cv::Mat& background_with_roi = background(background_roi);
    cv::Mat foreground_copy;
    if (foreground.size().width > background.size().width ||
        foreground.size().height > background.size().height)
    {
        foreground_copy = foreground.clone();
        mr::sync_img_size(background_with_roi, foreground_copy);
    }
    else
        foreground_copy = foreground;
    
    // get number of channels
    int background_channel = background_with_roi.channels();
    int foreground_channel = foreground_copy.channels();
    // sync foreground channel with background if needed
    if (foreground_channel > background_channel)
    {
        // since image channel can only be 1, 3, & 4
        // all of them are less than 4 bits.
        // so we can merge two channel number into one int
        // and use it in following switch statement
        int choice = (foreground_channel << 4) | background_channel;
        switch (choice)
        {
        // background_channel == 1
        case 0b00110001: // foreground_channel == 3
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_BGR2GRAY);
            break;
        case 0b01000001: // foreground_channel == 4
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_BGRA2GRAY);
            break;
        
        // background_channel == 3
        case 0b00010011: // foreground_channel == 1
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_GRAY2BGR);
            break;
        case 0b01000011: // foreground_channel == 4
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_BGRA2BGR);
            break;
        
        // background_channel == 4
        case 0b00010100: // foreground_channel == 1
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_GRAY2BGRA);
            break;
        case 0b00110100: // foreground_channel == 3
            cv::cvtColor(foreground_copy, foreground_copy, cv::COLOR_BGR2BGRA);
            break;
        
        default:
            throw std::runtime_error("Cannot convert foreground channel with background.");
            break;
        }
        
        foreground_channel = background_channel;
    }
    
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
    
    // setup image_out & fill-in pixel values
    cv::Vec4b default_px(0, 0, 0, 255);
    float transparency_factor = mr::clamp<float>(foreground_transparency, 0.0, 1.0);
    background_with_roi.forEach<cv::Vec4b>(
    [
        &background_with_roi, background_channel,
        &foreground_copy, foreground_channel,
        transparency_factor,
        filter_px, default_px, &select_fore_px_val
    ]
    (cv::Vec4b& unused_param, const int* position)
    {
        // accessing image pixel using uchar* to the underlying cv::Mat,
        // so we can handle images with unknown channel number easier
        const uchar* fore_px = foreground_copy.ptr<uchar>(position);
        uchar* back_px = background_with_roi.ptr<uchar>(position);
        
        
        // use a binary float to enable/disable foreground pixel
        float show_fore = 0.0f;
        // select foreground pixel value to compute show_fore
        show_fore = mr::make_binary_num<float>(select_fore_px_val(fore_px), 0.0f, 0.0f, 1.0f);
        
        // when filter_px value matches fore_px value, don't draw foreground
        if (filter_px && memcmp(filter_px->val, fore_px, foreground_channel) == 0)
            show_fore = 0.0;
        
        
        // if show_fore is 1.0, adding foreground pixel on top of background pixel,
        // otherwise nothing will be add on top of background pixel.
        // 
        // when applying foreground transparency, we use transparency factor (0~1 float) multiply
        // with the pixel, decrease its value, and then add it on top of background pixel,
        // so two pixels' color will blend together.
        // in this case, we also need to adjust background pixel's transparency.
        // it will use (1 - transparency_factor) as its transparency factor.
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
