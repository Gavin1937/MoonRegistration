#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <exception>

#include "MoonRegistration/MoonRegistrate/registrar.hpp"
#include "MoonRegistration/imgprocess.hpp"

// include MoonRegistration header first, so we get MR_HAVE_OPENCV_NONFREE macro
#ifdef MR_HAVE_OPENCV_NONFREE
#include <opencv2/xfeatures2d.hpp>
#endif


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
    
#ifdef MR_ENABLE_OPENCV_NONFREE
    // opencv non-free algorithms
    case mr::RegistrationAlgorithms::SURF_NONFREE:
        f2d_detector = cv::xfeatures2d::SURF::create();
        break;
#endif
    
    case mr::RegistrationAlgorithms::EMPTY_ALGORITHM:
        break;
    case mr::RegistrationAlgorithms::INVALID_ALGORITHM:
        throw std::runtime_error("Invalid Registration Algorithm");
        break;
    default:
        throw std::runtime_error("Invalid Registration Algorithm");
        break;
    }
}

EXPORT_SYMBOL bool default_is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
)
{
    return (m.distance < good_match_ratio * n.distance);
}

EXPORT_SYMBOL MoonRegistrar::MoonRegistrar()
{
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(
    const std::string& user_image_path,
    const std::string& model_image_path,
    const mr::RegistrationAlgorithms& algorithm
)
{
    this->update_images(user_image_path, model_image_path);
    this->update_f2d_detector(algorithm);
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(
    const std::vector<unsigned char>& user_image_binary,
    const std::vector<unsigned char>& model_image_binary,
    const mr::RegistrationAlgorithms& algorithm
)
{
    this->update_images(user_image_binary, model_image_binary);
    this->update_f2d_detector(algorithm);
}
EXPORT_SYMBOL MoonRegistrar::MoonRegistrar(
    const cv::Mat& user_image,
    const cv::Mat& model_image,
    const mr::RegistrationAlgorithms& algorithm
)
{
    this->update_images(user_image, model_image);
    this->update_f2d_detector(algorithm);
}


EXPORT_SYMBOL void MoonRegistrar::update_images(
    const std::string& user_image_path,
    const std::string& model_image_path
)
{
    this->user_image = cv::imread(user_image_path, cv::IMREAD_UNCHANGED);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imread(model_image_path, cv::IMREAD_UNCHANGED);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    
    // pre-processing
    mr::sync_img_size(this->user_image, this->model_image);
    this->image_size = this->user_image.size();
}
EXPORT_SYMBOL void MoonRegistrar::update_images(
    const std::vector<unsigned char>& user_image_binary,
    const std::vector<unsigned char>& model_image_binary
)
{
    this->user_image = cv::imdecode(user_image_binary, cv::IMREAD_UNCHANGED);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imdecode(model_image_binary, cv::IMREAD_UNCHANGED);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    
    // pre-processing
    mr::sync_img_size(this->user_image, this->model_image);
    this->image_size = this->user_image.size();
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
    this->image_size = this->user_image.size();
}

EXPORT_SYMBOL void MoonRegistrar::update_f2d_detector(const mr::RegistrationAlgorithms& algorithm)
{
    mr::create_f2d_detector(algorithm, this->f2d_detector);
}
EXPORT_SYMBOL void MoonRegistrar::update_f2d_detector(const cv::Ptr<cv::Feature2D>& f2d_detector)
{
    this->f2d_detector = f2d_detector;
}


EXPORT_SYMBOL void MoonRegistrar::compute_registration(
    const int knn_k,
    const float good_match_ratio,
    const int find_homography_method,
    const double find_homography_ransac_reproj_threshold
)
{
    if (this->f2d_detector.empty())
        throw std::runtime_error("Empty Feature2D detector");
    
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
    bf_matcher.knnMatch(tmp_user_descriptors, tmp_model_descriptors, matches, knn_k);
    
    // only allocate 75% of matches buffer, assuming good_matches are 75% of matches
    int keypoints_buffer_size = static_cast<int>(matches.size()*0.75);
    this->good_keypoint_matches.reserve(keypoints_buffer_size);
    std::vector<cv::Point2f> tmp_user_keypoints_pt2f;
    tmp_user_keypoints_pt2f.reserve(keypoints_buffer_size);
    std::vector<cv::Point2f> tmp_model_keypoints_pt2f;
    tmp_model_keypoints_pt2f.reserve(keypoints_buffer_size);
    
    // filter good matches
    for (auto mn : matches)
    {
        const cv::KeyPoint& user_kpt = this->user_keypoints[mn[0].queryIdx];
        const cv::KeyPoint& model_kpt = this->model_keypoints[mn[0].trainIdx];
        bool flag = this->is_good_match(
            mn[0], mn[1],
            good_match_ratio,
            user_kpt, model_kpt,
            this->user_image, this->model_image
        );
        if (flag)
        {
            this->good_keypoint_matches.push_back(std::vector<cv::DMatch>({mn[0]}));
            
            tmp_user_keypoints_pt2f.push_back(user_kpt.pt);
            tmp_model_keypoints_pt2f.push_back(model_kpt.pt);
        }
    }
    this->good_keypoint_matches.shrink_to_fit();
    
    // compute homography matrix
    if (tmp_user_keypoints_pt2f.size() < 4 || tmp_model_keypoints_pt2f.size() < 4)
        throw std::runtime_error("No enough keypoints for finding homography matrix");
    this->homography_matrix = cv::findHomography(
        tmp_user_keypoints_pt2f,
        tmp_model_keypoints_pt2f,
        find_homography_method,
        find_homography_ransac_reproj_threshold
    );
    if (this->homography_matrix.empty())
        throw std::runtime_error("Cannot find Homography Matrix");
}

EXPORT_SYMBOL void MoonRegistrar::transform_image(const cv::Mat& image_in, cv::Mat& image_out)
{
    this->__validate_registrar();
    cv::warpPerspective(image_in, image_out, this->homography_matrix, image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::transform_image_inverse(const cv::Mat& image_in, cv::Mat& image_out)
{
    this->__validate_registrar();
    cv::warpPerspective(image_in, image_out, this->homography_matrix.inv(), image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::transform_user_image(cv::Mat& image_out)
{
    this->__validate_registrar();
    this->transform_image(this->user_image, image_out);
}

EXPORT_SYMBOL void MoonRegistrar::transform_layer_image(const cv::Mat& layer_image_in, cv::Mat& layer_image_out)
{
    layer_image_out = layer_image_in.clone();
    mr::sync_img_size(this->user_image, layer_image_out);
    this->transform_image_inverse(layer_image_out, layer_image_out);
}


EXPORT_SYMBOL void MoonRegistrar::draw_matched_keypoints(cv::Mat& image_out)
{
    this->__validate_registrar();
    
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

EXPORT_SYMBOL void MoonRegistrar::draw_red_transformed_user_image(cv::Mat& image_out, const cv::Mat& transformed_image_in)
{
    this->__validate_registrar();
    
    cv::Mat transformed_image;
    if (transformed_image_in.empty())
        this->transform_user_image(transformed_image);
    else
        transformed_image = transformed_image_in;
    
    if (transformed_image.size() != this->image_size)
        throw std::runtime_error("Transformed Image size not match with original image size");
    
    cv::cvtColor(transformed_image, transformed_image, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->image_size, CV_8UC1),  // B
            cv::Mat::zeros(this->image_size, CV_8UC1),  // G
            transformed_image                           // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_green_model_image(cv::Mat& image_out)
{
    this->__validate_registrar();
    
    cv::Mat gray_model_image;
    cv::cvtColor(this->model_image, gray_model_image, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->image_size, CV_8UC1),  // B
            gray_model_image,                           // G
            cv::Mat::zeros(this->image_size, CV_8UC1)   // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_stacked_red_green_image(cv::Mat& image_out, const cv::Mat& transformed_image_in)
{
    this->__validate_registrar();
    
    cv::Mat green;
    cv::cvtColor(this->model_image, green, cv::COLOR_BGR2GRAY);
    
    cv::Mat red;
    if (transformed_image_in.empty())
        this->transform_user_image(red);
    else
        red = transformed_image_in;
    if (red.size() != this->image_size)
        throw std::runtime_error("Transformed Red Image size not match with original image size");
    cv::cvtColor(red, red, cv::COLOR_BGR2GRAY);
    
    cv::merge(
        std::vector<cv::Mat>({
            cv::Mat::zeros(this->image_size, CV_8UC1),  // B
            green,                                      // G
            red                                         // R
        }),
        image_out
    );
}

EXPORT_SYMBOL void MoonRegistrar::draw_layer_image(
    const cv::Mat& layer_image_in,
    cv::Mat& image_out,
    const float layer_image_transparency,
    const cv::Vec4b* filter_px
)
{
    this->__validate_registrar();
    
    // pre-process layer image
    cv::Mat processed_layer_image;
    this->transform_layer_image(layer_image_in, processed_layer_image);
    
    
    // create our own alpha channel for layer image if needed
    cv::Mat transparent_layer_image;
    if (processed_layer_image.channels() < 4)
    {
        // create alpha channel for layer image
        cv::Mat gray_processed_layer_image, alpha;
        cv::cvtColor(processed_layer_image, gray_processed_layer_image, cv::COLOR_BGR2GRAY);
        cv::threshold(gray_processed_layer_image, alpha, 0, 255, cv::THRESH_BINARY);
        
        // merge alpha into a layer image
        mr::ImageChannels processed_layer_image_channels;
        mr::split_img_channel(processed_layer_image, processed_layer_image_channels);
        processed_layer_image_channels.channels.push_back(alpha);
        mr::merge_img_channel(processed_layer_image_channels, transparent_layer_image);
    }
    else // channels == 4
        transparent_layer_image = processed_layer_image;
    
    
    mr::stack_imgs(
        this->user_image,
        cv::Rect({0,0}, this->image_size),
        transparent_layer_image,
        image_out,
        layer_image_transparency,
        filter_px
    );
}


// private helper functions
void MoonRegistrar::__validate_registrar()
{
    if (this->f2d_detector.empty())
        throw std::runtime_error("Empty Feature2D detector");
    if (this->user_image.empty() || this->model_image.empty())
        throw std::runtime_error("Empty user_image or model_image");
    if (this->homography_matrix.empty() || this->user_keypoints.empty() || this->model_keypoints.empty() || this->good_keypoint_matches.empty())
        throw std::runtime_error("Empty homography_matrix, good_keypoint_matches, user_keypoints, or model_keypoints");
}

}