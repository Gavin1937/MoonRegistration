# Assuming you already have MoonRegistration package installed in your python environment
import MoonRegistration as mr
import numpy
import cv2
from sys import argv

print(f'MoonRegistration Library Version: {mr.__version__}')


# Following functions are different steps in function mr.MoonRegistrate.MoonRegistrar.compute_registration()
# You can modify their behavior by rewriting them and setting them to mr.MoonRegistrate.MoonRegistrar object.
# Checkout "registrar.cpp" for detailed implementation of these step functions
# ==================================================

def is_good_match(
    m:cv2.DMatch,
    n:cv2.DMatch,
    good_match_ratio:float,
    user_kpt:cv2.KeyPoint,
    model_kpt:cv2.KeyPoint,
    user_image:numpy.ndarray,
    model_image:numpy.ndarray
) -> bool:
    return mr.MoonRegistrate.default_is_good_match(
        m, n,
        good_match_ratio,
        user_kpt, model_kpt,
        user_image, model_image
    )

# ==================================================


if len(argv) < 2:
    print('Usage: python3 ./MoonRegistrate_advance.py [USER_IMAGE] [MODEL_IMAGE] [LAYER_IMAGE]')
    exit(0)

user_image = argv[1]
model_image = argv[2]
layer_image = argv[3]
print("User Image Path:", user_image)
print("Model Image Path:", model_image)
print("Layer Image Path:", layer_image)

try:
    algorithm = mr.MoonRegistrate.RegistrationAlgorithms.ORB
    # Currently, SURF algorithm works the best, but its disabled by default.
    # Learn more in BUILDING.md at "About OpenCV versions & modules" section.
    # algorithm = mr.MoonRegistrate.RegistrationAlgorithms.SURF_NONFREE
    
    
    # # read image directly from filepath
    # registrar = mr.MoonRegistrate.MoonRegistrar(user_image, model_image, algorithm)
    
    
    # # read image from bytes
    # with open(user_image, 'rb') as file_in:
    #     user_image_buffer = file_in.read()
    
    # with open(model_image, 'rb') as file_in:
    #     model_image_buffer = file_in.read()
    
    # registrar = mr.MoonRegistrate.MoonRegistrar(user_image_buffer, model_image_buffer, algorithm)
    
    
    # # read image from cv2.MatLike|numpy.ndarray object
    # # fill-in pixel data to cv_image...
    # mat_user_image = cv2.imread(user_image, cv2.IMREAD_UNCHANGED)
    # mat_model_image = cv2.imread(model_image, cv2.IMREAD_UNCHANGED)
    # registrar = mr.MoonRegistrate.MoonRegistrar(mat_user_image, mat_model_image, algorithm)
    
    
    # initialize/update image or algorithm afterward
    registrar = mr.MoonRegistrate.MoonRegistrar()
    
    registrar.update_images(user_image, model_image)
    # registrar.update_images(user_image_buffer, model_image_buffer)
    # registrar.update_images(mat_user_image, mat_model_image)
    
    # f2d_detector is cv::Ptr<cv::Feature2D> object,
    # a pointer to the detection algorithm object.
    registrar.update_f2d_detector(algorithm)
    
    
    
    # To run image registration algorithm on moon images:
    
    # 0) customize registrar.compute_registration()
    
    # Following functions of mr.MoonRegistrate.MoonRegistrar will set function pointers in the class.
    # Those pointers are functions handling different steps in mr.MoonRegistrate.MoonRegistrar.compute_registration()
    # You can modify them to further customize how mr.MoonRegistrate.MoonRegistrar.compute_registration() works
    # All the function pointers are default to default_... functions defined in "registrar.hpp"
    
    registrar.set_is_good_match(is_good_match)
    
    # 1) compute moon image registration
    registrar.compute_registration()
    
    # 1.a) get intermediate data & image 
    
    registrar.get_homography_matrix()
    registrar.get_user_image()
    registrar.get_user_keypoints()
    registrar.get_model_image()
    registrar.get_model_keypoints()
    registrar.get_good_keypoint_matches()
    
    # 1.b) use computed intermediate data to generate outer image
    
    # input_image:numpy.ndarray = cv2.imread('/path/to/image')
    
    # output_image:numpy.ndarray = registrar.transform_image(input_image)
    # output_image:numpy.ndarray = registrar.transform_image_inverse(input_image)
    # output_image:numpy.ndarray = registrar.transform_user_image()
    # output_image:numpy.ndarray = registrar.transform_layer_image(cv2.imread(layer_image))
    
    # 2) draw result images
    matched_keypoints:numpy.ndarray = registrar.draw_matched_keypoints()
    cv2.imwrite("./output/01_matched_keypoints.png", matched_keypoints)
    
    transform_user_image:numpy.ndarray = registrar.transform_user_image()
    cv2.imwrite("./output/02_registrate_user_image.png", transform_user_image)
    
    green_model_image:numpy.ndarray = registrar.draw_green_model_image()
    cv2.imwrite("./output/03_green_model_image.png", green_model_image)
    
    red_transformed_user_image:numpy.ndarray = registrar.draw_red_transformed_user_image()
    cv2.imwrite("./output/04_red_transformed_user_image.png", red_transformed_user_image)
    
    stacked_red_green_image:numpy.ndarray = registrar.draw_stacked_red_green_image()
    cv2.imwrite("./output/05_stacked_red_green_image.png", stacked_red_green_image)
    
    layer_image_in = cv2.imread(layer_image, cv2.IMREAD_UNCHANGED)
    layer_image_transparency = 1.0
    filter_px = [0,0,0,255]
    layer_image_out:numpy.ndarray = registrar.draw_layer_image(layer_image_in, layer_image_transparency, filter_px)
    cv2.imwrite("./output/06_layer_image.png", layer_image_out)
    
except Exception as err:
    print(f'Exception: {err}')

