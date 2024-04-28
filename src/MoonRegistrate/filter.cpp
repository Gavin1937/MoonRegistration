#include <opencv2/core.hpp>

#include <vector>
#include <unordered_map>

#include "MoonRegistration/MoonRegistrate/filter.hpp"
#include "MoonRegistration/utils.hpp"


namespace mr
{

EXPORT_SYMBOL bool filter_by_lowes_ratio_test(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float ratio
)
{
    return (m.distance < ratio * n.distance);
}

EXPORT_SYMBOL bool filter_by_ignore_edge_kp(
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image,
    const float radius_guess_ratio
)
{
    // assuming input user_image and model_image have a moon at center and
    // the edge of the image is really close edge of the moon.
    // Then, get the image radius by: (image.width / 2) * ratio
    // Get the image circle center point: (image.width / 2), (image.height / 2)
    // Calculate the euclidean distance of image circle center and user_kpt
    
    cv::Size user_size = user_image.size();
    double user_radius_guess = (user_size.width / 2.0f) * radius_guess_ratio;
    cv::Point2f user_center_pt((user_size.width / 2.0f), (user_size.height / 2.0f));
    double user_distance = cv::norm(cv::Mat(user_kpt.pt), cv::Mat(user_center_pt), cv::NORM_L2);
    
    cv::Size model_size = model_image.size();
    double model_radius_guess = (model_size.width / 2.0f) * radius_guess_ratio;
    cv::Point2f model_center_pt((model_size.width / 2.0f), (model_size.height / 2.0f));
    double model_distance = cv::norm(cv::Mat(model_kpt.pt), cv::Mat(model_center_pt), cv::NORM_L2);
    
    // Finally, ignore all the keypoints outside of image circle that we guess
    return (
        (user_distance <= user_radius_guess) &&
        (model_distance <= model_radius_guess)
    );
}


struct KeyPointHolder
{
    cv::KeyPoint user;
    cv::KeyPoint model;
};
std::unordered_map<int,std::vector<KeyPointHolder>> user_filter_table;
std::unordered_map<int,std::vector<KeyPointHolder>> model_filter_table;
// helper function for filter_by_ignore_close_kp()
int ignore_close_kp_hash_coordinate(
    const cv::KeyPoint& kpt,
    const cv::Mat& image,
    float hash_coordinate_ratio
)
{
    // we can "hash" a 2d point by integer divide it by a value
    // e.g. if we have point a(1, 2), b(2, 3), c(11, 12)
    // we can divide all of them by 10 and get:
    // a(0, 0), b(0, 0), c(1, 1)
    // 
    // After division, we "blur" these points, and now point a & b are on
    // same location.
    // Finally, we can "hash" these points by converting them to 1d point.
    // We can use this algorithm to "blur" points, and points who are close
    // to each other will have them same coordinate after "blur", the final
    // 1d coordinate is our "hash sum".
    
    float x = kpt.pt.x;
    float y = kpt.pt.y;
    // determine the constant for integer division using image width & height
    float x_ratio = image.size().width * hash_coordinate_ratio;
    float y_ratio = image.size().height * hash_coordinate_ratio;
    int new_x = static_cast<int>(x / x_ratio);
    int new_y = static_cast<int>(y / y_ratio);
    int w = static_cast<int>(image.size().width / x_ratio);
    int h = static_cast<int>(image.size().height / y_ratio);
    return (w * (new_y - 1) + new_x);

}
bool ignore_close_kp_handle_table_dup(
    std::unordered_map<int,std::vector<KeyPointHolder>>& table,
    const int hash
)
{
    // randomly sample 3 elements out of table[hash]
    // make src = tmp[0], dst = tmp[1], comp = tmp[2]
    std::vector<KeyPointHolder> tmp;
    mr::sample_vector(3, table[hash], tmp);
    
    // We first use keypoints src and dst to calculate a ratio of euclidean distance.
    // So we get: ratio1 = distance(src.user, dst.user) / distance(src.model, dst.model)
    // If keypoint comp isn't really close to src & dst, then the distance ratio of
    // comp with src & dst should have a significant difference.
    // That is: ratio2 = distance(src.user, comp.user) / distance(src.model, comp.model)
    // ratio1 and ratio2 should have a significant difference.
    // We can do the same thing with ratio3,
    // ratio3 = distance(dst.user, comp.user) / distance(dst.model, comp.model)
    // ratio3 and ratio 2 should have a significant difference.
    
    double user_dist = mr::norm_kp(tmp[0].user, tmp[1].user, cv::NORM_L2);
    double model_dist = mr::norm_kp(tmp[0].model, tmp[1].model, cv::NORM_L2);
    double dist_ratio = user_dist / model_dist;
    
    double src_comp_user_dist = mr::norm_kp(tmp[0].user, tmp[2].user, cv::NORM_L2);
    double src_comp_model_dist = mr::norm_kp(tmp[0].model, tmp[2].model, cv::NORM_L2);
    double src_comp_dist_ratio = src_comp_user_dist / src_comp_model_dist;
    
    double dst_comp_user_dist = mr::norm_kp(tmp[1].user, tmp[2].user, cv::NORM_L2);
    double dst_comp_model_dist = mr::norm_kp(tmp[1].model, tmp[2].model, cv::NORM_L2);
    double dst_comp_dist_ratio = dst_comp_user_dist / dst_comp_model_dist;
    
    return (
        (fabs(dist_ratio - src_comp_dist_ratio) <= 0.1) &&
        (fabs(dist_ratio - dst_comp_dist_ratio) <= 0.1)
    );
}
EXPORT_SYMBOL bool filter_by_ignore_close_kp(
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image,
    const float hash_coordinate_ratio,
    const bool is_first_time
)
{
    if (is_first_time)
    {
        user_filter_table.clear();
        model_filter_table.clear();
    }
    
    // hash coordinate
    int user_hash = ignore_close_kp_hash_coordinate(
        user_kpt, user_image, hash_coordinate_ratio
    );
    int model_hash = ignore_close_kp_hash_coordinate(
        model_kpt, model_image, hash_coordinate_ratio
    );
    
    // insert user keypoints into user table
    KeyPointHolder holder = {user_kpt, model_kpt};
    bool user_hash_exist = (user_filter_table.find(user_hash) == user_filter_table.end());
    user_filter_table[user_hash].push_back(holder);
    if (user_hash_exist && user_filter_table[user_hash].size() >= 3)
        return ignore_close_kp_handle_table_dup(user_filter_table, user_hash);
    
    // insert model keypoints into model table
    bool model_hash_exist = (model_filter_table.find(model_hash) == model_filter_table.end());
    model_filter_table[model_hash].push_back(holder);
    if (model_hash_exist && user_filter_table[model_hash].size() >= 3)
        return ignore_close_kp_handle_table_dup(model_filter_table, model_hash);
    
    return true;
}

};