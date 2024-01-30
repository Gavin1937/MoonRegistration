#include "../../include/MoonRegistration/MoonRegistrar/registrar.hpp"
#include "../../include/MoonRegistration/utils.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include <exception>
#include <iostream>

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
    for (auto v : vec)
        out << v << "\n";
    return out;
}

std::ostream& operator<<(std::ostream& out, const cv::KeyPoint& kp)
{
    out << kp.pt;
    return out;
}

namespace mr
{

EXPORT_SYMBOL void create_f2d_detector(const mr::RegistrationAlgorithms algorithm, cv::Ptr<cv::Feature2D>& f2d_detector)
{
    switch (algorithm)
    {
    case mr::RegistrationAlgorithms::SIFT:
        f2d_detector = cv::SIFT::create();
        break;
    case mr::RegistrationAlgorithms::ORB:
        f2d_detector = cv::ORB::create();
        break;
    case mr::RegistrationAlgorithms::AKAZE:
        f2d_detector = cv::AKAZE::create();
        break;
    case mr::RegistrationAlgorithms::BRISK:
        f2d_detector = cv::BRISK::create();
        break;
    
    // opencv non-free algorithms
    // case mr::RegistrationAlgorithms::NONFREE_SURF:
    //     f2d_detector = cv::xfeatures2d::SURF::create();
    //     break;
    
    case mr::RegistrationAlgorithms::INVALID_ALGORITHM:
        throw std::runtime_error("Invalid Algorithm");
        break;
    default:
        throw std::runtime_error("Invalid Algorithm");
        break;
    }
}

EXPORT_SYMBOL MoonRegistrar::MoonRegistrar()
{
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(const std::string& user_image_path, const std::string& model_image_path, const mr::RegistrationAlgorithms& algorithm)
{
    this->update_images(user_image_path, model_image_path);
    this->update_f2d_detector(algorithm);
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(const std::vector<unsigned char>& user_image_binary, const std::vector<unsigned char>& model_image_binary, const mr::RegistrationAlgorithms& algorithm)
{
    this->update_images(user_image_binary, model_image_binary);
    this->update_f2d_detector(algorithm);
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(const cv::Mat& user_image, const cv::Mat& model_image, const mr::RegistrationAlgorithms& algorithm)
{
    this->update_images(user_image, model_image);
    this->update_f2d_detector(algorithm);
}

EXPORT_SYMBOL void MoonRegistrar::update_images(const std::string& user_image_path, const std::string& model_image_path)
{
    this->user_image = cv::imread(user_image_path);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imread(model_image_path);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    
    // pre-processing
    mr::sync_img_size(this->user_image, this->model_image);
}
EXPORT_SYMBOL void MoonRegistrar::update_images(const std::vector<unsigned char>& user_image_binary, const std::vector<unsigned char>& model_image_binary)
{
    this->user_image = cv::imdecode(user_image_binary, cv::IMREAD_COLOR);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imdecode(model_image_binary, cv::IMREAD_COLOR);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    
    // pre-processing
    mr::sync_img_size(this->user_image, this->model_image);
}
EXPORT_SYMBOL void MoonRegistrar::update_images(const cv::Mat& user_image, const cv::Mat& model_image)
{
    this->user_image = user_image.clone();
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = model_image.clone();
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    
    // pre-processing
    mr::sync_img_size(this->user_image, this->model_image);
}

EXPORT_SYMBOL void MoonRegistrar::update_f2d_detector(const mr::RegistrationAlgorithms& algorithm)
{
    mr::create_f2d_detector(algorithm, this->f2d_detector);
}
EXPORT_SYMBOL void MoonRegistrar::update_f2d_detector(const cv::Ptr<cv::Feature2D>& f2d_detector)
{
    this->f2d_detector = f2d_detector;
}

EXPORT_SYMBOL void MoonRegistrar::compute_registration()
{
    cv::Mat gray_user_image, gray_model_image;
    cv::cvtColor(this->user_image, gray_user_image, cv::COLOR_BGR2GRAY);
    cv::cvtColor(this->model_image, gray_model_image, cv::COLOR_BGR2GRAY);
    
    // compute keypoints & descriptors
    cv::Mat tmp_user_descriptors, tmp_model_descriptors;
    this->f2d_detector->detectAndCompute(
        gray_user_image, cv::noArray(), this->user_keypoints, tmp_user_descriptors
    );
    this->f2d_detector->detectAndCompute(
        gray_model_image, cv::noArray(), this->model_keypoints, tmp_model_descriptors
    );
    
    // matching keypoints
    cv::BFMatcher bf_matcher;
    std::vector<std::vector<cv::DMatch>> matches;
    bf_matcher.knnMatch(tmp_user_descriptors, tmp_model_descriptors, matches, 2);
    
    this->good_keypoint_matches.reserve(matches.size());
    std::vector<cv::Point2f> tmp_user_keypoints_pt2f;
    tmp_user_keypoints_pt2f.reserve(matches.size());
    std::vector<cv::Point2f> tmp_model_keypoints_pt2f;
    tmp_model_keypoints_pt2f.reserve(matches.size());
    
    // filter good matches
    for (auto mn : matches)
    {
        if (mn[0].distance < 0.7 * mn[1].distance)
        {
            this->good_keypoint_matches.push_back(std::vector<cv::DMatch>({mn[0]}));
            
            tmp_user_keypoints_pt2f.push_back(this->user_keypoints[mn[0].queryIdx].pt);
            tmp_model_keypoints_pt2f.push_back(this->model_keypoints[mn[0].trainIdx].pt);
        }
    }
    tmp_user_keypoints_pt2f.shrink_to_fit();
    tmp_model_keypoints_pt2f.shrink_to_fit();
    this->good_keypoint_matches.shrink_to_fit();
    
    // compute homography matrix
    this->homography_matrix = cv::findHomography(tmp_user_keypoints_pt2f, tmp_model_keypoints_pt2f, cv::RANSAC, 5.0);
    if (this->homography_matrix.empty())
        throw std::runtime_error("Cannot find Homography Matrix");
}

EXPORT_SYMBOL void MoonRegistrar::draw_matched_keypoints(cv::Mat& image_out)
{
    cv::drawMatches(
        this->user_image, this->user_keypoints,
        this->model_image, this->model_keypoints,
        this->good_keypoint_matches,
        image_out,
        cv::Scalar::all(-1),
        cv::Scalar::all(-1),
        std::vector<std::vector<char>>(),
        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
    );
}

EXPORT_SYMBOL void MoonRegistrar::registrate_image(const cv::Mat& image_in, cv::Mat& image_out)
{
    cv::warpPerspective(image_in, image_out, this->homography_matrix, image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::registrate_image_inverse_homography(const cv::Mat& image_in, cv::Mat& image_out)
{
    cv::warpPerspective(image_in, image_out, this->homography_matrix.inv(), image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::registrate_user_image(cv::Mat& image_out)
{
    this->registrate_image(this->user_image, image_out);
}

EXPORT_SYMBOL void MoonRegistrar::draw_red_transformed_user_image(cv::Mat& image_out, const cv::Mat& transformed_image_in)
{
    cv::Mat transformed_image;
    if (transformed_image_in.empty())
        this->registrate_user_image(transformed_image);
    else
        transformed_image = transformed_image_in;
    
    if (transformed_image.size() != this->user_image.size() || transformed_image.size() != this->model_image.size())
        throw std::runtime_error("Transformed Image size not match with original image size");
    
    cv::cvtColor(transformed_image, transformed_image, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->user_image.size(), CV_8UC1),  // B
            cv::Mat::zeros(this->user_image.size(), CV_8UC1),  // G
            transformed_image                                  // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_green_model_image(cv::Mat& image_out)
{
    cv::Mat gray_model_image;
    cv::cvtColor(this->model_image, gray_model_image, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->user_image.size(), CV_8UC1),  // B
            gray_model_image,                                  // G
            cv::Mat::zeros(this->user_image.size(), CV_8UC1)   // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_stacked_red_green_image(cv::Mat& image_out, const cv::Mat& transformed_image_in)
{
    cv::Mat green;
    cv::cvtColor(this->model_image, green, cv::COLOR_BGR2GRAY);
    
    cv::Mat red;
    if (transformed_image_in.empty())
        this->registrate_user_image(red);
    else
        red = transformed_image_in;
    if (red.size() != this->user_image.size() || red.size() != this->model_image.size())
        throw std::runtime_error("Transformed Image size not match with original image size");
    cv::cvtColor(red, red, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->user_image.size(), CV_8UC1),  // B
            green,                                             // G
            red                                                // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_layer_image(const cv::Mat& layer_image_in, cv::Mat& image_out, const float layer_image_transparency)
{
    // pre-process layer image
    cv::Mat processed_layer_image = layer_image_in.clone();
    mr::sync_img_size(this->user_image, processed_layer_image);
    this->registrate_image_inverse_homography(processed_layer_image, processed_layer_image);
    
    // create alpha channel for layer image
    cv::Mat gray_processed_layer_image, alpha;
    cv::cvtColor(processed_layer_image, gray_processed_layer_image, cv::COLOR_BGR2GRAY);
    cv::threshold(gray_processed_layer_image, alpha, 0, 255, cv::THRESH_BINARY);
    
    // merge into a transparent layer image
    cv::Mat transparent_layer_image;
    mr::ImageChannels processed_layer_image_channels;
    mr::split_img_channel(processed_layer_image, processed_layer_image_channels);
    processed_layer_image_channels.channels.push_back(alpha);
    mr::merge_img_channel(processed_layer_image_channels, transparent_layer_image);
    
    // merge 
    cv::Mat all_alpha_layer_image;
    mr::merge_img_channel(mr::ImageChannels(alpha, alpha, alpha), all_alpha_layer_image);
    cv::Mat front_layer;
    cv::cvtColor(transparent_layer_image, front_layer, cv::COLOR_BGRA2BGR);
    
    image_out = cv::Mat::zeros(this->user_image.size(), CV_8UC3);
    cv::Vec3b zero_pixel(0, 0, 0);
    for (int row = 0; row < image_out.rows; ++row)
    {
        for (int col = 0; col < image_out.cols; ++col)
        {
            cv::Point p(col, row);
            if (all_alpha_layer_image.at<cv::Vec3b>(p) == zero_pixel)
                image_out.at<cv::Vec3b>(p) = this->user_image.at<cv::Vec3b>(p);
            else
                image_out.at<cv::Vec3b>(p) = front_layer.at<cv::Vec3b>(p);
        }
    }
}

}