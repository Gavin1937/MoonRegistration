export { 
  HoughCirclesAlgorithms,
  detect_moon,
};

import { instance } from './wasm_loader.js';
import { get_cpp_exception } from './internal.js';
import { MR_HAVE_HOUGH_GRADIENT_ALT } from './constants.js';
import { Circle } from './shapes.js';
import { ImageHandler } from './image_handler.js';


// Starting from OpenCV 4.8.1, algorithm HOUGH_GRADIENT_ALT is available for cv::HoughCircles().
// This enum will be enabled if OpenCV version >= 4.8.1
class HoughCirclesAlgorithms {
  // Private Fields
  static #_HOUGH_GRADIENT        = 0x101;
  // use cv::HOUGH_GRADIENT_ALT with basic optimization
  static #_HOUGH_GRADIENT_ALT    = 0x102;
  // use cv::HOUGH_GRADIENT and cv::HOUGH_GRADIENT_ALT together for the best result
  static #_HOUGH_GRADIENT_MIX    = 0x103;
  static #_EMPTY_ALGORITHM       = 0x001;
  static #_INVALID_ALGORITHM     = 0x000;

  // Accessors for "get" functions only (no "set" functions)
  static get HOUGH_GRADIENT() { return this.#_HOUGH_GRADIENT; }
  static get HOUGH_GRADIENT_ALT() {
    if (!MR_HAVE_HOUGH_GRADIENT_ALT)
      throw new Error("MoonRegistration did not compiled with OpenCV >= 4.8.1");
    return this.#_HOUGH_GRADIENT_ALT;
  }
  static get HOUGH_GRADIENT_MIX() {
    if (!MR_HAVE_HOUGH_GRADIENT_ALT)
      throw new Error("MoonRegistration did not compiled with OpenCV >= 4.8.1");
    return this.#_HOUGH_GRADIENT_MIX;
  }
  static get EMPTY_ALGORITHM() { return this.#_EMPTY_ALGORITHM; }
  static get INVALID_ALGORITHM() { return this.#_INVALID_ALGORITHM; }
}

/**
 * Run mr::MoonDetector::detect_moon on input image
 * 
 * @param {ImageHandler} image_handler input image
 * @param {int} algorithm use class RegistrationAlgorithms to select the algorithm for circle detection, can be:
 * HOUGH_GRADIENT, HOUGH_GRADIENT_ALT, HOUGH_GRADIENT_MIX, EMPTY_ALGORITHM, INVALID_ALGORITHM,
 * Algorithms HOUGH_GRADIENT_ALT and HOUGH_GRADIENT_MIX only available when compiling with OpenCV >= 4.8.1
 * If this parameter is set to -1, this function will use a default algorithm base on the OpenCV version:
 * If compiled with OpenCV < 4.8.1, we will use HOUGH_GRADIENT algorithm by default
 * If compiled with OpenCV >= 4.8.1, we will use HOUGH_GRADIENT_MIX algorithm by default
 * @returns {Promise<Circle>} circle found
 */
async function detect_moon(
  image_handler,
  algorithm = -1
) {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        if (algorithm === -1 && MR_HAVE_HOUGH_GRADIENT_ALT)
          algorithm = HoughCirclesAlgorithms.HOUGH_GRADIENT_MIX;
        else if (algorithm === -1 && !MR_HAVE_HOUGH_GRADIENT_ALT)
          algorithm = HoughCirclesAlgorithms.HOUGH_GRADIENT;
        
        // run moon_detection
        let result_ptr = await instance._mrwasm_detect_moon(
          image_handler.image_ptr, algorithm
        );
        
        let ret = new Circle();
        await ret.load_from_ptr(result_ptr);
        resolve(ret);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}
