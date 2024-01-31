#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/features2d.hpp>

#include <vector>
#include <string>

#include "../macros.h"


namespace mr
{

EXPORT_SYMBOL typedef enum class RegistrationAlgorithms
{
    SIFT   = 0x100,
    ORB    = 0x101,
    AKAZE  = 0x102,
    BRISK  = 0x103,
    
    // opencv non-free algorithms
    // NONFREE_SURF   = 0x200,
    
    INVALID_ALGORITHM = 0x000
} RegistrationAlgorithms;

EXPORT_SYMBOL void create_f2d_detector(const mr::RegistrationAlgorithms algorithm, cv::Ptr<cv::Feature2D>& f2d_detector);

EXPORT_SYMBOL typedef class MoonRegistrar
{
public:
    EXPORT_SYMBOL MoonRegistrar();
    
    EXPORT_SYMBOL MoonRegistrar(const std::string& user_image_path, const std::string& model_image_path, const mr::RegistrationAlgorithms& algorithm);
    
    EXPORT_SYMBOL MoonRegistrar(const std::vector<unsigned char>& user_image_binary, const std::vector<unsigned char>& model_image_binary, const mr::RegistrationAlgorithms& algorithm);
    
    EXPORT_SYMBOL MoonRegistrar(const cv::Mat& user_image, const cv::Mat& model_image, const mr::RegistrationAlgorithms& algorithm);
    
    
    // (re)init user_image & model_image with filepath
    EXPORT_SYMBOL void update_images(const std::string& user_image_path, const std::string& model_image_path);
    
    // (re)init user_image & model_image with image binary
    EXPORT_SYMBOL void update_images(const std::vector<unsigned char>& user_image_binary, const std::vector<unsigned char>& model_image_binary);
    
    // (re)init user_image & model_image with cv::Mat
    EXPORT_SYMBOL void update_images(const cv::Mat& user_image, const cv::Mat& model_image);
    
    
    // (re)init f2d_detector with pre-defined algorithms
    EXPORT_SYMBOL void update_f2d_detector(const mr::RegistrationAlgorithms& algorithm);
    
    // (re)init f2d_detector with custom cv::Ptr<cv::Feature2D>
    EXPORT_SYMBOL void update_f2d_detector(const cv::Ptr<cv::Feature2D>& f2d_detector);
    
    EXPORT_SYMBOL const cv::Mat& get_homography_matrix() const
    {
        return this->homography_matrix;
    }
    
    EXPORT_SYMBOL const cv::Mat& get_user_image() const
    {
        return this->user_image;
    }
    
    EXPORT_SYMBOL const std::vector<cv::KeyPoint>& get_user_keypoints() const
    {
        return this->user_keypoints;
    }
    
    EXPORT_SYMBOL const cv::Mat& get_model_image() const
    {
        return this->model_image;
    }
    
    EXPORT_SYMBOL const std::vector<cv::KeyPoint>& get_model_keypoints() const
    {
        return this->model_keypoints;
    }
    
    EXPORT_SYMBOL const std::vector<std::vector<cv::DMatch>>& get_good_keypoint_matches() const
    {
        return this->good_keypoint_matches;
    }
    
    
    // moon registration
    EXPORT_SYMBOL void compute_registration();
    
    EXPORT_SYMBOL void registrate_image(const cv::Mat& image_in, cv::Mat& image_out);
    
    EXPORT_SYMBOL void registrate_image_inverse_homography(const cv::Mat& image_in, cv::Mat& image_out);
    
    EXPORT_SYMBOL void registrate_user_image(cv::Mat& image_out);
    
    EXPORT_SYMBOL void transform_layer_image(const cv::Mat& layer_image_in, cv::Mat& layer_image_out);
    
    
    // draw result image
    EXPORT_SYMBOL void draw_matched_keypoints(cv::Mat& image_out);
    
    EXPORT_SYMBOL void draw_red_transformed_user_image(cv::Mat& image_out, const cv::Mat& transformed_image_in = cv::Mat());
    
    EXPORT_SYMBOL void draw_green_model_image(cv::Mat& image_out);
    
    EXPORT_SYMBOL void draw_stacked_red_green_image(cv::Mat& image_out, const cv::Mat& transformed_image_in = cv::Mat());
    
    EXPORT_SYMBOL void draw_layer_image(const cv::Mat& layer_image_in, cv::Mat& image_out, const float layer_image_transparency = 1.0);
    
private:
    cv::Ptr<cv::Feature2D> f2d_detector;
    cv::Mat homography_matrix;
    std::vector<std::vector<cv::DMatch>> good_keypoint_matches;
    // user
    cv::Mat user_image;
    std::vector<cv::KeyPoint> user_keypoints;
    // model
    cv::Mat model_image;
    std::vector<cv::KeyPoint> model_keypoints;
    
} MoonRegistrar;

}
