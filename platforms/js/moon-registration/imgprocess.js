export {
  cut_image_from_circle,
  stack_imgs,
};

import { instance } from './wasm_loader.js';
import { get_cpp_exception } from './internal.js';
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
        reject(await get_cpp_exception(error));
      }
    });
  });
}


/**
 * Stack two images together respecting transparency
 * 
 * @param {ImageHandler} background_image_handler background image handler object
 * @param {Array<int>} background_roi Region of Interest (ROI) of background image [x, y, width, height]
 * @param {ImageHandler} foreground_image_handler foreground image handler object
 * @param {float} foreground_transparency a 0~1 float percentage changing foreground image's transparency, default 1.0
 * @param {int} filter_px a 4-bytes integer that represents BGRA value of a pixel in each of its bytes.
 * function will use it to filter the pixel in foreground image, set it to -1 if you don't need it.
 * Note that integer are processed in little-endian, so it should looks like: (A,R,G,B)
 * 
 * Note:
 *   - this function is designed to work with different number of color channels
 *   - if foreground size is bigger then background_roi size, this function will call mr::sync_img_size()
 *     to sync it with background_roi. After sync, this function will place foreground at the center of
 *     background_roi automatically.
 *   - output image will follow the maximum number of channels between two input images,
 *     thus if one input is grayscale image and another is not, then it will become
 *     the blue channel of image_out, as OpenCV uses BGRA pixel order.
 *   - when using filter_px, we only read number of elements corresponding to
 *     number of channels in foreground. if foreground image has 3 channels, we only read
 *     first 3 elements of filter_px. rest of elements will be ignored.
 * 
 * @returns {Promise<ImageHandler>} output ImageHandler object
 */
async function stack_imgs(
  background_image_handler,
  background_roi,
  foreground_image_handler,
  foreground_transparency = 1.0,
  filter_px = -1
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function () {
      try {
        let ptr = await instance._mrwasm_stack_imgs(
          background_image_handler.image_ptr,
          background_roi[0], background_roi[1],
          background_roi[2], background_roi[3],
          foreground_image_handler.image_ptr,
          foreground_transparency,
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
