from __future__ import annotations
from enum import IntEnum
from typing import Callable, overload
import numpy
import cv2

from .. import MR_HAVE_HOUGH_GRADIENT_ALT
from ..shapes import Circle, Rectangle
from ..imgprocess import ImageShape

__all__: list[str] = [
    'select_circle_by_brightness_perc',
    'select_n_circles_by_brightness_perc',
    'select_circle_by_largest_radius',
    'select_n_circles_by_largest_radius',
    'select_circle_by_shape',
    'HoughCirclesAlgorithms',
    'find_circles_in_img',
    'default_preprocess_steps',
    'default_param_init',
    'default_iteration_param_update',
    'default_iteration_circle_select',
    'default_coordinate_remap',
    'MoonDetector',
]

def select_circle_by_brightness_perc(
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...
def select_n_circles_by_brightness_perc(
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray,
    n:int
) -> Circle: ...
def select_circle_by_largest_radius(
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...
def select_n_circles_by_largest_radius(
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray,
    n:int
) -> Circle: ...
def select_circle_by_shape(
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...

if MR_HAVE_HOUGH_GRADIENT_ALT:
    class HoughCirclesAlgorithms(IntEnum):
        HOUGH_GRADIENT        = 0x101,
    # Starting from OpenCV 4.8.1, algorithm HOUGH_GRADIENT_ALT is available for cv::HoughCircles().
    # This enum will be enabled if OpenCV version >= 4.8.1
    #ifdef MR_HAVE_HOUGH_GRADIENT_ALT
        # use cv::HOUGH_GRADIENT_ALT with basic optimization
        HOUGH_GRADIENT_ALT    = 0x102,
        
        # use cv::HOUGH_GRADIENT and cv::HOUGH_GRADIENT_ALT together for the best result
        HOUGH_GRADIENT_MIX    = 0x103,
    #endif
        EMPTY_ALGORITHM       = 0x001,
        INVALID_ALGORITHM     = 0x000
else:
    class HoughCirclesAlgorithms(IntEnum):
        HOUGH_GRADIENT        = 0x101,
        
        EMPTY_ALGORITHM       = 0x001,
        INVALID_ALGORITHM     = 0x000

def find_circles_in_img(
    image_in:numpy.ndarray,
    circle_threshold:int,
    dp:float,
    minDist:float,
    minRadius:int,
    maxRadius:int,
    param1:float,
    param2:float,
    algorithm:int = cv2.HOUGH_GRADIENT
) -> numpy.ndarray: ...


# default stage functions
# for class MoonDetector differ by the
# HoughCirclesAlgorithms they use.
# Algorithm name to function prefix translation:
# HOUGH_GRADIENT        => HG
# HOUGH_GRADIENT_ALT    => HGA
# HOUGH_GRADIENT_MIX    => HGM

# HOUGH_GRADIENT (HG)
def HG_default_preprocess_steps(
    image_in:numpy.ndarray
) -> tuple[numpy.ndarray, float]: ...
def HG_default_param_init(
    image_shape:ImageShape,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,int,float,float,float,int,float,int,float,float,int]: ...
def HG_default_iteration_param_update(
    iteration:int,
    image_brightness_perc:float,
    initial_image_size:tuple,
    image_shape:ImageShape,
    curr_circle_found:Circle,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    process_image:numpy.ndarray,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,numpy.ndarray,float,float,float,int,float,int,float,float,int]: ...
def HG_default_iteration_circle_select(
    iteration:int,
    max_iteration:int,
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...
def HG_default_coordinate_remap(
    result_list:list[tuple[int, Circle, Rectangle]],
    resize_ratio:float
) -> Circle: ...

# HOUGH_GRADIENT (HGA)
def HGA_default_preprocess_steps(
    image_in:numpy.ndarray
) -> tuple[numpy.ndarray, float]: ...
def HGA_default_param_init(
    image_shape:ImageShape,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,int,float,float,float,int,float,int,float,float,int]: ...
def HGA_default_iteration_param_update(
    iteration:int,
    image_brightness_perc:float,
    initial_image_size:tuple,
    image_shape:ImageShape,
    curr_circle_found:Circle,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    process_image:numpy.ndarray,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,numpy.ndarray,float,float,float,int,float,int,float,float,int]: ...
def HGA_default_iteration_circle_select(
    iteration:int,
    max_iteration:int,
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...
def HGA_default_coordinate_remap(
    result_list:list[tuple[int, Circle, Rectangle]],
    resize_ratio:float
) -> Circle: ...

# HOUGH_GRADIENT (HGM)
def HGM_default_preprocess_steps(
    image_in:numpy.ndarray
) -> tuple[numpy.ndarray, float]: ...
def HGM_default_param_init(
    image_shape:ImageShape,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,int,float,float,float,int,float,int,float,float,int]: ...
def HGM_default_iteration_param_update(
    iteration:int,
    image_brightness_perc:float,
    initial_image_size:tuple,
    image_shape:ImageShape,
    curr_circle_found:Circle,
    max_iteration:int,
    circle_threshold:int,
    hough_circles_algorithm:int,
    process_image:numpy.ndarray,
    dp:float,
    minDist:float,
    minRadiusRate:float,
    minRadius:int,
    maxRadiusRate:float,
    maxRadius:int,
    param1:float,
    param2:float,
    cut_circle_padding:int
) -> tuple[int,int,numpy.ndarray,float,float,float,int,float,int,float,float,int]: ...
def HGM_default_iteration_circle_select(
    iteration:int,
    max_iteration:int,
    image_in:numpy.ndarray,
    detected_circles:numpy.ndarray
) -> Circle: ...
def HGM_default_coordinate_remap(
    result_list:list[tuple[int, Circle, Rectangle]],
    resize_ratio:float
) -> Circle: ...


class MoonDetector():
    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, filepath:str) -> None: ...
    @overload
    def __init__(self, binary:bytes) -> None: ...
    @overload
    def __init__(self, image:numpy.ndarray) -> None: ...
    def is_empty(self) -> bool: ...
    def init_by_path(self, image_filepath:str) -> None: ...
    def init_by_byte(self, image_binary:bytes) -> None: ...
    def init_by_mat(self, image_in:numpy.ndarray) -> None: ...
    def update_hough_circles_algorithm(algorithm:HoughCirclesAlgorithms) -> None: ...
    def detect_moon(self) -> Circle: ...
    def set_preprocess_steps(self, func:Callable) -> None: ...
    def set_param_init(self, func:Callable) -> None: ...
    def set_iteration_param_update(self, func:Callable) -> None: ...
    def set_iteration_circle_select(self, func:Callable) -> None: ...
    def set_coordinate_remap(self, func:Callable) -> None: ...
