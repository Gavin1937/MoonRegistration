
# When installed under unix system, the package may be scattered into different
# folders defined in sys.path. Following 2 lines will add to the package's __path__
# all subdirectories of directories on sys.path named after the package.
# It can help this __init__.py to find all the scattered modules.
# https://stackoverflow.com/a/9003220
from pkgutil import extend_path
__path__ = extend_path(__path__, __name__)


__doc__ = '''
Python wrapper for MoonRegistration Library.
MoonRegistration Is A Cross Platform C++ library for Moon Location Detection & Moon Image Registration.
'''

import os, sys
import importlib
from pathlib import Path
import platform


# Inspired by OpenCV Python's top layer __init__.py setup
# https://github.com/opencv/opencv/blob/4.x/modules/python/package/cv2/__init__.py
# Loading python extension submodules for this module
def __load_py_ext_submodule_for_module(base, submodule_path):
    submodule_name = submodule_path.split(".")[-1]
    module_name = "{}.{}".format(base, submodule_name)
    export_module_name = "{}.{}".format(base, submodule_name)
    native_module = sys.modules.pop(module_name, None)
    
    try:
        py_module = importlib.import_module(submodule_path)
    except ImportError as err:
        # Extension doesn't contain extra py code
        return False
    
    if not hasattr(base, submodule_path):
        setattr(sys.modules[base], submodule_name, py_module)
    sys.modules[export_module_name] = py_module
    return True

# Inspired by OpenCV Python's top layer __init__.py setup
# https://github.com/opencv/opencv/blob/4.x/modules/python/package/cv2/__init__.py
# Collect python extension submodules under the parent of current __init__.py file
def __collect_py_ext_submodules():
    init_root = os.path.dirname(__file__)
    def modules_filter(module:str):
        # print('module:',module)
        return (
            os.path.isdir(os.path.join(init_root,module)) and
            not module.endswith('libs') and
            not module.endswith('__')
        )
    return filter(modules_filter, os.listdir(init_root))

# os.add_dll_directory is only for windows, other platforms use PATH env variable
# this wrapper function can handle different platforms
def __add_dll_directory(lib_folder:str):
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
            __add_dll_directory(str(folder.resolve()))


# add libraries bring with the package
libs_dir = Path(__file__).parent / 'libs'
if libs_dir.exists():
    __add_dll_directory(str(libs_dir.resolve()))


try:
    from .libs.MoonRegistration_pywrapper import *
    __version__:str = version()
    
    for submodule in __collect_py_ext_submodules():
        print('submodule:',submodule)
        __load_py_ext_submodule_for_module(
            'MoonRegistration',
            '.'.join(['MoonRegistration','libs','MoonRegistration_pywrapper',submodule])
        )
except ImportError as ierr:
    if 'DLL' in ierr.msg:
        raise ImportError('Missing Dependencies, please make sure you install all the dependencies correctly.')
    raise ierr

