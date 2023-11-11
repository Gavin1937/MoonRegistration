from setuptools import setup, Extension
from distutils.sysconfig import get_python_lib
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
import os
from os import sep as SEP
from sys import argv
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
with open(ROOT/'MoonRegistration/__version__.py', 'r', encoding='utf-8') as file:
    exec(file.read(), VERSION)

LIB_FILES = (
    [f.resolve() for f in (ROOT/'build').rglob(f'*.{OS_SHARED_LIBRARY_SUFFIX}') if f.is_file()] +
    [f.resolve() for f in (ROOT/'build').rglob(f'MoonRegistration_pywrapper.{OS_PYEXT_SUFFIX}') if f.is_file()]
)
DATA_FILES = None
PACKAGE_DATA = {
    'MoonRegistration':[
        f'*.py',
        f'./build/**/*.{OS_SHARED_LIBRARY_SUFFIX}',
        f'./build/**/*.{OS_PYEXT_SUFFIX}',
    ],
}
DEST_NAME = None
if argv[1] == 'bdist_wheel':
    DEST_NAME = f'{SEP}MoonRegistration{SEP}libs'
else:
    DEST_NAME = f'{get_python_lib()}{SEP}MoonRegistration{SEP}libs'
DATA_FILES = [
    (DEST_NAME, [str(f) for f in LIB_FILES])
]



class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='.', **kwa):
        Extension.__init__(self, name, sources=[], **kwa)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)

class cmake_build_ext(build_ext):
    def build_extensions(self):
        
        import subprocess
        
        # Ensure that CMake is present and working
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('Cannot find CMake executable')
        
        for ext in self.extensions:
            
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            cfg = 'Debug' if os.environ.get('DISPTOOLS_DEBUG','OFF') == 'ON' else 'Release'
            
            cmake_args = [
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE=%s' % self.get_ext_fullpath(ext.name)
            ]
            
            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)
            
            # Config
            subprocess.check_call(['cmake', ext.cmake_lists_dir] + cmake_args, cwd=self.build_temp)
            
            # Build
            subprocess.check_call(['cmake', '--build', '.', '--config', cfg], cwd=self.build_temp)

class Build_ext_first(install):
    def run(self):
        self.run_command("build_ext")
        
        global LIB_FILES
        LIB_FILES = (
            [f.resolve() for f in (ROOT/'build').rglob(f'*.{OS_SHARED_LIBRARY_SUFFIX}') if f.is_file()] +
            [f.resolve() for f in (ROOT/'build').rglob(f'MoonRegistration_pywrapper.{OS_PYEXT_SUFFIX}') if f.is_file()]
        )
        
        # install destination is different than DEST_NAME
        install_dest_name = f'{get_python_lib()}{SEP}MoonRegistration{SEP}libs'
        Path(install_dest_name).mkdir(parents=True, exist_ok=True)
        # copy libraries to install directory
        for libs in LIB_FILES:
            self.copy_file(str(libs), f'{install_dest_name}{SEP}')
        
        return install.run(self)



def main():
    setup(
        name="MoonRegistration",
        version=VERSION['__version__'],
        description="Python wrapper for MoonRegistration library",
        author="Gavin1937",
        url='https://github.com/Gavin1937/MoonRegistration',
        python_requires='>=3.8.0',
        
        packages=['MoonRegistration'],
        ext_modules=[CMakeExtension(
            name="MoonRegistration",
            cmake_lists_dir=ROOT,
        )],
        cmdclass={
            'build_ext':cmake_build_ext,
            'install': Build_ext_first,
        },
        data_files=DATA_FILES,
        package_data=PACKAGE_DATA,
    )

if __name__ == "__main__":
    main()
