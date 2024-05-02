
# Building MoonRegistration library from source code


**If you only want to get library binary, consider [download pre-build binary](https://github.com/Gavin1937/MoonRegistration/releases/latest) or [build using docker](#build-using-docker-recommend)**

## Build System

This project uses CMake (version >= 3.11) as its build system. You need to [install CMake](https://cgold.readthedocs.io/en/latest/first-step/installation.html) in your system first


## **About OpenCV versions & modules:**

This library is heavily rely on OpenCV, linking with different version of OpenCV or with different OpenCV modules will enable/disable some feature of the library.

Starting from **version 4.8.1**, OpenCV [provide a new algorithm (HOUGH_GRADIENT_ALT) for hough circle detection function](https://docs.opencv.org/4.x/dd/d1a/group__imgproc__feature.html#ga47849c3be0d0406ad3ca45db65a25d2d), thus increase the overall accuracy. If this library is linked with OpenCV >= 4.8.1, the MoonDetect module will use an optimized circle detection pipeline utilizing the new algorithm from OpenCV by default. Otherwise, the MoonDetect module will use the original HOUGH_GRADIENT algorithm for circle detection.

[OpenCV Contrib](https://github.com/opencv/opencv_contrib) is a set of community contributed extra modules thats not include in the main repository. Some modules and algorithms inside maybe patented in some countries or have some other limitations on the use. The MoonRegistrate module uses some image registration algorithms from OpenCV Contrib module. Therefore, when linking with an OpenCV that doesn't contain OpenCV Contrib, part of the image registration algorithms are not available.

### Table below is a list of non-free modules & algorithms from OpenCV Contrib that we use, and the image registration algorithms relating to it.

| non-free OpenCV modules used                                                         | non-free OpenCV algorithms used                                                | mr::RegistrationAlgorithms related |
|--------------------------------------------------------------------------------------|--------------------------------------------------------------------------------|------------------------------------|
| [XFeatures2D](https://docs.opencv.org/4.9.0/d2/dca/group__xfeatures2d__nonfree.html) | [SURF](https://docs.opencv.org/3.4/d5/df7/classcv_1_1xfeatures2d_1_1SURF.html) | SURF_NONFREE                       |

> When OpenCV Contrib is not present, above mr::RegistrationAlgorithms are not available.

### How to get OpenCV Contrib

OpenCV Contrib is not included in normal OpenCV release. If you want to enable those non-free registration algorithms, you need to:
* Install an OpenCV compiled with extra modules in [OpenCV Contrib](https://github.com/opencv/opencv_contrib).
  * That means you probably need to **compile OpenCV with Contrib module by yourself**
  * [Compile in Windows](https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html)
  * [Compile in MacOS](https://docs.opencv.org/3.4/d0/db2/tutorial_macos_install.html)
  * [Compile in Linux](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
  * Download pre-built binary: libopencv-dev and libopencv-contrib-dev
    * Note that you may not be able to find the latest OpenCV Contrib pre-built binary
  * [You can also get OpenCV Contrib from vcpkg](#build-with-vcpkg)
* Once you have OpenCV and OpenCV Contrib library installed, you can enable MoonRegistration's non-free algorithms by setting cmake flag `-DMR_ENABLE_OPENCV_NONFREE=ON`. More details in [CMake Build Arguments](#cmake-build-arguments).


## Build Using Docker (recommend)

### Dependencies

* [Docker](https://docs.docker.com/engine/install/)

### Build library

build the docker image with:

```sh
docker build -t moonregistration -f ./dockerfiles/cpp_demo.dockerfile .
```

once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration moonregistration
```

built library binaries are inside folder `/src/MoonRegistration/build`

all the build environments are set, so you can easily rebuild the library following [these steps](#build-into-library) and modify [build arguments](#cmake-build-arguments)

### Build release package

build the docker image with:

```sh
docker build -t moonregistration -f ./dockerfiles/release_env.dockerfile .
```

once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration moonregistration
```

built library release packages are in `/src/MoonRegistration/*.zip` archives

all the build environments are set, so you can easily rebuild the library following [these steps](#build-into-webassembly-library) and modify [build arguments](#cmake-build-arguments)


## Develope with Dev Container

You can use Docker container to develope this library.

* If you use VSCode as your Editor, you can [follow this doc](https://code.visualstudio.com/docs/devcontainers/tips-and-tricks) to install docker and [VSCode Dev Container Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers).
  * After installation, you can use [VSCode Command Palette](https://code.visualstudio.com/docs/getstarted/userinterface#_command-palette) to search & execute command: `Dev Containers: Reopen in Container`
  * This command will reopen this folder inside a Dev Container
  * If git is breaking inside Dev Container, follow [this doc](https://code.visualstudio.com/remote/advancedcontainers/sharing-git-credentials) to fix it.
* If you don't use VSCode as your Editor, you can launch a container from [./dockerfiles/dev_env.dockerfile](./dockerfiles/dev_env.dockerfile) and trying to connect to it.
  * Remember to mount repository root into this container.


## Build with vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) is a cross-platform C++ Library Manager (Dependency Package Manager). You can use it for easy dependency setup. In following examples, we will use powershell as our shell, [checkout this doc](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started) for other shells and more detail.

> vcpkg will download and build opencv4 for your machine, so it will take a while.

> Note: Util March 24, 2024, vcpkg only have opencv version 4.8.0. Which means you cannot use HOUGH_CIRCLE_ALT algorithm for MoonDetect module ([learn more here](#about-opencv-versions--modules)). Checkout [this manifest file](https://github.com/microsoft/vcpkg/blob/master/ports/opencv4/vcpkg.json) for the latest version.

1. Setup vcpkg

```powershell
# get vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg; .\bootstrap-vcpkg.bat
# setup environment variables
$env:VCPKG_ROOT = "C:\path\to\vcpkg"
$env:PATH = "$env:VCPKG_ROOT;$env:PATH"
```

2. Go to MoonRegistration repository root and install dependency

```powershell
vcpkg install
```

3. Now, you can use the dependency you just built to build MoonRegistration. You need to use vcpkg's cmake toolchain file with cmake.

```powershell
mkdir build
# set cmake toolchain file using cmake flag
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
# or set cmake toolchain file using environment variable
$env:CMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake -S . -B build
```


## Build Manually from Source

### Dependencies

* OpenCV
  * [Setup using vcpkg (recommend)](#build-with-vcpkg)
  * [Install in Windows](https://opencv.org/releases/)
  * [Install in MacOS](https://www.geeksforgeeks.org/how-to-install-opencv-for-c-on-macos/)
    * `brew install opencv@4.8.0`
  * [Install in linux](https://phoenixnap.com/kb/installing-opencv-on-ubuntu)
  * [Checkout OpenCV's version & modules that we use](#about-opencv-versions--modules)
  * **Once you installed OpenCV, remember to set an environment variable `OpenCV_DIR=/path/to/opencv_root_dir`. This environment variable will help other software to find OpenCV**
    * This is mandatory for Windows user.
    * For other OS, your OS package manager may handle those thing for you, but it's good to set it up.


### Build Steps

```sh
# clone this repo
git clone --recursive https://github.com/Gavin1937/MoonRegistration

cd MoonRegistration

mkdir build && cd build

# use cmake to build the library
cmake ..
cmake --build .
```

## CMake Build Arguments

* Build Static / Shared Library
  * You can tell cmake to build the library into a shared library by setting `-DMR_BUILD_SHARED_LIBS=ON` (this is on by default)
  * Or, you can turn it off to build the library into a static library `-DMR_BUILD_SHARED_LIBS=OFF`
* Use Custom build of opencv
  * You can tell cmake to use your opencv build with flag `-DOPENCV_DIR`. This flag should point to a folder that contains cmake files like **"OpenCVConfig.cmake"** or **"opencv-config.cmake"**
* To enable MoonRegistration library to [use OpenCV non-free modules and algorithms](#about-opencv-non-free-modules), you can set cmake flag `-DMR_ENABLE_OPENCV_NONFREE=ON`. By default, this option is `OFF` by default.

## Install with CMake

To install this library with cmake, simply use `cmake --install` command:

```sh
cmake --install /path/to/build/dir --prefix /path/to/install/dir
```

If you don't supply `--prefix` flag, cmake will try to install all the library & header files to:

* If in Windows or is cross-compiling this library, install to:
  * `"${CMAKE_BINARY_DIR}/install"`
* Otherwise install to:
  * `"/usr/local"`
