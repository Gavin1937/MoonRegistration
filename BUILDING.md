
# Building MoonRegistration library from source code


**If you only want to get library binary, consider [download pre-build binary](https://github.com/Gavin1937/MoonRegistration/releases/latest) or [build using docker](#build-using-docker-recommend)**

## Build System

This project uses CMake (version >= 3.11) as its build system. You need to [install CMake](https://cgold.readthedocs.io/en/latest/first-step/installation.html) in your system first


## Build Using Docker (recommend)

### Dependencies

* [Docker](https://docs.docker.com/engine/install/)

### Build library

build the docker image with:

```sh
docker build -t moonregistration -f .\Dockerfile .
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
docker build -t moonregistration -f .\Dockerfile_release .
```

once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration moonregistration
```

built library release packages are in `/src/MoonRegistration/*.zip` archives

all the build environments are set, so you can easily rebuild the library following [these steps](#build-into-webassembly-library) and modify [build arguments](#cmake-build-arguments)



## Build Into Library

### Dependencies

* OpenCV
  * [Install in Windows](https://opencv.org/releases/)
  * [Install in MacOS](https://www.geeksforgeeks.org/how-to-install-opencv-for-c-on-macos/)
    * `brew install opencv@4.8.0`
  * [Install in linux](https://phoenixnap.com/kb/installing-opencv-on-ubuntu)
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
