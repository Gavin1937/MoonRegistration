
# MoonRegistration Python

Python wrapper for MoonRegistration Library.

# Supporting Python Version >= 3.8

# Build

## Build With Docker (Recommend)

If you just want to try out this library, you can build it with docker using following commands.

1. Under the repository root, build Docker image

```sh
docker build -t moonregistration_python -f ./dockerfiles/python_demo.dockerfile .
```

2. Run Docker container

```sh
docker run -it --rm --name moonregistration_python moonregistration_python
```

1. The Docker container you just launched contains MoonRegistration library in its python environment, you can goto [demo folder](../../demo/README.md) under the repository root to play with all the demo scripts. Checkout [Usage Section for more detail](#usage)

## Build & Install From Source

To build this library from source and install it to your python environment, you need to:

1. Install all the dependencies and make sure you can [build the MoonRegistration C++ library](../../BUILDING.md#build-into-library)
   * You need to have a C++ compiler, [CMake >= 3.11](https://cmake.org/), OpenCV, and [Python >= 3.8](https://www.python.org/) (with numpy, it will be install by setup.py)
   * You can find install tutorial in [BUILDING.md](../../BUILDING.md)
   * **For the ease of dependency searching, this package will looking for environment variables**
     * As suggested in [BUILDING.md Dependencies section](../../BUILDING.md#build-into-library), you should set an environment variable for OpenCV root directory, so python can correctly find OpenCV libraries.
2. Under `platforms/python` folder, use following command to build and install this python library

```sh
python setup.py install
```

> Note: you can append argument `--mr-enable-opencv-nonfree` after this command to enable `MR_ENABLE_OPENCV_NONFREE` CMake flag, checkout [CMake Build Arguments](../../BUILDING.md#cmake-build-arguments) for detail.

3. [Optional] You can build this library into a python .whl package, but it will be tie to your python version and cannot install/run in an environment with different python version.
   * The argument `--py-limited-api cp38` will set the minimum supported cpython version to cpython 3.8

```sh
python setup.py bdist_wheel --py-limited-api cp38
```

4. [Optional] You can build only the C++ part of this library using CMake with following commands.

```sh
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config=Release
```

### Python Dependencies

| Name                  | Version     | Description                                                     |
|-----------------------|-------------|-----------------------------------------------------------------|
| numpy                 | >= 1.23.5   | install before compile, C++ dependency for data type conversion |
| opencv-python         | >= 4.8.0.76 | python opencv package, you can use it with this library         |
| opencv-contrib-python | >= 4.8.0.76 | python opencv package, you can use it with this library         |

# Usage

Once you installed this library into your python environment, you can import it with:

```py
import MoonRegistration as mr
```

You can checkout [demo folder](../../demo/README.md) for some simple usages.

# Special Thanks

* [pybind11 for easy C++ & Python binding](https://github.com/pybind/pybind11)
* [pybind11_opencv_numpy by edmBernard](https://github.com/edmBernard/pybind11_opencv_numpy) for easy conversion between cv::Mat and cv2.MatLike (np.ndarray)

