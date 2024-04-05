import { instance } from './wasm_loader.js';
import { get_c_str, get_cpp_exception } from './internal.js';

export { MAX_C_STRING_LENGTH } from './internal.js';
export { ImageHandler } from './image_handler.js';
export {
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
} from './shapes.js';
export {
  cut_image_from_circle,
  stack_imgs,
} from './imgprocess.js';
export {
  detect_moon,
} from './MoonDetect.js';
export {
  RegistrationAlgorithms,
  transform_user_image,
  transform_layer_image,
  draw_layer_image,
  draw_layer_image_no_compute,
} from './MoonRegistrate.js';

/**
 * Get MoonRegistration library version
 * 
 * @returns {Promise<String>} version string
 */
async function version() {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function(){
      try {
        let ptr = await instance._mrwasm_version();
        let output = await get_c_str(ptr, 20);
        
        resolve(output);
      } catch (error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}

export { instance, get_c_str, get_cpp_exception, version };
