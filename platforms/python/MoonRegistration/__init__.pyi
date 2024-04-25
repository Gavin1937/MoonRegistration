import shapes
import imgprocess
import MoonDetect
import MoonRegistrate

from .shapes import (
    Circle,
    Square,
    Rectangle,
)
from .imgprocess import (
    ImageShape,
    ImageChannels,
)
from .MoonDetect import (
    HoughCirclesAlgorithm,
    MoonDetector,
)
from .MoonRegistrate import (
    RegistrationAlgorithms,
    MoonRegistrar,
)

__version__: str
__all__: list[str] = [
    'version',
    'MR_VERSION_STR',
    'MR_HAVE_HOUGH_GRADIENT_ALT',
    'MR_ENABLE_OPENCV_NONFREE',
    'MR_HAVE_OPENCV_NONFREE',
    
    'shapes',
    'Circle',
    'Square',
    'Rectangle',
    
    'imgprocess',
    'ImageShape',
    'ImageChannels',
    
    'MoonDetect',
    'HoughCirclesAlgorithm',
    'MoonDetector',
    
    'MoonRegistrate',
    'RegistrationAlgorithms',
    'MoonRegistrar',
]

def version() -> str: ...
MR_VERSION_STR = ...
MR_HAVE_HOUGH_GRADIENT_ALT = ...
MR_ENABLE_OPENCV_NONFREE = ...
MR_HAVE_OPENCV_NONFREE = ...
