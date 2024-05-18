
# CHANGELOG

<details open>
<summary><strong style="font-size:140%">v1.0.1</strong></summary>

* Bug fixes
  * in js interface, force `filter_px` to be Number
  * fix CMake exposing wrong include dir
  * try fix python release scripts
* Others
  * add invalid circle (x == -1 && y == -1 && radius == -1) handling code
  * add vscode cpp extension to dev container
  * make install package compatible with cmake `find_package()`
  * allow CMakeLists.txt in demo folder to handle `-DOpenCV_DIR` flag

</details>


<details>
<summary><strong style="font-size:140%">v1.0.0</strong></summary>

* Interface change
  * add overload method for `sync_img_size()`
  * update `stack_imgs()` interface in JS
  * add abstraction interface for ImageHandler to blob obj
  * add `draw_layer_image_no_compute()` for JS, draw layer img with input homography matrix
  * add `compute_registration()` for JS, only compute homography matrix and return it
  * change some functions in `shapes` module to overload function
  * add shape to string functions
  * add `default_steps` for both `MoonDetect` & `MoonRegistrate`, all the default step functions are store there
  * add `binarize_image()` to `imgprocess` module
  * update `cut_image_from_circle()` interface
  * update `calc_circle_brightness_perc()` interface
  * make C++ macros available in JS & Python. JS & Python interface will change base on macro settings
  * rename `HoughCirclesAlgorithm` enum
* Behavior change
  * update `filter_px` parameter behavior, its now filtering pixels <= input `filter_px`
  * update `sync_img_size()` behavior
* Optimization
  * optimize C++ exception handling for JS
  * optimize `MoonDetect` module: improve preprocessing, and also add optimized step functions for `HOUGH_GRADIENT`, `HOUGH_GRADIENT_ALT`, and `HOUGH_GRADIENT_MIX` algorithm
  * use random sampling for find_circles when it excess circle_threshold
  * optimize `MoonRegistrate` module: add more filtering algorithms & improve the filter process
* Bug fixes
  * fix `stack_imgs()` shift foreground img in a weird way when foreground and background have different size
  * fix `stack_imgs()` background_roi update logic
  * fix bug on `cv::merge() size not match`
  * fix image color goes wrong when transferring between C++ & JS
  * fix `sync_img_size()` roi not found issue
  * fix some memory management bugs in wasm wrappers
  * fix `stack_imgs()` shifting foreground in some case
* Others
  * update doc
  * general bug fix
  * general code base cleanup

</details>


<details>
<summary><strong style="font-size:140%">v0.1.10</strong></summary>

* add new features to js api
  * stack_imgs
  * transform_layer_image
  * transform_user_image
  * draw_layer_image_no_compute
* add new setter api to MoonRegistrar
  * update_homography_matrix
  * update_good_keypoint_matches
* implement new release scripts
* bug fixes
* making repo cleaner

</details>


<details>
<summary><strong style="font-size:140%">v0.1.9</strong></summary>

* add proper javascript wrapper layer for the library

</details>


<details>
<summary><strong style="font-size:140%">v0.1.8</strong></summary>

* add python wrapper for the library
* further divide the library into smaller modules and rename them so they make sense
* add HOUGH_CIRCLE_ALT support for circle detection (not optimized yet)
* fix a few bugs

</details>


<details>
<summary><strong style="font-size:140%">v0.1.7</strong></summary>

* Re-structured project so we can:
  * abstract WASM related code to `platform/js`
  * abstract C api to `src/c_mrapi`
* Update OpenCV to version `4.9.0`
* Implement MoonRegistrar module, demo, & doc
* Further abstract functions so we can reuse them
* Fix some bugs

</details>


<details>
<summary><strong style="font-size:140%">v0.1.6</strong></summary>

* Improve overall error handling in this library
* Extract C API out and make it a single unit
  * We now only support an abstracted version of C API because:
> Since C++ version of this library is heavily rely on
> C++ features like namespace & class, plus we relies on
> OpenCV's C++ API. So providing a full API of the library
> for C is time consuming. Thus, we only provide an
> abstracted version of this library.

</details>


<details>
<summary><strong style="font-size:140%">v0.1.5</strong></summary>

* Add python wrapper for the entire library
* Use both `select_n_circles_by_largest_radius` and `select_circle_by_brightness_perc` in the 1st iteration of circle selection
* Add `cut_ref_image_from_circle` to handle OpenCV cutting image reference

</details>


<details>
<summary><strong style="font-size:140%">v0.1.4</strong></summary>

* Update HoughCircles parameter further more in each iteration
* Fix links in doc

</details>


<details>
<summary><strong style="font-size:140%">v0.1.3</strong></summary>

* Update Github Action

</details>


<details>
<summary><strong style="font-size:140%">v0.1.2</strong></summary>

* Update Github Action release filename

</details>


<details>
<summary><strong style="font-size:140%">v0.1.1</strong></summary>

* Implement Moon Detection & utilities around it
* Add demo folder
* Make Moon Detection available in WASM
* Dockerize the entire build process

</details>


