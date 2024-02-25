# Assuming you already have MoonRegistration package installed in your python environment
import numpy as np
import MoonRegistration as mr
from sys import argv
from pathlib import Path


# Following functions are different steps in function mr.MoonDetect.MoonDetector.moon_detect()
# You can modify their behavior by rewriting them and setting them using mr.MoonDetect.MoonDetector.set_... functions.
# Checkout "detector.cpp" for detailed implementation of these step functions
# ==================================================

def preprocess_steps(image_in:np.ndarray) -> tuple:
    return mr.MoonDetect.default_preprocess_steps(image_in)

def param_init(
    image_shape:mr.imgprocess.ImageShape, max_iteration:int, circle_threshold:int,
    dp:float, minDist:float,
    minRadiusRate:float, minRadius:int,
    maxRadiusRate:float, maxRadius:int,
    param1:float, param2:float
) -> tuple:
    return mr.MoonDetect.default_param_init(
        image_shape, max_iteration, circle_threshold,
        dp, minDist,
        minRadiusRate, minRadius,
        maxRadiusRate, maxRadius,
        param1, param2
    )

def iteration_param_update(
    iteration:int, image_brightness_perc:float, image_shape:mr.imgprocess.ImageShape,
    max_iteration:int, circle_threshold:int, dp:float, minDist:float,
    minRadiusRate:float, minRadius:int,
    maxRadiusRate:float, maxRadius:int,
    param1:float, param2:float
) -> tuple:
    return mr.MoonDetect.default_iteration_param_update(
        iteration, image_brightness_perc, image_shape,
        max_iteration, circle_threshold, dp, minDist,
        minRadiusRate, minRadius,
        maxRadiusRate, maxRadius,
        param1, param2
    )

def iteration_circle_select(iteration:int, image_in:np.ndarray, detected_circles:np.ndarray) -> mr.shapes.Circle:
    return mr.MoonDetect.default_iteration_circle_select(iteration, image_in, detected_circles)

def coordinate_remap(result_list:list, resize_ratio:float) -> mr.shapes.Circle:
    return mr.MoonDetect.default_coordinate_remap(result_list, resize_ratio)

# ==================================================


print(f'MoonRegistration Library Version: {mr.__version__}')


if len(argv) < 2:
    print('Usage: python3 ./MoonDetector_advance.py [IMAGE_FOLDER]')
    exit(0)


folder = Path(argv[1])
print(f'Folder Path: {folder}')
for dirEntry in folder.rglob('*'):
    if not dirEntry.is_file():
        continue
    try:
        # read image directly from filepath
        detector = mr.MoonDetect.MoonDetector(str(dirEntry))
        
        # # read image from bytes
        # with open(dirEntry, 'rb') as file_in:
        #     buffer = file_in.read()
        # detector = mr.MoonDetect.MoonDetector(buffer)
        
        # # read image from cv2.MatLike (numpy.ndarray) object
        # import cv2
        # # fill-in pixel data to cv_image...
        # cv_image = cv2.imread(str(dirEntry))
        # detector = mr.MoonDetect.MoonDetector(cv_image)
        
        
        # Following functions of mr.MoonDetect.MoonDetector will set function pointers in the class.
        # Those pointers are functions to handle different steps in mr.MoonDetect.MoonDetector.detect_moon()
        # You can modify them to further customize how mr.MoonDetect.MoonDetector.detect_moon() works
        # All the function pointers are set to default_*** functions defined in "detector.hpp" by default
        detector.set_preprocess_steps(preprocess_steps)
        detector.set_param_init(param_init)
        detector.set_iteration_param_update(iteration_param_update)
        detector.set_iteration_circle_select(iteration_circle_select)
        detector.set_coordinate_remap(coordinate_remap)
        
        
        # # You can change the underlying algorithm used in cv::HoughCircles()
        # detector.hough_circles_algorithm = mr.MoonDetect.HoughCirclesAlgorithm.HOUGH_GRADIENT
        # # HOUGH_GRADIENT_ALT algorithm only available with OpenCV >= 4.8.1
        # detector.hough_circles_algorithm = mr.MoonDetect.HoughCirclesAlgorithm.HOUGH_GRADIENT_ALT
            
        # calculate moon position
        final_circle = detector.detect_moon()
        
        
        # printing out result
        print('\n\n\n', end='')
        print(f'file: \'{dirEntry}\'')
        print(f'Circle: {final_circle}')
        print(f'Square: {mr.shapes.circle_to_square_s(final_circle)}')
        print(f'Rectangle: {mr.shapes.circle_to_rectangle_s(final_circle)}')
        
    except Exception as err:
        print(f'Exception: {err}')

