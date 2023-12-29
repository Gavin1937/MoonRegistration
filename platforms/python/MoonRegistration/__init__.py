
# When installed under unix system, the package may be scattered into different
# folders defined in sys.path. Following 2 lines will add to the package's __path__
# all subdirectories of directories on sys.path named after the package.
# It can help this __init__.py to find all the scattered modules.
# https://stackoverflow.com/a/9003220
from pkgutil import extend_path
__path__ = extend_path(__path__, __name__)

__all__ = [
    # version.hpp
    'version',
    # shapes.hpp
    'Circle', 'Square', 'Rectangle',
    'circle_to_square_s',
    'circle_to_square_p',
    'circle_to_rectangle_s',
    'circle_to_rectangle_p',
    'is_valid_circle_s',
    'is_valid_circle_p',
    # utils.hpp
    'ImageShape',
    'calc_image_shape',
    # preprocessing.hpp
    'resize_with_aspect_ratio',
    'apply_brightness_contrast',
    'calc_img_brightness_perc',
    'calc_circle_brightness_perc',
    'cut_image_from_circle',
    # MoonDetect/selector.hpp
    'select_circle_by_brightness_perc',
    'select_n_circles_by_brightness_perc',
    'select_circle_by_largest_radius',
    'select_n_circles_by_largest_radius',
    'select_circle_by_shape',
    # MoonDetect/detector.hpp
    'find_circles_in_img',
    'default_preprocess_steps',
    'default_param_init',
    'default_iteration_param_update',
    'default_iteration_circle_select',
    'default_coordinate_remap',
    'MoonDetector',
]

__doc__ = '''
Python wrapper for MoonRegistration Library.
MoonRegistration Is A Cross Platform C++ library for Moon Location Detection & Moon Image Registration.
'''

import os
from pathlib import Path
import platform


# os.add_dll_directory is only for windows, other platforms use PATH env variable
# this wrapper function can handle different platforms
def add_dll_directory(lib_folder:str):
    if platform.system() == 'Windows':
        os.add_dll_directory(lib_folder)
    elif not os.environ['PATH'].__contains__(lib_folder):
        os.environ['PATH'] = lib_folder + os.pathsep + os.environ['PATH']

# search for dependency libraries if their environment variable exists
# and then add their library files to python library search list
env_OpenCV_DIR = os.getenv('OpenCV_DIR')
if env_OpenCV_DIR:
    for folder in Path(env_OpenCV_DIR).glob('**/'):
        if 'lib' in folder.name or 'bin' in folder.name:
            add_dll_directory(str(folder.resolve()))


# add libraries bring with the package
libs_dir = Path(__file__).parent / 'libs'
if libs_dir.exists():
    add_dll_directory(str(libs_dir.resolve()))


try:
    from .libs.MoonRegistration_pywrapper import *
    __version__ = version()
except ImportError as ierr:
    if 'DLL' in ierr.msg:
        raise ImportError('Missing Dependencies, please make sure you install all the dependencies correctly.')
    raise ierr

