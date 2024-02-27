from __future__ import annotations
from typing import Iterable, overload
import numpy
import cv2

from ..shapes import Rectangle

__all__: list[str] = [
    'ImageShape',
    'calc_image_shape',
    'resize_with_aspect_ratio',
    'apply_brightness_contrast',
    'calc_img_brightness_perc',
    'calc_circle_brightness_perc',
    'cut_image_from_circle',
    'sync_img_size',
    'ImageChannels',
    'split_img_channel',
    'merge_img_channel',
    'stack_imgs',
    'stack_imgs_in_place',
]

class ImageShape():
    def __init__(self, height:int, width:int, longer_side:int, shorter_side:int) -> None:
        self.height:int = height
        self.width:int = width
        self.longer_side:int = longer_side
        self.shorter_side:int = shorter_side

def calc_image_shape(image_in:numpy.ndarray) -> ImageShape: ...
def resize_with_aspect_ratio(
    image_in:numpy.ndarray,
    width:int             = -1,
    height:int            = -1,
    longer_side_val:int   = -1,
    inter:int             = cv2.INTER_AREA
) -> tuple[float, numpy.ndarray]: ...
def apply_brightness_contrast(
    image_in:numpy.ndarray,
    brightness:int        = 0,
    contrast:int          = 0
) -> numpy.ndarray: ...
def calc_img_brightness_perc(
    image_in:numpy.ndarray
) -> float: ...
def calc_circle_brightness_perc(
    image_in:numpy.ndarray,
    center_x:int,
    center_y:int,
    radius:int
) -> float: ...
def cut_image_from_circle(
    image_in:numpy.ndarray,
    x:int,
    y:int,
    radius:int,
    padding:int           = 0
) -> tuple[Rectangle, numpy.ndarray]: ...
def sync_img_size(
    primary:numpy.ndarray,
    secondary:numpy.ndarray
) -> numpy.ndarray: ...

class ImageChannels():
    @overload
    def __init__(self) -> None:
        self.channels:ImageChannels
    @overload
    def __init__(self, alpha:numpy.ndarray) -> None:
        self.channels:ImageChannels
    @overload
    def __init__(self, blue:numpy.ndarray, green:numpy.ndarray, red:numpy.ndarray) -> None:
        self.channels:ImageChannels
    @overload
    def __init__(self, blue:numpy.ndarray, green:numpy.ndarray, red:numpy.ndarray, alpha:numpy.ndarray) -> None:
        self.channels:ImageChannels
    def blue(self) -> numpy.ndarray: ...
    def green(self) -> numpy.ndarray: ...
    def red(self) -> numpy.ndarray: ...
    def alpha(self) -> numpy.ndarray: ...
    def channel_num(self) -> int: ...

def split_img_channel(image_in:numpy.ndarray) -> ImageChannels: ...
def merge_img_channel(channels:ImageChannels) -> numpy.ndarray: ...

def stack_imgs(
    background:numpy.ndarray,
    background_roi:Iterable[int],
    foreground:numpy.ndarray,
    foreground_transparency:float = 1.0,
    filter_px:Iterable[int] = None
) -> numpy.ndarray: ...
def stack_imgs_in_place(
    background:numpy.ndarray,
    background_roi:Iterable[int],
    foreground:numpy.ndarray,
    foreground_transparency:float = 1.0,
    filter_px:Iterable[int] = None
) -> numpy.ndarray: ...
