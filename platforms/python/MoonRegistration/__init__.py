import os
from pathlib import Path
import platform


# os.add_dll_directory isn't available nor necessary for all platforms
def add_dll_directory(*args):
    if platform.system() == 'Windows':
        os.add_dll_directory(*args)

# search for dependency libraries if their environment variable exists
# and then add their library files to python library search list
env_OpenCV_DIR = os.getenv('OpenCV_DIR')
if env_OpenCV_DIR:
    for folder in Path(env_OpenCV_DIR).glob('**/'):
        if 'lib' in folder.name or 'bin' in folder.name:
            add_dll_directory(folder)
env_BOOST_ROOT = os.getenv('BOOST_ROOT')
if env_BOOST_ROOT:
    for folder in Path(env_BOOST_ROOT).glob('**/'):
        if 'lib' in folder.name or 'bin' in folder.name:
            add_dll_directory(folder)


libs_dir = Path(__file__).parent / 'libs'
if libs_dir.exists():
    add_dll_directory(str(libs_dir.resolve()))


try:
    from .__version__ import __version__
    from .libs.MoonRegistration_pywrapper import MoonDetector
except ImportError as ierr:
    if 'DLL' in ierr.msg:
        raise ImportError('Missing Dependencies, please make sure you install all the dependencies correctly.')
    raise ierr

