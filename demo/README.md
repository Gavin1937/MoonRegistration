# MoonRegistration Library Demonstration


## Demo Explanation

| Name                                                   | Module       | Description                                                       |
|--------------------------------------------------------|--------------|-------------------------------------------------------------------|
| [MoonDetector_basic.cpp](./MoonDetector_basic.cpp)     | MoonDetector | A basic usage, easy and quick                                     |
| [MoonDetector_advance.cpp](./MoonDetector_advance.cpp) | MoonDetector | An advanced usage, you can customize each steps in moon detection |
| [MoonDetector_basic.py](./MoonDetector_basic.py)       | MoonDetector | A basic usage of MoonRegistration Python Wrapper api              |


## Build demos (for C++)

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

**You can use dataset in [./data](./data) folder**

**You can play with demo applications inside [Docker Container](../BUILDING.md#build-using-docker-recommend)**

