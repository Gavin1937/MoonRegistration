
# MoonRegistration JavaScript / WASM

JavaScript wrapper for MoonRegistration WebAssembly Library.

# Build

## Build With Docker (Recommend)

1. Under the repository root, build Docker image with:

```sh
docker build -t moonregistration_wasm -f .\Dockerfile_wasm .
```

2. once docker image is built, all MoonRegistration library is inside the container, launch the container by:

```sh
docker run -it --rm --name moonregistration_wasm moonregistration_wasm
```

3. built library binaries are inside folder `/src/MoonRegistration/platforms/js/build_wasm`

all the build environments are set, so you can easily rebuild the library following [these steps](#build-into-webassembly-library) and modify [build arguments](#cmake-build-arguments)

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
4. create a new directory called **build_wasm** (you shouldn't change this name)
5. build opencv into WebAssembly (this will take a while)

```sh
mkdir build_wasm
python ./platforms/js/build_js.py build_wasm --build_wasm
```

* Also note that, you can add cmake flag for opencv by supplying python flag `--cmake_option`. This flag allows you to build opencv wasm library with opencv_contrib and other opencv flags.
  * [learn more in here](https://docs.opencv.org/4.x/d4/da1/tutorial_js_setup.html)
  * [Checkout Dockerfile_wasm for some example](../../Dockerfile_wasm)

6. go to MoonRegistration folder 
7. create a new directory called **build_wasm** and go inside it
8. build MoonRegistration wasm

CMake initialize

```sh
mkdir build_wasm && cd build_wasm
emcmake cmake -DOPENCV_SRC="/some/root/opencv" ..
```

* **You need to supply a cmake flag `-DOPENCV_SRC`, which is point to opencv source folder (parent of opencv's build_wasm folder)**

Make build

```sh
emmake make
```

9. once building is done, you can start using `MoonRegistration.js` and `MoonRegistration.wasm`


# Known Issues

1. When building the library into WebAssembly, it will produce mismatched result (compared with C++ version) for some file, and for some other files it works perfectly.

