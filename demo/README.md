# MoonRegistration Library Demonstration


## Before we start

* **You can use dataset in [./data](./data) folder**
  * datasets are divided into folders for testing different modules
* **You can play with demo applications inside [Docker Container](../BUILDING.md#build-using-docker-recommend)**
* **All demo code are store in their folder same as language name**

## C++ & C

| Name                                                                                 | Module         | Description                                                          |
|--------------------------------------------------------------------------------------|----------------|----------------------------------------------------------------------|
| [MoonDetect_basic.cpp](./cpp_c/MoonDetect_basic.cpp)                                 | MoonDetect     | A basic usage, easy and quick                                        |
| [MoonDetect_advance.cpp](./cpp_c/MoonDetect_advance.cpp)                             | MoonDetect     | An advanced usage, you can further customize moon image detection    |
| [MoonDetect_c_api.c](./cpp_c/MoonDetect_c_api.c)                                     | MoonDetect     | A basic usage of the C abstraction API                               |
| [MoonRegistrate_basic.cpp](./cpp_c/MoonRegistrate_basic.cpp)                         | MoonRegistrate | A basic usage, easy and quick                                        |
| [MoonRegistrate_advance.cpp](./cpp_c/MoonRegistrate_advance.cpp)                     | MoonRegistrate | An advanced usage, you can further customize moon image registration |
| [MoonRegistrate_live_registration.cpp](./cpp_c/MoonRegistrate_live_registration.cpp) | MoonRegistrate | Running moon image registration on a live video                      |
| [MoonRegistrate_c_api.c](./cpp_c/MoonRegistrate_c_api.c)                             | MoonRegistrate | A basic usage of the C abstraction API                               |

### Building demos (for C++ & C)

To build all the demos:

1. create a new directory "build"

```sh
mkdir build
```

2. setup cmake (assuming you are in `./demo` folder)

```sh
cmake -S . -B build
```

3. build projects (assuming you are in `./demo` folder)

```sh
cmake --build
```

4. play with demo applications, they are in `./build/bin` folder

**Note that demo need to build with C++ >= 14 because they need the filesystem module**

**Checkout the [CMakeLists.txt](./CMakeLists.txt) file for examples of compiling & linking this library**


## Python

| Name                                                                                | Module         | Description                                                                                                  |
|-------------------------------------------------------------------------------------|----------------|--------------------------------------------------------------------------------------------------------------|
| [MoonDetect_basic.py](./python/MoonDetect_basic.py)                                 | MoonDetect     | A basic usage of MoonRegistration Python Wrapper api                                                         |
| [MoonDetect_advance.py](./python/MoonDetect_advance.py)                             | MoonDetect     | An advanced usage of MoonRegistration Python Wrapper api, you can customize steps in moon detection          |
| [MoonRegistrate_basic.py](./python/MoonRegistrate_basic.py)                         | MoonRegistrate | A basic usage of MoonRegistration Python Wrapper api                                                         |
| [MoonRegistrate_advance.py](./python/MoonRegistrate_advance.py)                     | MoonRegistrate | An advanced usage of MoonRegistration Python Wrapper api, you can customize steps in moon image registration |
| [MoonRegistrate_live_registration.py](./python/MoonRegistrate_live_registration.py) | MoonRegistrate | Running moon image registration on a live video                                                              |

To play with these demo applications,

1. Install [MoonRegistration Python wrapper library](../platforms/python/README.md) following  its doc.
2. Once you have the `MoonRegistration` package in your python environment, simply run:

```sh
python3 /path/to/demo_name.py
```

3. You can import the library with:

```py
import MoonRegistration as mr
```

