
# MoonRegistration

A Cross-Platform Multi-Language library for Moon Location Detection & Moon Image Registration.

# What it does?

* Given an input image

![input image](./imgs/00_input_image.png)

## MoonDetect

* Tries to detect moon's location on the image

![moon detector](./imgs/01_moon_detector.png)

## MoonRegistrate

* Using a provided model image as reference, run image registration algorithm on the moon part, and tries to find matching points

![registrate user image](./imgs/01_moon_registrate_matched_keypoints.png)

* So we can compute a homography matrix from these matching points, and "rotate" images as we want

* Rotate original image to match model image

![rotate original image](./imgs/02_moon_registrate_registrate_user_image.png)

* Color code original image and model image, so we can stack them together to see the error

![green model image (rotated to original image's perspective)](./imgs/03_moon_registrate_green_model_image.png)
![red original image](./imgs/04_moon_registrate_red_transformed_user_image.png)
![stacked image](./imgs/05_moon_registrate_stacked_red_green_image.png)

* Draw a layer on top of original image

![stack layer on top of original image](./imgs/06_moon_registrate_layer_image.png)

# Supported Language

|                                  | MoonDetect Module | MoonRegistrate Module |
|----------------------------------|-------------------|----------------------|
| C++                              | ✅                 | ✅                    |
| C (with abstracted api)          | ✅                 | ✅                    |
| Python3                          | ✅                 | ❌                    |
| JavaScript (with abstracted api) | ✅                 | ❌                    |


# Known Issues

1. When building the library into WebAssembly, it will produce mismatched result (compared with C++ version) for some file, and for some other files it works perfectly.

# [Usage Demonstration](./demo/README.md)

# [Building the Library](./BUILDING.md)

# [See Changelog](./CHANGELOG.md)
