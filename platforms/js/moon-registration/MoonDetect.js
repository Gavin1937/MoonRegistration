export { detect_moon };

import { instance } from './wasm_loader.js';
import { Circle } from './shapes.js';
import { ImageHandler } from './image_handler.js';


/**
 * Run mr::MoonDetector::detect_moon on input image
 * 
 * @param {ImageHandler} image_handler input image
 * @returns {Promise<Circle>} circle found
 */
async function detect_moon(image_handler) {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        // run moon_detection
        let result_ptr = await instance._mrwasm_detect_moon(
          image_handler.image_ptr
        );
        
        let ret = new Circle();
        await ret.load_from_ptr(result_ptr);
        resolve(ret);
      } catch (error) {
        reject(error);
      }
    });
  });
}
