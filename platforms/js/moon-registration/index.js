import { instance } from './wasm_loader.js';
import { get_c_str, get_cpp_exception } from './internal.js';
import { MR_VERSION_STR } from './constants.js';

export { MAX_C_STRING_LENGTH } from './internal.js';
export {
  MR_VERSION_STR,
  MR_HAVE_HOUGH_GRADIENT_ALT,
  MR_ENABLE_OPENCV_NONFREE,
  MR_HAVE_OPENCV_NONFREE,
} from './constants.js';
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
  compute_registration,
  draw_layer_image,
  draw_layer_image_no_compute,
} from './MoonRegistrate.js';

/**
 * Get MoonRegistration library version
 * 
 * @returns {String} version string
 */
function version() {
  return MR_VERSION_STR;
}

export { instance, get_c_str, get_cpp_exception, version };
