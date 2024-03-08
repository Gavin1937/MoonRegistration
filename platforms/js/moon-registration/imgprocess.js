export { cut_image_from_circle };

import { instance } from './wasm_loader.js';
import { ImageHandler } from './image_handler.js';
import { Circle, Rectangle } from './shapes.js';


/**
 * Cut a square image using input circle and output it as a copy of input image
 * 
 * @param {ImageHandler} image_in ImageHandler input
 * @param {Circle} circle Circle input
 * @param {int} padding add some padding pixels around input circle
 * @returns {Promise<Array<ImageHandler, Rectangle>>} returns [image_out, rect_out]
 */
async function cut_image_from_circle(
  image_in,
  circle,
  padding = 15
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function () {
      try {
        let ptr = await instance._mrwasm_cut_image_from_circle(
          image_in.image_ptr,
          circle.x, circle.y, circle.radius,
          padding
        );
        
        const data_list = new Int32Array(instance.HEAP32.buffer, ptr, 2);
        let image_out = new ImageHandler();  
        let rect_out = new Rectangle();
        await image_out.load_from_ImageHandlerData(data_list[0]);
        await rect_out.load_from_ptr(data_list[1]);
        
        await instance._mrwasm_destroy_RectangleAndImageHandlerData(ptr);
        
        resolve([image_out, rect_out]);
      } catch (error) {
        reject(error);
      }
    });
  });
}
