#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>


// MoonRegistration library api
#include "MoonRegistration/mrapi.hpp"


// Following functions are different steps in function mr::MoonRegistrar::compute_registration()
// You can modify their behavior by rewriting them and setting them to mr::MoonRegistrar object.
// Checkout "registrar.cpp" for detailed implementation of these step functions
// ==================================================

bool is_good_match(
    const cv::DMatch& m,
    const cv::DMatch& n,
    const float good_match_ratio,
    const cv::KeyPoint& user_kpt,
    const cv::KeyPoint& model_kpt,
    const cv::Mat& user_image,
    const cv::Mat& model_image
)
{
    return mr::default_is_good_match(
        m, n,
        good_match_ratio,
        user_kpt, model_kpt,
        user_image, model_image
    );
}

// ==================================================


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "Usage: ./MoonRegistrar_advance [USER_IMAGE] [MODEL_IMAGE] [LAYER_IMAGE]\n";
        return 0;
    }
    
    std::cout << "MoonRegistration Library Version: " << mr::version() << "\n";
    
    
    const std::string user_image(argv[1]);
    const std::string model_image(argv[2]);
    const std::string layer_image(argv[3]);
    std::cout << "User Image Path: " << user_image << "\n";
    std::cout << "Model Image Path: " << model_image << "\n";
    std::cout << "Layer Image Path: " << layer_image << "\n";
    
    try
    {
        mr::RegistrationAlgorithms algorithm = mr::RegistrationAlgorithms::ORB;
        // Currently, SURF algorithm works the best, but its disabled by default.
        // Learn more in BUILDING.md at "About OpenCV non-free modules" section.
        // mr::RegistrationAlgorithms algorithm = mr::RegistrationAlgorithms::SURF_NONFREE;
        
        
        // // read image directly from filepath
        // mr::MoonRegistrar registrar(user_image, model_image, algorithm);
        
        
        // // read image from bytes
        // std::ifstream file_in1(user_image, std::ios::binary);
        // file_in1.seekg(0, std::ios::end);
        // size_t file_size1 = file_in1.tellg();
        // std::vector<unsigned char> user_image_buffer(file_size1);
        // file_in1.seekg(0);
        // file_in1.read((char*)(user_image_buffer.data()), file_size1);
        // file_in1.close();
        
        // std::ifstream file_in2(model_image, std::ios::binary);
        // file_in2.seekg(0, std::ios::end);
        // size_t file_size2 = file_in2.tellg();
        // std::vector<unsigned char> model_image_buffer(file_size2);
        // file_in2.seekg(0);
        // file_in2.read((char*)(model_image_buffer.data()), file_size2);
        // file_in2.close();
        
        // mr::MoonRegistrar registrar(user_image_buffer, model_image_buffer, algorithm);
        
        
        // // read image from cv::Mat object
        // cv::Mat mat_user_image, mat_model_image;
        // // fill-in pixel data to cv_image...
        // mat_user_image = cv::imread(user_image, cv::IMREAD_UNCHANGED);
        // mat_model_image = cv::imread(model_image, cv::IMREAD_UNCHANGED);
        // mr::MoonRegistrar registrar(mat_user_image, mat_model_image, algorithm);
        
        
        // initialize/update image or algorithm afterward
        mr::MoonRegistrar registrar;
        
        registrar.update_images(user_image, model_image);
        // registrar.update_images(user_image_buffer, model_image_buffer);
        // registrar.update_images(mat_user_image, mat_model_image);
        
        // f2d_detector is cv::Ptr<cv::Feature2D> object,
        // a pointer to the detection algorithm object.
        registrar.update_f2d_detector(algorithm);
        
        // // you also can update it using cv::Ptr<cv::Feature2D>
        // cv::Ptr<cv::Feature2D> my_detector;
        // registrar.update_f2d_detector(my_detector);
        
        
        
        // To run image registration algorithm on moon images:
        
        // 0) customize registrar.compute_registration()
        
        // Following public members of mr::MoonRegistrar are function pointers
        // They are functions handling different steps in mr::MoonRegistrar::compute_registration()
        // You can modify them to further customize how mr::MoonRegistrar::compute_registration() works
        // All the function pointers are default to default_... functions defined in "registrar.hpp"
        
        registrar.is_good_match = is_good_match;
        
        // 1) compute moon image registration
        registrar.compute_registration();
        
        // 1.a) get intermediate data & image 
        
        registrar.get_homography_matrix();
        registrar.get_user_image();
        registrar.get_user_keypoints();
        registrar.get_model_image();
        registrar.get_model_keypoints();
        registrar.get_good_keypoint_matches();
        
        // 1.b) use computed intermediate data to generate outer image
        
        // cv::Mat input_image, output_image;
        
        // registrar.transform_image(input_image, output_image);
        // registrar.transform_image_inverse(input_image, output_image);
        // registrar.transform_user_image(output_image);
        // registrar.transform_layer_image(cv::imread(layer_image), output_image);
        
        // 2) draw result images
        cv::Mat matched_keypoints;
        registrar.draw_matched_keypoints(matched_keypoints);
        cv::imwrite("./output/01_matched_keypoints.png", matched_keypoints);
        
        cv::Mat transform_user_image;
        registrar.transform_user_image(transform_user_image);
        cv::imwrite("./output/02_registrate_user_image.png", transform_user_image);
        
        cv::Mat green_model_image;
        registrar.draw_green_model_image(green_model_image);
        cv::imwrite("./output/03_green_model_image.png", green_model_image);
        
        cv::Mat red_transformed_user_image;
        registrar.draw_red_transformed_user_image(red_transformed_user_image);
        cv::imwrite("./output/04_red_transformed_user_image.png", red_transformed_user_image);
        
        cv::Mat stacked_red_green_image;
        registrar.draw_stacked_red_green_image(stacked_red_green_image);
        cv::imwrite("./output/05_stacked_red_green_image.png", stacked_red_green_image);
        
        cv::Mat layer_image_in = cv::imread(layer_image, cv::IMREAD_COLOR);
        cv::Mat layer_image_out;
        registrar.draw_layer_image(layer_image_in, layer_image_out);
        cv::imwrite("./output/06_layer_image.png", layer_image_out);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Exception: " << error.what() << "\n";
        return -1;
    }
    
    return 0;
}
