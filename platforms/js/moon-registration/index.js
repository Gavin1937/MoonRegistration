export {
  // wasm_loader.js
  instance,
  version,
  // image_handler.js
  ImageHandler,
  // shapes.js
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
  // imgprocess.js
  cut_image_from_circle,
  // MoonDetect.js
  detect_moon,
  // MoonRegistrate.js
  draw_layer_image,
  RegistrationAlgorithms,
};

import { instance } from './wasm_loader.js';
import { ImageHandler } from './image_handler.js';
import {
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
} from './shapes.js';
import { cut_image_from_circle } from './imgprocess.js';
import { detect_moon } from './MoonDetect.js';
import {
  draw_layer_image,
  RegistrationAlgorithms,
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
