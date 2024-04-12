#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>


// MoonRegistration library api MoonRegistrate module
#include "MoonRegistration/MoonRegistrate.hpp"


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "Usage: ./MoonRegistrate_basic [USER_IMAGE] [MODEL_IMAGE] [LAYER_IMAGE]\n";
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
        
        // 1) compute moon image registration
        registrar.compute_registration();
        
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
        
        cv::Mat layer_image_in = cv::imread(layer_image, cv::IMREAD_UNCHANGED);
        cv::Mat layer_image_out;
        float layer_image_transparency = 1.0f;
        const cv::Vec4b filter_px = cv::Vec4b(0,0,0,255);
        registrar.draw_layer_image(layer_image_in, layer_image_out, layer_image_transparency, &filter_px);
        cv::imwrite("./output/06_layer_image.png", layer_image_out);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Exception: " << error.what() << "\n";
        return -1;
    }
    
    return 0;
}
