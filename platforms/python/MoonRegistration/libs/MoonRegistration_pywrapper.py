# this file will act as a dummy module of MoonRegistration_pywrapper,
# it will automatically search for the MoonRegistration_pywrapper file,
# and then load the correct file instead,
# this approach can avoid ModuleNotFoundError
# https://stackoverflow.com/a/13466708
def __bootstrap__():
    global __bootstrap__, __loader__, __file__
    from imp import load_dynamic
    from site import getsitepackages
    from os.path import join, exists
    from platform import system
    if system() == 'Windows':
        OS_PYEXT_SUFFIX = 'pyd'
    elif system() == 'Linux':
        OS_PYEXT_SUFFIX = 'so'
    elif system() == 'Darwin': # MacOS
        OS_PYEXT_SUFFIX = 'so'
    file = None
    for folder in getsitepackages():
        file = join(folder,'MoonRegistration/libs/MoonRegistration_pywrapper.'+OS_PYEXT_SUFFIX)
        if exists(file):
            break
    __file__ = file
    __loader__ = None; del __bootstrap__, __loader__
    load_dynamic(__name__,__file__)
__bootstrap__()