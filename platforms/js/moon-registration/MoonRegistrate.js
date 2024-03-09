export { draw_layer_image, RegistrationAlgorithms };

import { instance } from './wasm_loader.js';
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
 * Run mr::MoonRegistrar::draw_layer_image on input image
 * 
 * @param {ImageHandler} user_image_handler ImageHandler of user_image
 * @param {ImageHandler} model_image_handler ImageHandler of model_image
 * @param {ImageHandler} layer_image_handler ImageHandler of layer_image
 * @param {int} algorithm use class RegistrationAlgorithms to select the algorithm for registration, can be:
 * SIFT, ORB, AKAZE, BRISK, EMPTY_ALGORITHM, INVALID_ALGORITHM
 * @param {float} layer_image_transparency a 0~1 float percentage changing layer image's transparency
 * @param {int} filter_px a 4-bytes integer that represents BGRA value of a pixel in each of its bytes.
 * function will use it to filter the pixel in layer image, set it to -1 if you don't need it.
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
        reject(error);
      }
    });
  });
}
