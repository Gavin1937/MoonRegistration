
# Building MoonRegistration library from source code


**If you only want to get library binary, consider [download pre-build binary](https://github.com/Gavin1937/MoonRegistration/releases/latest) or [build using docker](#build-using-docker-recommend)**

## Build System

This project uses CMake as its build system. You need to [install CMake](https://cgold.readthedocs.io/en/latest/first-step/installation.html) in your system first


## Build Using Docker (recommend)

### Dependencies

* [Docker](https://docs.docker.com/engine/install/)

### Build normal library

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

### Build WebAssembly library

build the docker image with:

```sh
docker build -t moonregistration_wasm -f .\Dockerfile_wasm .
```

once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration_wasm moonregistration_wasm
```

built library binaries are inside folder `/src/MoonRegistration/build_wasm`

all the build environments are set, so you can easily rebuild the library following [these steps](#build-into-webassembly-library) and modify [build arguments](#cmake-build-arguments)

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

* OpenCV 4.8.0
  * [Install in Windows](https://sourceforge.net/projects/opencvlibrary/files/4.8.0/)
  * [Install in MacOS](https://www.geeksforgeeks.org/how-to-install-opencv-for-c-on-macos/)
    * `brew install opencv@4.8.0`
  * [Install in linux](https://phoenixnap.com/kb/installing-opencv-on-ubuntu)

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


## Build Into WebAssembly Library

**Note: this WebAssembly library only successfully built on Linux or WSL environment**

### Dependencies

* OpenCV 4.8.0 **Source Code**
  * [download source code zip file](https://github.com/opencv/opencv/archive/refs/tags/4.8.0.zip)
  * [clone from github](https://github.com/opencv/opencv) (slower)
* [setup emsdk](https://emscripten.org/docs/getting_started/downloads.html)

### Build Steps

**Project Structure in following steps**

```
/some/root/
    |
    -- opencv/
    |       |
    |       -- build_wasm/
    |
    -- MoonRegistration
    |       |
    |       -- build_wasm/
    |
    -- emsdk/
            |
            -- upstream/
                    |
                    -- emscripten/
```

1. after you setup emsdk
2. add EMSCRIPTEN to your environment variable.

```sh
export EMSCRIPTEN="/some/root/emsdk/upstream/emscripten"
```

3. goto your opencv source code directory
4. creaate a new directory called **build_wasm** (you cannot change this name)
5. build opencv into WebAssembly (this will take a while)

```sh
mkdir build_wasm
python ./platforms/js/build_js.py build_wasm --build_wasm
```

6. go to MoonRegistration folder 
7. create a new directory called **build_wasm** and go inside it
8. build MoonRegistration wasm

CMake initialize

```sh
mkdir build_wasm && cd build_wasm
emcmake cmake -DMR_BUILD_WASM=ON -DOPENCV_DIR="/some/root/opencv" ..
```

**Note that I add two cmake arguments to the command, [checkout build argument section](#cmake-build-arguments)**

Make build

```sh
emmake make
```

9. once building is done, you can start using `MoonRegistration.js` and `MoonRegistration.wasm`


## CMake Build Arguments

* Build Static / Shared Library
  * You can tell cmake to build the library into a shared library by setting `-DMR_BUILD_SHARED_LIBS=ON` (this is on by default)
  * Or, you can turn it off to build the library into a static library `-DMR_BUILD_SHARED_LIBS=OFF`

* Build WebAssembly
  * In order to build this library into WebAssembly
  * You MUST pass in following arguments
    * `-DMR_BUILD_WASM=ON`
    * `-DOPENCV_DIR="/path/to/opencv"`
  * These arguments will enable WebAssembly mode and tells CMake where is opencv's source code
  * **Note that you need to point to opencv's master source directory, not "build_wasm" directory inside it**
