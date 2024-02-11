#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <exception>

#include "MoonRegistration/MoonRegistrar/registrar.hpp"
#include "MoonRegistration/utils.hpp"

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
    this->user_image = cv::imread(user_image_path, cv::IMREAD_COLOR);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imread(model_image_path, cv::IMREAD_COLOR);
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
    this->user_image = cv::imdecode(user_image_binary, cv::IMREAD_COLOR);
    if (this->user_image.empty())
        throw std::runtime_error("Input User Image is empty");
    this->model_image = cv::imdecode(model_image_binary, cv::IMREAD_COLOR);
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
        if (mn[0].distance < good_match_ratio * mn[1].distance)
        {
            this->good_keypoint_matches.push_back(std::vector<cv::DMatch>({mn[0]}));
            
            tmp_user_keypoints_pt2f.push_back(this->user_keypoints[mn[0].queryIdx].pt);
            tmp_model_keypoints_pt2f.push_back(this->model_keypoints[mn[0].trainIdx].pt);
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

EXPORT_SYMBOL void MoonRegistrar::registrate_image(const cv::Mat& image_in, cv::Mat& image_out)
{
    this->__validate_registrar();
    cv::warpPerspective(image_in, image_out, this->homography_matrix, image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::registrate_image_inverse_homography(const cv::Mat& image_in, cv::Mat& image_out)
{
    this->__validate_registrar();
    cv::warpPerspective(image_in, image_out, this->homography_matrix.inv(), image_in.size());
}

EXPORT_SYMBOL void MoonRegistrar::registrate_user_image(cv::Mat& image_out)
{
    this->__validate_registrar();
    this->registrate_image(this->user_image, image_out);
}

EXPORT_SYMBOL void MoonRegistrar::transform_layer_image(const cv::Mat& layer_image_in, cv::Mat& layer_image_out)
{
    layer_image_out = layer_image_in.clone();
    mr::sync_img_size(this->user_image, layer_image_out);
    this->registrate_image_inverse_homography(layer_image_out, layer_image_out);
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
        this->registrate_user_image(transformed_image);
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
        this->registrate_user_image(red);
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
    const cv::Vec3b* filter_bgr
)
{
    this->__validate_registrar();
    
    // pre-process layer image
    cv::Mat processed_layer_image;
    this->transform_layer_image(layer_image_in, processed_layer_image);
    
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
    
    // merge
    float transparency_factor = mr::clamp<float>(layer_image_transparency, 0.0, 1.0);
    image_out = cv::Mat::zeros(this->image_size, CV_8UC3);
    image_out.forEach<cv::Vec3b>(
        [this, &transparent_layer_image, filter_bgr, transparency_factor]
        (cv::Vec3b& pixel, const int* position)
        {
            cv::Vec4b front_layer_bgra = transparent_layer_image.at<cv::Vec4b>(position);
            
            // use front_layer alpha as a binary float to enable/disable front_layer pixel
            float on_layer = mr::make_binary_num<float>(static_cast<float>(front_layer_bgra[3])/255.0f, 0.0f, 1.0f);
            
            // when filter_bgr value matches front_layer_bgra value, don't draw layer
            if (filter_bgr && memcmp(filter_bgr->val, front_layer_bgra.val, 3) == 0)
                on_layer = 0.0;
            
            // if on_layer is 1, adding front_layer pixel on top of user_image pixel,
            // otherwise nothing will be add on top of user_image pixel.
            // 
            // when applying layer transparency, we use transparency factor (0~1 float) multiply
            // with the pixel, decrease its RGB value, and then add it on top of background pixel,
            // so two pixels' color will blend together.
            // in this case, we also need to adjust background pixel's transparency.
            // it will use (1 - transparency_factor) as its transparency factor.
            pixel = (
                (
                    this->user_image.at<cv::Vec3b>(position) * (1.0 - (transparency_factor * on_layer))
                ) +
                (
                    cv::Vec3b(front_layer_bgra.val) * transparency_factor * on_layer
                )
            );
        }
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