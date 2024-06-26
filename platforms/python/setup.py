from setuptools import setup, Extension
from distutils.sysconfig import get_python_lib
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from wheel.bdist_wheel import bdist_wheel
from shutil import copy as shcopy, SameFileError
import re
import os
from os import sep as SEP
import sys
from pathlib import Path
import platform


if platform.system() == 'Windows':
    OS_SHARED_LIBRARY_SUFFIX = 'dll'
    OS_PYEXT_SUFFIX = 'pyd'
elif platform.system() == 'Linux':
    OS_SHARED_LIBRARY_SUFFIX = 'so'
    OS_PYEXT_SUFFIX = 'so'
elif platform.system() == 'Darwin': # MacOS
    OS_SHARED_LIBRARY_SUFFIX = 'dylib'
    OS_PYEXT_SUFFIX = 'so'
ROOT = Path(__file__).parent

VERSION = {}
with open(ROOT/'../../VERSION', 'r', encoding='utf-8') as file:
    VERSION = {'__version__':file.read().strip()}

LICENSE = None
with open(ROOT/'../../LICENSE', 'r', encoding='utf-8') as file:
    LICENSE = file.read()

requirements = []
with open('requirements.txt', 'r', encoding='utf-8') as file:
    requirements = [line.strip() for line in file.readlines() if len(line.strip()) > 0]

HAS_BUILT = False
LIB_FILES = (
    [f.resolve() for f in (ROOT/'build').rglob(f'*.{OS_SHARED_LIBRARY_SUFFIX}') if f.is_file()] +
    [f.resolve() for f in (ROOT/'build').rglob(f'MoonRegistration_pywrapper.{OS_PYEXT_SUFFIX}') if f.is_file()] +
    [ROOT/'MoonRegistration/libs/MoonRegistration_pywrapper.py']
)
DATA_FILES = None
PACKAGE_DATA = {
    'MoonRegistration':[
        f'./build/**/*.{OS_SHARED_LIBRARY_SUFFIX}',
        f'./build/**/*.{OS_PYEXT_SUFFIX}',
    ],
}
PACKAGE_FILES = []
for pattern in PACKAGE_DATA['MoonRegistration'][:-2]:
    PACKAGE_FILES += [f for f in ROOT.glob(pattern)]
DEST_NAME = None
if sys.argv[1] == 'bdist_wheel':
    DEST_NAME = f'{SEP}MoonRegistration{SEP}libs'
else:
    DEST_NAME = f'{get_python_lib()}{SEP}MoonRegistration{SEP}libs'
DATA_FILES = [
    (DEST_NAME, list(map(str, LIB_FILES)))
]


# https://stackoverflow.com/a/48015772
class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='.', **kwa):
        Extension.__init__(self, name, sources=[], **kwa)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)

