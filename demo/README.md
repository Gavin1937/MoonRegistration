# MoonRegistration Library Demonstration


## Before we start

* **You can use dataset in [./data](./data) folder**
  * datasets are divided into folders for testing different modules
* **You can play with demo applications inside [Docker Container](../BUILDING.md#build-using-docker-recommend)**
* **All demo code are store in their folder same as language name**

## C++ & C

| Name                                                                               | Module        | Description                                                          |
|------------------------------------------------------------------------------------|---------------|----------------------------------------------------------------------|
| [MoonDetector_basic.cpp](./cpp_c/MoonDetector_basic.cpp)                           | MoonDetector  | A basic usage, easy and quick                                        |
| [MoonDetector_advance.cpp](./cpp_c/MoonDetector_advance.cpp)                       | MoonDetector  | An advanced usage, you can further customize moon image detection    |
| [MoonRegistrar_basic.cpp](./cpp_c/MoonRegistrar_basic.cpp)                         | MoonRegistrar | A basic usage, easy and quick                                        |
| [MoonRegistrar_advance.cpp](./cpp_c/MoonRegistrar_advance.cpp)                     | MoonRegistrar | An advanced usage, you can further customize moon image registration |
| [MoonRegistrar_live_registration.cpp](./cpp_c/MoonRegistrar_live_registration.cpp) | MoonRegistrar | Running moon image registration on a live video                      |
| [MoonDetector_c_api.c](./cpp_c/MoonDetector_c_api.c)                               | MoonDetector  | A basic usage of the C abstraction API                               |

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

| Name                                                        | Module       | Description                                                                                              |
|-------------------------------------------------------------|--------------|----------------------------------------------------------------------------------------------------------|
| [MoonDetector_basic.py](./python/MoonDetector_basic.py)     | MoonDetector | A basic usage of MoonRegistration Python Wrapper api                                                     |
| [MoonDetector_advance.py](./python/MoonDetector_advance.py) | MoonDetector | An advanced usage of MoonRegistration Python Wrapper api, you can customize each steps in moon detection |

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

