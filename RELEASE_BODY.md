This release page contains all kind of version of the library.

All file names follow this convention:

```
{library}-{version}-{system}-{architecture}-{other attributes}.zip
```

> `other attributes` can be empty

For example, in the following file:

```
MoonRegistration-js-v0.1.10-linux-x86_64_64-detect.zip
```

* library => `MoonRegistration-js`
* version => `v0.1.10`
* system => `linux`
* architecture => `x86_64_64`
* other attributes => `detect`

Currently, we have 3 libraries:
* **MoonRegistration** => C++ & C library
* **MoonRegistration-python** => Python library
* **MoonRegistration-js** => JavaScript library

### MoonRegistration

only have one attribute differentiate free version and nonfree version

* It is `nonfree` when its compiled with OpenCV non-free modules & algorithms.
* It is empty when its not compiled with those algorithms. [Checkout this doc for detail](https://github.com/Gavin1937/MoonRegistration/blob/main/BUILDING.md#about-opencv-non-free-modules)

### MoonRegistration-python

only have one attribute differentiate free version and nonfree version

* It is `nonfree` when its compiled with OpenCV non-free modules & algorithms.
* It is empty when its not compiled with those algorithms. [Checkout this doc for detail](https://github.com/Gavin1937/MoonRegistration/blob/main/BUILDING.md#about-opencv-non-free-modules)

### MoonRegistration-js

only have one attribute differentiate the library modules it contains

* It is `detect` when its compiled with MoonDetect module only.
* It is `registrate` when its not compiled with both MoonDetect and MoonRegistrate modules.
