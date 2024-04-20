#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "MoonRegistration/c_mrapi/utils_c_api.h"
#include "MoonRegistration/c_mrapi/internal_c_api.hpp"


extern "C"
{

EXPORT_SYMBOL mat_ptr mrc_read_image_from_filepath(const char* filepath)
{
    cv::Mat* ptr = new cv::Mat();
    cv::Mat tmp = cv::imread(std::string(filepath), cv::IMREAD_UNCHANGED);
    tmp.copyTo(*ptr);
    return mrc_mat_to_ptr(*ptr);
}

EXPORT_SYMBOL mat_ptr mrc_read_image_from_bytes(const unsigned char* bytes, const size_t byte_length)
{
    cv::Mat* ptr = new cv::Mat();
    cv::Mat tmp = cv::imdecode(std::vector<unsigned char>(bytes, bytes+byte_length), cv::IMREAD_UNCHANGED);
    tmp.copyTo(*ptr);
    return mrc_mat_to_ptr(*ptr);
}

EXPORT_SYMBOL bool mrc_write_image_to_filepath(mat_ptr image, const char* filepath)
{
    cv::Mat& mat_image = mrc_ptr_to_mat(image);
    return cv::imwrite(std::string(filepath), mat_image);
}

EXPORT_SYMBOL bool mrc_write_image_to_bytes(mat_ptr image, const char* fileext, unsigned char** buffer, size_t* buffer_length)
{
    cv::Mat& mat_image = mrc_ptr_to_mat(image);
    std::vector<unsigned char> tmp;
    bool result = cv::imencode(std::string(fileext), mat_image, tmp);
    *buffer_length = tmp.size();
    *buffer = new unsigned char[*buffer_length];
    std::move(tmp.data(), tmp.data()+tmp.size(), *buffer);
    return result;
}

EXPORT_SYMBOL void mrc_destroy_mat_ptr(mat_ptr ptr)
{
    cv::Mat* original = reinterpret_cast<cv::Mat*>(ptr);
    original->~Mat();
    delete original;
}
}