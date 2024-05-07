// This demo is built on top of OpenCV's demo application for cv::VideoCapture class
// https://docs.opencv.org/4.x/d8/dfe/classcv_1_1VideoCapture.html
// 
// For the ease of demonstration, I will cover the code relating to MoonRegistration inside:
// ==================== MoonRegistration ====================
// ...
// ==================== MoonRegistration ====================

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <stdio.h>

// MoonRegistration library api
#include "MoonRegistration/mrapi.hpp"


int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "Usage: ./MoonRegistrar_live_registration [MODEL_IMAGE] [LAYER_IMAGE] [CAMERA_DEVICE_ID]\n";
        return 0;
    }
    
    // ==================== MoonRegistration ====================
    
    std::string MODEL_IMAGE = argv[1];
    std::string LAYER_IMAGE = argv[2];
    
    cv::Mat model_img = imread(MODEL_IMAGE, cv::IMREAD_UNCHANGED);
    cv::Mat layer_img = imread(LAYER_IMAGE, cv::IMREAD_UNCHANGED);
    mr::MoonRegistrar registrar;
    registrar.update_f2d_detector(mr::RegistrationAlgorithms::SIFT);
    
    // Currently, SURF algorithm works the best, but its disabled by default.
    // Learn more in BUILDING.md at "About OpenCV versions & modules" section.
    // registrar.update_f2d_detector(mr::RegistrationAlgorithms::SURF_NONFREE);
    
    // ==================== MoonRegistration ====================
    
    cv::Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    cv::VideoCapture cap;
    // open the default camera using default API
    int deviceID;             // 0 = open default camera
    try
    {
        deviceID = std::stoi(std::string(argv[3]));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to convert \"" << argv[3] << "\" to integer. " << e.what() << '\n';
    }
    // OR advance usage: select any API backend
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID, apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    std::cout
        << "Start grabbing" << std::endl
        << "Press any key to terminate" << std::endl;
    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        
        
        // ==================== MoonRegistration ====================
        
        try
        {
            // detect moon location from current frame
            mr::MoonDetector detector(frame);
            mr::Circle circle = detector.detect_moon();
            if (!mr::is_valid_circle(circle))
                throw std::runtime_error("Cannot find moon circle.");
            
            // cut an image from the circle as our user image
            cv::Mat user_img;
            mr::Rectangle rect_out;
            mr::cut_image_from_circle(frame, user_img, rect_out, circle);
            registrar.update_images(user_img, model_img);
            
            // compute moon image registration with updated images
            registrar.compute_registration();
            
            // transform layer image to match the perspective of user image
            cv::Mat transformed_layer;
            registrar.transform_layer_image(layer_img, transformed_layer);
            
            // draw transformed layer image on top of current frame, so we can show it
            cv::Rect roi = mr::rectangle_to_roi(rect_out);
            float layer_image_transparency = 1.0f;
            const cv::Vec4b filter_px = cv::Vec4b(0,0,0,255);
            mr::stack_imgs_in_place(frame, roi, transformed_layer, layer_image_transparency, &filter_px);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        // ==================== MoonRegistration ====================
        
        
        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}