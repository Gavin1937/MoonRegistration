#pragma once

#include <opencv2/core/mat.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>

#include <vector>
#include <string>
#include <functional>

#include "MoonRegistration/macros.h"
#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"


namespace mr
{

EXPORT_SYMBOL typedef enum class RegistrationAlgorithms
{
    SIFT                               = 0x100,
    ORB                                = 0x101,
    AKAZE                              = 0x102,
    BRISK                              = 0x103,
    
#ifdef MR_HAVE_OPENCV_NONFREE
    // opencv non-free algorithms
    SURF_NONFREE                       = 0x200,
#endif
    
    EMPTY_ALGORITHM                    = 0x001,
    INVALID_ALGORITHM                  = 0x000
} RegistrationAlgorithms;

EXPORT_SYMBOL void create_f2d_detector(const mr::RegistrationAlgorithms algorithm, cv::Ptr<cv::Feature2D>& f2d_detector);

EXPORT_SYMBOL bool default_is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
);

EXPORT_SYMBOL typedef class MoonRegistrar
{
public:
    // constructors
    
    EXPORT_SYMBOL MoonRegistrar();
    
    EXPORT_SYMBOL MoonRegistrar(
        const std::string& user_image_path,
        const std::string& model_image_path,
        const mr::RegistrationAlgorithms& algorithm
    );
    
    EXPORT_SYMBOL MoonRegistrar(
        const std::vector<unsigned char>& user_image_binary,
        const std::vector<unsigned char>& model_image_binary,
        const mr::RegistrationAlgorithms& algorithm
    );
    
    EXPORT_SYMBOL MoonRegistrar(
        const cv::Mat& user_image,
        const cv::Mat& model_image,
        const mr::RegistrationAlgorithms& algorithm
    );
    
    
    // setters
    
    // (re)init user_image & model_image with filepath
    EXPORT_SYMBOL void update_images(
        const std::string& user_image_path,
        const std::string& model_image_path
    );
    
    // (re)init user_image & model_image with image binary
    EXPORT_SYMBOL void update_images(
        const std::vector<unsigned char>& user_image_binary,
        const std::vector<unsigned char>& model_image_binary
    );
    
    // (re)init user_image & model_image with cv::Mat
    EXPORT_SYMBOL void update_images(const cv::Mat& user_image, const cv::Mat& model_image);
    
    
    // (re)init f2d_detector with pre-defined algorithms
    EXPORT_SYMBOL void update_f2d_detector(const mr::RegistrationAlgorithms& algorithm);
    
    // (re)init f2d_detector with custom cv::Ptr<cv::Feature2D>
    EXPORT_SYMBOL void update_f2d_detector(const cv::Ptr<cv::Feature2D>& f2d_detector);
    
    // update homography_matrix
    EXPORT_SYMBOL void update_homography_matrix(const cv::Mat& homography_matrix);
    
    // update good_keypoint_matches
    EXPORT_SYMBOL void update_good_keypoint_matches(const std::vector<std::vector<cv::DMatch>>& good_keypoint_matches);
    
    
    // getters
    
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
    
    
    // registration
    
    // Compute keypoints, good keypoint matches, and homography matrix.
    // You must call this function before using any of the transform_* or draw_* functions.
    // 
    // Parameters:
    //   - knn_k: int, k value for cv::BFMatcher::knnMatch(). default 2
    //   - good_match_ratio: float, n m distance ratio for filtering good matches. default 0.7
    //   - find_homography_method: int, method for cv::findHomography()
    //     The following methods are possible:
    //       - **0** - a regular method using all the points, i.e., the least squares method
    //       - RANSAC - RANSAC-based robust method
    //       - LMEDS - Least-Median robust method
    //       - RHO - PROSAC-based robust method
    //     default to RANSAC
    //   - find_homography_ransac_reproj_threshold: double, RansacReprojThreshold for cv::findHomograph().
    //     default 5.0
    EXPORT_SYMBOL void compute_registration(
        const int knn_k = 2,
        const float good_match_ratio = 0.7,
        const int find_homography_method = cv::RANSAC,
        const double find_homography_ransac_reproj_threshold = 5.0
    );
    
    // Using computed homography_matrix to apply a perspective transformation to image_in.
    // "Rotate" image_in using homography_matrix.
    // 
    // Parameters:
    //   - image_in: input image
    //   - image_out: output image
    EXPORT_SYMBOL void transform_image(const cv::Mat& image_in, cv::Mat& image_out);
    
    // Using computed homography_matrix inverse to apply a perspective transformation to image_in.
    // "Rotate" image_in using homography_matrix inverse.
    // 
    // Parameters:
    //   - image_in: input image
    //   - image_out: output image
    EXPORT_SYMBOL void transform_image_inverse(const cv::Mat& image_in, cv::Mat& image_out);
    
    // Same as mr::transform_image(), but using user_image as image_in.
    // Using computed homography_matrix to apply a perspective transformation to user_image.
    // "Rotate" user_image to match model_image's perspective.
    // 
    // Parameters:
    //   - image_out: output image
    EXPORT_SYMBOL void transform_user_image(cv::Mat& image_out);
    
    // Wrapper around mr::transform_image_inverse(), transform any layer image.
    // Using computed homography_matrix to transform layer_image_in in the perspective of user_image.
    // "Rotate" layer_image_in to match user_image's perspective.
    // 
    // Parameters:
    //   - layer_image_in: input image
    //   - layer_image_out: output image
    // 
    // Note:
    //   - input layer image's size will be sync with user_image
    EXPORT_SYMBOL void transform_layer_image(const cv::Mat& layer_image_in, cv::Mat& layer_image_out);
    
    
    // draw result image
    
    // Draw user_image and model_image side-by-side with good keypoint matches,
    // and output to image_out
    // 
    // Parameters:
    //   - image_out: output image
    EXPORT_SYMBOL void draw_matched_keypoints(cv::Mat& image_out);
    
    // Draw user_image after transforming in the perspective of model_image,
    // and output to image_out's red channel
    // 
    // Parameters:
    //   - image_out: output image
    //   - transformed_image_in: a replacement of red transformed image
    //     if this is an empty cv::Mat, this function will generate one using user_image.
    //     otherwise, this function will use user provided image as red image.
    //     default is an empty cv::Mat()
    EXPORT_SYMBOL void draw_red_transformed_user_image(cv::Mat& image_out, const cv::Mat& transformed_image_in = cv::Mat());
    
    // Draw model_image, and output to image_out's green channel
    // 
    // Parameters:
    //   - image_out: output image
    EXPORT_SYMBOL void draw_green_model_image(cv::Mat& image_out);
    
    // Stack red image and green image generated by
    // mr::draw_red_transformed_user_image() and mr::draw_green_model_image()
    // and write to image_out as its red and green channel
    // 
    // Parameters:
    //   - image_out: output image
    //   - transformed_image_in: a replacement of red transformed image
    //     if this is an empty cv::Mat, this function will generate one using user_image.
    //     otherwise, this function will use user provided image as red image.
    //     default is an empty cv::Mat()
    EXPORT_SYMBOL void draw_stacked_red_green_image(cv::Mat& image_out, const cv::Mat& transformed_image_in = cv::Mat());
    
    // Transform input layer image to the perspective of user_image
    // using mr::transform_layer_image(), and draws it on top of
    // user_image, then write to image_out.
    // 
    // Parameters:
    //   - layer_image_in: input layer image
    //   - image_out: output image
    //   - layer_image_transparency: a 0~1 float percentage changing layer image's transparency,
    //     default 1.0
    //   - filter_px: pointer to cv::Vec4b pixel with pixel value to filter in the layer image,
    //     set it to NULL if you don't need it, default NULL
    // 
    // Note:
    //   - this function is designed to work with different number of color channels
    //   - when using filter_px, we only read number of elements corresponding to
    //     number of channels in layer image. if layer image has 3 channels, we only read
    //     first 3 elements of filter_px. rest of elements will be ignored.
    EXPORT_SYMBOL void draw_layer_image(
        const cv::Mat& layer_image_in,
        cv::Mat& image_out,
        const float layer_image_transparency = 1.0,
        const cv::Vec4b* filter_px = NULL
    );
    
public:
    // Following public members of mr::MoonRegistrar are function pointers
    // They are functions handling different steps in mr::MoonRegistrar::compute_registration()
    // You can modify them to further customize how mr::MoonRegistrar::compute_registration() works
    // All the function pointers are default to default_... functions defined in "registrar.hpp"
    
    // A function pointer to a ratio test function that determines
    // whether a pair of keypoints are good matches. It runs in a loop
    // of all the elements in matches returned by cv::BFMatcher::knnMatch()
    // 
    // function signature:
    //   bool (
    //       const cv::DMatch& m,
    //       const cv::DMatch& n,
    //       const float good_match_ratio,
    //       const cv::KeyPoint& user_kpt,
    //       const cv::KeyPoint& model_kpt,
    //       const cv::Mat& user_image,
    //       const cv::Mat& model_image
    //   )
    // 
    // function parameters:
    //   - m: matches element[0]
    //   - n: matches element[1]
    //   - good_match_ratio: the same good_match_ratio pass into mr::MoonRegistrar::compute_registration(),
    //     a float of n m distance ratio for filtering good matches. default 0.7
    //   - user_kpt: user_keypoints[m.queryIdx]
    //   - model_kpt: user_keypoints[m.trainIdx]
    //   - user_image: a const reference to user_image
    //   - model_image: a const reference to model_image
    // 
    // function pointer default points to mr::default_is_good_match()
    std::function<bool(
        const cv::DMatch&,
        const cv::DMatch&,
        const float,
        const cv::KeyPoint&,
        const cv::KeyPoint&,
        const cv::Mat&,
        const cv::Mat&
    )> is_good_match = mr::default_is_good_match;
    
private: // helper functions
    void __validate_registrar();
    void __validate_image_matrix();
    
private:
    cv::Ptr<cv::Feature2D> f2d_detector;
    cv::Mat homography_matrix;
    std::vector<std::vector<cv::DMatch>> good_keypoint_matches;
    cv::Size image_size;
    // user
    cv::Mat user_image;
    std::vector<cv::KeyPoint> user_keypoints;
    // model
    cv::Mat model_image;
    std::vector<cv::KeyPoint> model_keypoints;
    
} MoonRegistrar;

}
