#include "../include/MoonRegistration/utils.hpp"

#include <opencv2/imgproc.hpp>

#include <fstream>


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

}
