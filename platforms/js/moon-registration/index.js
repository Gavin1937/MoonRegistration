import { instance } from './wasm_loader.js';

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
  draw_layer_image,
  RegistrationAlgorithms,
  transform_user_image,
  transform_layer_image
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
        let peak_bytes = 40;
        let data = new Uint8Array(instance.HEAP8.buffer, ptr, peak_bytes);
        let output = '';
        for (let i = 0; i < peak_bytes; i+=1) {
          if (data[i] == 0)
            break;
          output += String.fromCharCode(data[i]);
        }
        
        resolve(output);
      } catch (error) {
        reject(error);
      }
    });
  });
}

export { instance, version };
