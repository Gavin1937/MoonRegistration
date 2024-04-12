export {
  RegistrationAlgorithms,
  transform_user_image,
  transform_layer_image,
  draw_layer_image,
  draw_layer_image_no_compute,
};

import { instance } from './wasm_loader.js';
import { get_cpp_exception } from './internal.js';
import { ImageHandler } from './image_handler.js';


class RegistrationAlgorithms {
  // Private Fields
  static #_SIFT                  = 0x100;
  static #_ORB                   = 0x101;
  static #_AKAZE                 = 0x102;
  static #_BRISK                 = 0x103;
  static #_EMPTY_ALGORITHM       = 0x001;
  static #_INVALID_ALGORITHM     = 0x000;
  
  // Accessors for "get" functions only (no "set" functions)
  static get SIFT() { return this.#_SIFT; }
  static get ORB() { return this.#_ORB; }
  static get AKAZE() { return this.#_AKAZE; }
  static get BRISK() { return this.#_BRISK; }
  static get EMPTY_ALGORITHM() { return this.#_EMPTY_ALGORITHM; }
  static get INVALID_ALGORITHM() { return this.#_INVALID_ALGORITHM; }
}

/**
 * Same as mr::transform_image(), but using user_image as image_in.
 * Using computed homography_matrix to apply a perspective transformation to user_image.
 * "Rotate" user_image to match model_image's perspective.
 * 
 * @param {ImageHandler} user_image_handler ImageHandler of user_image
 * @param {ImageHandler} model_image_handler ImageHandler of model_image
 * @param {int} algorithm use class RegistrationAlgorithms to select the algorithm for registration, can be:
 * SIFT, ORB, AKAZE, BRISK, EMPTY_ALGORITHM, INVALID_ALGORITHM
 * Default SIFT
 * @returns {Promise<ImageHandler>} output ImageHandler object
 */
async function transform_user_image(
  user_image_handler,
  model_image_handler,
  algorithm = RegistrationAlgorithms.SIFT
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let ptr = await instance._mrwasm_transform_user_image(
          user_image_handler.image_ptr,
          model_image_handler.image_ptr,
          algorithm
        );
        let ret = new ImageHandler();
        await ret.load_from_ImageHandlerData(ptr);
        await instance._mrwasm_destroy_ImageHandlerData(ptr);
        
        resolve(ret);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}

/**
 * Wrapper around mr::transform_image_inverse(), transform any layer image.
 * Using computed homography_matrix to transform layer_image_in in the perspective of user_image.
 * "Rotate" layer_image_in to match user_image's perspective.
 * 
 * @param {ImageHandler} user_image_handler ImageHandler of user_image
 * @param {ImageHandler} model_image_handler ImageHandler of model_image
 * @param {ImageHandler} layer_image_handler ImageHandler of layer_image
 * @param {int} algorithm use class RegistrationAlgorithms to select the algorithm for registration, can be:
 * SIFT, ORB, AKAZE, BRISK, EMPTY_ALGORITHM, INVALID_ALGORITHM
 * Default SIFT
 * @returns {Promise<ImageHandler>} output ImageHandler object
 */
async function transform_layer_image(
  user_image_handler,
  model_image_handler,
  layer_image_handler,
  algorithm = RegistrationAlgorithms.SIFT
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let ptr = await instance._mrwasm_transform_layer_image(
          user_image_handler.image_ptr,
          model_image_handler.image_ptr,
          layer_image_handler.image_ptr,
          algorithm
        );
        let ret = new ImageHandler();
        await ret.load_from_ImageHandlerData(ptr);
        await instance._mrwasm_destroy_ImageHandlerData(ptr);
        
        resolve(ret);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}

/**
 * Run mr::MoonRegistrar::draw_layer_image on input image
 * 
 * @param {ImageHandler} user_image_handler ImageHandler of user_image
 * @param {ImageHandler} model_image_handler ImageHandler of model_image
 * @param {ImageHandler} layer_image_handler ImageHandler of layer_image
 * @param {int} algorithm use class RegistrationAlgorithms to select the algorithm for registration, can be:
 * SIFT, ORB, AKAZE, BRISK, EMPTY_ALGORITHM, INVALID_ALGORITHM
 * Default SIFT
 * @param {float} layer_image_transparency a 0~1 float percentage changing layer image's transparency
 * @param {int} filter_px a 4-bytes integer that represents BGRA value of a pixel in each of its bytes.
 * function will use it to filter the pixel in layer image. A pixel will be ignore when all of its values
 * is <= filter_px. Set it to -1 if you don't need it.
 * Note that integer are processed in little-endian, so it should looks like: (A,R,G,B)
 * @returns {Promise<ImageHandler>} output ImageHandler object
 */
async function draw_layer_image(
  user_image_handler,
  model_image_handler,
  layer_image_handler,
  algorithm = RegistrationAlgorithms.SIFT,
  layer_image_transparency = 1.0,
  filter_px = -1
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let ptr = await instance._mrwasm_draw_layer_image(
          user_image_handler.image_ptr,
          model_image_handler.image_ptr,
          layer_image_handler.image_ptr,
          algorithm,
          layer_image_transparency,
          filter_px
        );
        let ret = new ImageHandler();
        await ret.load_from_ImageHandlerData(ptr);
        await instance._mrwasm_destroy_ImageHandlerData(ptr);
        
        resolve(ret);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}

/**
 * Run mr::MoonRegistrar::draw_layer_image on input image.
 * However, we do not run mr::MoonRegistrar::compute_registration() and
 * use an input homography_matrix to setup mr::MoonRegistrar class.
 * 
 * @param {ImageHandler} user_image_handler ImageHandler of user_image
 * @param {ImageHandler} model_image_handler ImageHandler of model_image
 * @param {ImageHandler} layer_image_handler ImageHandler of layer_image
 * @param {Array<Array<float>>} homography_matrix a 3x3 2d array of homography_matrix
 * @param {float} layer_image_transparency a 0~1 float percentage changing layer image's transparency
 * @param {int} filter_px a 4-bytes integer that represents BGRA value of a pixel in each of its bytes.
 * function will use it to filter the pixel in layer image. A pixel will be ignore when all of its values
 * is <= filter_px. Set it to -1 if you don't need it.
 * Note that integer are processed in little-endian, so it should looks like: (A,R,G,B)
 * @returns {Promise<ImageHandler>} output ImageHandler object
 */
async function draw_layer_image_no_compute(
  user_image_handler,
  model_image_handler,
  layer_image_handler,
  homography_matrix,
  layer_image_transparency = 1.0,
  filter_px = -1
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        homography_matrix = new Float32Array(homography_matrix.flat());
        let homography_matrix_buffer_ptr = await instance._mrwasm_create_image_buffer(
          homography_matrix.length * homography_matrix.BYTES_PER_ELEMENT
        );
        
        await instance.HEAPF32.set(homography_matrix, homography_matrix_buffer_ptr/4);
        
        let homography_matrix_ptr = await instance._mrwasm_create_homography_matrix_ptr(
          homography_matrix_buffer_ptr
        );
        
        let ptr = await instance._mrwasm_draw_layer_image_no_compute(
          user_image_handler.image_ptr,
          model_image_handler.image_ptr,
          layer_image_handler.image_ptr,
          homography_matrix_ptr,
          layer_image_transparency,
          filter_px
        );
        
        let ret = new ImageHandler();
        await ret.load_from_ImageHandlerData(ptr);
        
        await instance._mrwasm_destroy_ImageHandlerData(ptr);
        await instance._mrwasm_destroy_homography_matrix_ptr(homography_matrix_ptr);
        await instance._mrwasm_destroy_image(homography_matrix_buffer_ptr);
        
        resolve(ret);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}
