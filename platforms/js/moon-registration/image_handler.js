export { ImageHandler };

import { instance } from './wasm_loader.js';


/**
 * A simple class that handles image loading & reading between JS & C++
 */
class ImageHandler {
  constructor() {
    this.img_width = null;
    this.img_height = null;
    this.img_data_length = null;
    this.buffer_ptr = null;
    this.image_ptr = null;
  }
  
  /**
   * Load image from C++ ImageHandlerData struct returned from C++ function
   * 
   * @param {int} ptr C++ pointer to an ImageHandlerData struct
   * @returns {Promise<boolean>} true if success
   */
  async load_from_ImageHandlerData(ptr) {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          const data_list = new Int32Array(instance.HEAP32.buffer, ptr, 5);
          
          self.img_width = data_list[0];
          self.img_height = data_list[1];
          self.img_data_length = data_list[2];
          self.buffer_ptr = data_list[3];
          self.image_ptr = data_list[4];
          
          await instance._mrwasm_destroy_ImageHandlerData(ptr);
          
          resolve(true);
        } catch (error) {
          reject(error);
        }
      });
    });
  }
  
  /**
   * Load image from JS to C++ via Data URL
   * 
   * {@link https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/Data_URLs}
   * 
   * @param {String} data_url 
   * @returns {Promise<boolean>} true if success
   */
  async load_from_dataurl(data_url) {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(_ => {
        let img = new Image();
        img.src = data_url;
        
        img.onload = async function () {
          const canvas = document.createElement('canvas');
          self.img_width = img.width;
          self.img_height = img.height;
          // Make canvas same size as image
          canvas.width = self.img_width;
          canvas.height = self.img_height;
          
          try {
            // Draw image onto canvas
            // we MUST draw img, otherwise canvas won't contain any data
            const ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0);
            
            const canvas_image = ctx.getImageData(0, 0, self.img_width, self.img_height);
            self.img_data_length = canvas_image.data.length;
            
            self.buffer_ptr = await instance._mrwasm_create_image_buffer(self.img_data_length);
            await instance.HEAP8.set(canvas_image.data, self.buffer_ptr);
            
            self.image_ptr = await instance._mrwasm_create_image_ptr(self.buffer_ptr, self.img_data_length, self.img_width, self.img_height);
          } catch (error) {
            reject(error);
          }
          
          resolve(true);
        }
      });
    });
  }
  
  /**
   * Load image from JS to C++ via File object
   * 
   * {@link https://developer.mozilla.org/en-US/docs/Web/API/File}
   * 
   * @param {File} file_obj
   * @returns {Promise<boolean>} true if success
   */
  async load_from_fileobject(file_obj) {
    let url = await URL.createObjectURL(file_obj);
    return this.load_from_dataurl(url);
  }
  
  /**
   * Load image from JS to C++ via Blob object
   * 
   * {@link https://developer.mozilla.org/en-US/docs/Web/API/Blob}
   * 
   * @param {Blob} blob_obj
   * @returns {Promise<boolean>} true if success
   */
  async load_from_blobobj(blob_obj) {
    let url = await URL.createObjectURL(blob_obj);
    return this.load_from_dataurl(url);
  }
  
  /**
   * Load image from JS to C++ via Uint8Array
   * 
   * @param {Uint8Array} uint8_array
   * @param {String} mime_type mime type of input image
   * @returns {Promise<boolean>} true if success
   */
  async load_from_uint8array(uint8_array, mime_type) {
    let blob = new Blob([uint8_array], { 'type': mime_type });
    let url = await URL.createObjectURL(blob);
    return this.load_from_dataurl(url);
  }
  
  /**
   * Destroy image loaded by load_from_*** functions
   * 
   * You must call this function when you are done with an ImageHandler object,
   * otherwise it might lead to memory leak.
   * 
   * @returns {Promise<boolean>} true if success
   */
  async destroy_image() {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          if (self.is_valid_image()) {
            await instance._mrwasm_destroy_image(self.buffer_ptr, self.image_ptr);
          }
          
          self.img_width = null;
          self.img_height = null;
          self.img_data_length = null;
          self.buffer_ptr = null;
          self.image_ptr = null;
          
          resolve(true);
        } catch(error) {
          reject(error);
        }
      });
    });
  }
  
  /**
   * Read image pixel data from C++ to JS
   * 
   * @returns {Uint8Array} Uint8Array
   */
  get_uint8_array() {
    return new Uint8Array(
      instance.HEAP8.buffer,
      this.buffer_ptr,
      this.img_data_length
    );
  }
  
  /**
   * Read image pixel data from C++ to JS
   * 
   * @returns {Uint8ClampedArray} Uint8ClampedArray
   */
  get_uint8_clamped_array() {
    return new Uint8ClampedArray(
      instance.HEAP8.buffer,
      this.buffer_ptr,
      this.img_data_length
    );
  }
  
  /**
   * Check whether ImageHandler is valid or not
   * 
   * @returns {boolean} true if valid, otherwise false
   */
  is_valid_image() {
    return (
      this.img_width &&
      this.img_height &&
      this.img_data_length &&
      this.buffer_ptr &&
      this.image_ptr
    );
  }
  
}
