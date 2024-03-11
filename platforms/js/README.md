
# MoonRegistration JavaScript

JavaScript wrapper for MoonRegistration Library.

We build C++ library into WebAssembly, and then wrap it with a JavaScript abstraction layer.

# Build

## Build With Docker (Recommend)

1. Under the repository root, build Docker image with:

```sh
docker build -t moonregistration_wasm -f .\Dockerfile_wasm .
```

2. Once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration_wasm moonregistration_wasm
```

3. All library binaries are inside folder `/src/MoonRegistration/platforms/js/build_wasm`

4. Released library binaries are inside folder `/src/MoonRegistration/platforms/js/build_wasm/release`

All the build environments are set, so you can easily rebuild the library following [these steps](#build-steps) and modify [build arguments](#build-arguments)

## Build & Install From Source

**Note: this WebAssembly library only successfully built on Linux or WSL environment**

### Dependencies

* OpenCV **Source Code**
  * [download source code](https://opencv.org/releases/)
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
    |       -- platforms/js/build_wasm/
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
4. create a new directory called `build_wasm` (you shouldn't change this name)
5. build opencv into WebAssembly (this will take a while)

```sh
mkdir build_wasm
python ./platforms/js/build_js.py build_wasm --build_wasm
```

* Also note that, you can add cmake flag for opencv by supplying python flag `--cmake_option`. This flag allows you to build opencv wasm library with opencv_contrib and other opencv flags.
  * [learn more in here](https://docs.opencv.org/4.x/d4/da1/tutorial_js_setup.html)
  * [Checkout Dockerfile_wasm for some example](../../Dockerfile_wasm)
  * [Checkout how I build OpenCV in this Dockerfile](https://github.com/Gavin1937/emsdk-cv-wasm/blob/main/Dockerfile)

6. go to `MoonRegistration` folder and goto `platforms/js` folder
7. create a new directory called `build_wasm` and go inside
8. build MoonRegistration wasm

CMake initialize

```sh
emcmake cmake -DOPENCV_SRC="/some/root/opencv" ..
```

* **You need to supply a cmake flag `-DOPENCV_SRC`, which is point to opencv source folder (parent of opencv's build_wasm folder)**
* You can supply other [CMake build arguments](#build-arguments)

Make build

```sh
emmake make
```

Install library

```sh
cmake --install .
```

9. once build & installation are done, you can find a JavaScript ES6 module inside `platforms/js/build_wasm/install/moon-registration` folder. This `moon-registration` folder contains all the js & wasm files for the library.

## Build Arguments

When running CMake initialization, you can supply arguments to CMake.

* flag `-DOPENCV_SRC` is point to opencv source folder (parent of opencv's build_wasm folder)
* flag `-DMRWASM_BUILD_MODULE` defines what C++ functions to export to WebAssembly. By limit the amount of functions exported, we can limit the WebAssembly binary size. It can be any of the following strings:
  * `MoonDetect` - MoonDetect module of the library
  * `MoonRegistrate` - MoonDetect + MoonRegistrate module of the library
  * `All` - All modules of the library
* You also use [build arguments from the core library](../../BUILDING.md#cmake-build-arguments). However, flags `MR_BUILD_SHARED_LIBS` and `OPENCV_DIR` are automatically set by the CMakeLists file in this folder. And flag `MR_ENABLE_OPENCV_NONFREE` isn't available due to OpenCV.js doesn't support non-free module & algorithm yet. [Checkout this list of available features](https://github.com/opencv/opencv/blob/4.x/platforms/js/opencv_js.config.py)

> Regarding to the registration algorithms available in WebAssembly, we have following algorithms:
> * SIFT
> * ORB
> * AKAZE
> * BRISK
> 
> [Checkout MoonRegistrate.js for detail](./moon-registration/MoonRegistrate.js)


# Limitation & Known Issues

1. WebAssembly version of the MoonDetect module will produce different result (compared with C++ version) for some file, and for some other files it works fine.
   * This might be due to how we transfer image from JavaScript side to WebAssembly side. Checkout [image_handler.js](./moon-registration/image_handler.js) and [utils_wasm.cpp](./src/utils_wasm.cpp) for detail implementation
2. In WebAssembly, we have no way to control in a guaranteed fashion when new memory commit vs address space reserve occurs.
   * Which means, we have no way to know if a newly allocated memory block is ready to use or not, every time a heap allocation happens, it has a chance to trigger "memory access out of bounds" error in JavaScript. This is because we might accessing the memory when it isn't ready.
   * [Detailed article](https://github.com/WebAssembly/design/issues/1397)
> Workarounds for above limitation (use all of them):
> 1. de-allocate memory right after we are done with it
>    * call `destroy_***` function right after we are done with the variable
> 2. create an `<iframe>`, and put all the WebAssembly related code inside. So, every time we get "memory access out of bounds", we just refresh the `<iframe>`