class cmake_build_ext(build_ext):
    def build_extensions(self):
        global HAS_BUILT
        if HAS_BUILT:
            print('already build_ext, skip...')
            return
        
        import subprocess
        
        # install requirements.txt first, since we need numpy to build the library
        subprocess.check_call(['pip', 'install', '--no-input', 'numpy>=1.23.5'], cwd='.')
        
        # Ensure that CMake is present and working
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('Cannot find CMake executable')
        
        for ext in self.extensions:
            
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            cfg = 'Debug' if os.environ.get('DISPTOOLS_DEBUG','OFF') == 'ON' else 'Release'
            
            cmake_args = [
                '-DMR_BUILD_SHARED_LIBS=OFF',
                '-DMR_ENABLE_OPENCV_NONFREE=%s' % ('ON' if hasattr(self,'mr_enable_opencv_nonfree') and self.mr_enable_opencv_nonfree else 'OFF'),
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE=%s' % self.get_ext_fullpath(ext.name),
                (self.cmake_arguments if hasattr(self, 'cmake_arguments') and self.cmake_arguments else '')
            ]
            
            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)
            
            # Config
            proc = subprocess.Popen(
                ['cmake', '-S', '.', '-B', str(Path(self.build_temp).resolve())] + cmake_args,
                cwd=ext.cmake_lists_dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            
            # searching for 3rd party dependencies library location from cmake output
            out, err = proc.communicate()
            out = out.decode('utf-8').strip()
            err = err.decode('utf-8').strip()
            if len(out) > 0: print(out)
            if len(err) > 0: print(err)
            global LIB_FILES
            opencv_pattern = r'.*OpenCV_DIR: (.*)'
            opencv_match = re.search(opencv_pattern, out)
            if opencv_match:
                opencv_dir = Path(opencv_match.group(1))
                for f in opencv_dir.rglob(f'opencv_*.{OS_SHARED_LIBRARY_SUFFIX}'):
                    if cfg == 'Debug' and f.stem.endswith('d'):
                        LIB_FILES.append(f)
                    elif cfg == 'Release' and not f.stem.endswith('d'):
                        LIB_FILES.append(f)
            # update DATA_FILES accordingly
            global DATA_FILES
            DATA_FILES = [
                (DEST_NAME, list(map(str, LIB_FILES)))
            ]
            # also update data_files variable for other setuptools Commands classes
            self.distribution.data_files = DATA_FILES
            
            # Build
            subprocess.check_call(['cmake', '--build', '.', '--config', cfg], cwd=self.build_temp)
            
            HAS_BUILT = True

class Build_ext_first(install):
    # initialize arguments for install
    # https://stackoverflow.com/a/33200591
    user_options = install.user_options + [
        ('mr-enable-opencv-nonfree', None, 'Set CMake Flag MR_ENABLE_OPENCV_NONFREE to true'),
        ('cmake-arguments=', None, 'Add more CMake Arguments'),
    ]
    
    def initialize_options(self):
        install.initialize_options(self)
        self.mr_enable_opencv_nonfree = False
        self.cmake_arguments = None
    
    def finalize_options(self):
        install.finalize_options(self)
    
    def run(self):
        # set build_ext arguments & run it
        build_ext_command = self.distribution.get_command_obj("build_ext")
        build_ext_command.mr_enable_opencv_nonfree = self.mr_enable_opencv_nonfree
        build_ext_command.cmake_arguments = self.cmake_arguments
        
        # run build_py
        self.run_command("build_py")
        
        global LIB_FILES
        LIB_FILES += (
            [f.resolve() for f in (ROOT/'build').rglob(f'*.{OS_SHARED_LIBRARY_SUFFIX}') if f.is_file()] +
            [f.resolve() for f in (ROOT/'build').rglob(f'MoonRegistration_pywrapper.{OS_PYEXT_SUFFIX}') if f.is_file()]
        )
        
        # install destination is different than DEST_NAME
        install_dest_name = f'{get_python_lib()}{SEP}MoonRegistration{SEP}'
        install_dest_lib_name = f'{install_dest_name}libs{SEP}'
        Path(install_dest_lib_name).mkdir(parents=True, exist_ok=True)
        
        for file in PACKAGE_FILES:
            subfolder_name = Path(file).relative_to(ROOT/'MoonRegistration').parent
            loc_subfolder_name = SEP.join(subfolder_name.parts)
            self.copy_file(str(file), f'{install_dest_name}{SEP}{loc_subfolder_name}')
        
        # copy libraries to install directory
        # also copy to ./MoonRegistration/libs in case of importing lib under current dir
        for libs in LIB_FILES:
            self.copy_file(str(libs), install_dest_lib_name)
            self.copy_file(str(libs), f'.{SEP}MoonRegistration{SEP}libs{SEP}')
        
        return install.run(self)

# This class is a wrapper class on top of bdist_wheel
# it is a workaround of bdist_wheel not including some .pyd & .dll files
# but it still cannot find all the files during the first execution (with blank build folder)
# this is due to the glob pattern in PACKAGE_FILES only be evaluate once and not updating after build_ext
# so we need to run bdist_wheel twice when build folder is empty
class My_bdist_wheel(bdist_wheel):
    # initialize arguments for install
    # https://stackoverflow.com/a/33200591
    user_options = bdist_wheel.user_options + [
        ('mr-enable-opencv-nonfree', None, 'Set CMake Flag MR_ENABLE_OPENCV_NONFREE to true'),
        ('cmake-arguments=', None, 'Add more CMake Arguments'),
    ]
    
    def initialize_options(self):
        bdist_wheel.initialize_options(self)
        self.mr_enable_opencv_nonfree = False
        self.cmake_arguments = None
    
    def finalize_options(self):
        bdist_wheel.finalize_options(self)
    
    def run(self):
        # set build_ext arguments & run it
        build_ext_command = self.distribution.get_command_obj("build_ext")
        build_ext_command.mr_enable_opencv_nonfree = self.mr_enable_opencv_nonfree
        build_ext_command.cmake_arguments = self.cmake_arguments
        
        # copy additional libraries to wheel build dir
        loc_bdist_dir = ROOT / self.bdist_dir
        loc_bdist_dir = (loc_bdist_dir/'MoonRegistration'/'libs')
        loc_bdist_dir.mkdir(parents=True, exist_ok=True)
        for lib in LIB_FILES:
            try:
                shcopy(lib, ROOT/'MoonRegistration/libs')
                shcopy(lib, loc_bdist_dir)
            except SameFileError:
                pass
        
        # run bdist_wheel
        bdist_wheel.run(self)



def main():
    setup(
        name="MoonRegistration",
        version=VERSION['__version__'],
        license=LICENSE,
        description="Python wrapper for MoonRegistration library",
        author="Gavin1937",
        url='https://github.com/Gavin1937/MoonRegistration',
        python_requires='>=3.8.0',
        install_requires=requirements,
        classifiers=[
            'Programming Language :: Python :: 3.8',
            'Programming Language :: Python :: 3.9',
            'Programming Language :: Python :: 3.10',
            'Programming Language :: Python :: 3.11',
        ],
        setup_requires=['wheel'],
        
        packages=[
            'MoonRegistration',
            'MoonRegistration.shapes',
            'MoonRegistration.imgprocess',
            'MoonRegistration.MoonDetect',
            'MoonRegistration.MoonRegistrate',
        ],
        ext_modules=[CMakeExtension(
            name="MoonRegistration",
            cmake_lists_dir=str(ROOT.resolve()),
        )],
        cmdclass={
            'build_ext':cmake_build_ext,
            'install': Build_ext_first,
            'bdist_wheel': My_bdist_wheel,
        },
        data_files=DATA_FILES,
        package_data=PACKAGE_DATA,
    )

if __name__ == "__main__":
    main()
