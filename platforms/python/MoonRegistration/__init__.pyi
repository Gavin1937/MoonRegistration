import shapes
import imgprocess
import MoonDetect
import MoonRegistrar

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
from .MoonRegistrar import (
    RegistrationAlgorithms,
    MoonRegistrar,
)

__version__: str
__all__: list[str] = [
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
    
    'MoonRegistrar',
    'RegistrationAlgorithms',
    'MoonRegistrar',
    
    'version',
]

def version() -> str: ...
