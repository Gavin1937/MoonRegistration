export { draw_layer_image };

import { instance } from './wasm_loader.js';
import { ImageHandler } from './image_handler.js';


// Run mr::MoonRegistrar::draw_layer_image on input image
async function draw_layer_image(
  user_image_handler,
  model_image_handler,
  layer_image_handler,
  algorithm = 256,
  layer_image_transparency = 1.0,
  filter_px = -1
)
{
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let result = await instance._mrwasm_draw_layer_image(
          // using emscripten binding struct will make this function takes an extra argument at the beginning
          // null,
          user_image_handler.image_ptr,
          model_image_handler.image_ptr,
          layer_image_handler.image_ptr,
          algorithm,
          layer_image_transparency,
          filter_px
        );
        
        // // unpack int*
        // const NUM_INT_FIELDS = 22;
        // instance.HEAP64
        // const addr32 = ptr / instance.HEAP32.BYTES_PER_ELEMENT;
        // const data32 = instance.HEAP32.slice(addr32, addr32 + NUM_INT_FIELDS);
        
        const data_list = new Int32Array(instance.HEAP32.buffer, result, 5);
        console.log(data_list);
        
        // const data_list = data32.slice(0, 3);
        // self.x = data_list[0];
        // self.y = data_list[1];
        // self.radius = data_list[2];
        
        
        let ret = new ImageHandler();
        // ret.img_width = result.img_width;
        // ret.img_height = result.img_height;
        // ret.img_data_length = result.img_data_length;
        // ret.buffer_ptr = result.buffer_ptr;
        // ret.image_ptr = result.image_ptr;
        ret.img_width = data_list[0];
        ret.img_height = data_list[1];
        ret.img_data_length = data_list[2];
        ret.buffer_ptr = data_list[3];
        ret.image_ptr = data_list[4];
        resolve(ret);
      } catch (error) {
        reject(error);
      }
    });
  });
}
