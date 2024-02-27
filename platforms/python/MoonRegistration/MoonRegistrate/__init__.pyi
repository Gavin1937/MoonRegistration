from __future__ import annotations
from enum import IntEnum
from typing import Iterable, Callable, overload
import numpy
import cv2

__all__: list[str] = [
    'RegistrationAlgorithms',
    'default_is_good_match',
    'MoonRegistrar',
]

class RegistrationAlgorithms(IntEnum):
    SIFT                               = 0x100,
    ORB                                = 0x101,
    AKAZE                              = 0x102,
    BRISK                              = 0x103,
    
#ifdef MR_HAVE_OPENCV_NONFREE
    # opencv non-free algorithms
    SURF_NONFREE                       = 0x200,
#endif
    EMPTY_ALGORITHM                    = 0x001,
    INVALID_ALGORITHM                  = 0x000

def default_is_good_match(
    m:cv2.DMatch,
    n:cv2.DMatch,
    good_match_ratio:float,
    user_kpt:cv2.KeyPoint,
    model_kpt:cv2.KeyPoint,
    user_image:numpy.ndarray,
    model_image:numpy.ndarray
) -> bool: ...

class MoonRegistrar():
    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(
        self,
        user_filepath:str,
        model_filepath:str,
        algorithm:RegistrationAlgorithms
    ) -> None: ...
    @overload
    def __init__(
        self,
        user_binary:bytes,
        model_binary:bytes,
        algorithm:RegistrationAlgorithms
    ) -> None: ...
    @overload
    def __init__(
        self,
        user_image:numpy.ndarray,
        model_image:numpy.ndarray,
        algorithm:RegistrationAlgorithms
    ) -> None: ...
    @overload
    def update_images(
        self,
        user_filepath:str,
        model_filepath:str
    ) -> None: ...
    @overload
    def update_images(
        self,
        user_binary:bytes,
        model_binary:bytes
    ) -> None: ...
    @overload
    def update_images(
        self,
        user_image:numpy.ndarray,
        model_image:numpy.ndarray
    ) -> None: ...
    def update_f2d_detector(
        self,
        algorithm:RegistrationAlgorithms
    ) -> None: ...
    def get_homography_matrix(self) -> numpy.ndarray: ...
    def get_user_image(self) -> numpy.ndarray: ...
    def get_user_keypoints(self) -> list[cv2.KeyPoint]: ...
    def get_model_image(self) -> numpy.ndarray: ...
    def get_model_keypoints(self) -> list[cv2.KeyPoint]: ...
    def get_good_keypoint_matches(self) -> list[list[cv2.DMatch]]: ...
    def compute_registration(
        self,
        knn_k:int                                     = 2,
        good_match_ratio:float                        = 0.7,
        find_homography_method:int                    = cv2.RANSAC,
        find_homography_ransac_reproj_threshold:float = 5.0
    ) -> None: ...
    def transform_image(self, image_in:numpy.ndarray) -> numpy.ndarray: ...
    def transform_image_inverse(self, image_in:numpy.ndarray) -> numpy.ndarray: ...
    def transform_user_image(self) -> numpy.ndarray: ...
    def transform_layer_image(self, layer_image_in:numpy.ndarray) -> numpy.ndarray: ...
    def draw_matched_keypoints(self) -> numpy.ndarray: ...
    def draw_red_transformed_user_image(self, transformed_image_in:numpy.ndarray) -> numpy.ndarray: ...
    def draw_green_model_image(self) -> numpy.ndarray: ...
    def draw_stacked_red_green_image(self, transformed_image_in:numpy.ndarray) -> numpy.ndarray: ...
    def draw_layer_image(
        self,
        layer_image_in:numpy.ndarray,
        layer_image_transparency:float                = 1.0,
        filter_px:Iterable                            = None
    ) -> numpy.ndarray: ...
    def set_is_good_match(self, func:Callable) -> None: ...
