
# CHANGELOG

<details open>
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


