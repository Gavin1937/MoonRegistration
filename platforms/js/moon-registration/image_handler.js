export { ImageHandler };

import { instance } from './wasm_loader.js';
import { get_cpp_exception } from './internal.js';


/**
 * A simple class that handles image loading & reading between JS & C++
 */
class ImageHandler {
  constructor() {
    this.img_width = null;
    this.img_height = null;
    this.data_url = null;
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
          reject(await get_cpp_exception(error));
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
        self.data_url= data_url;
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
            // use colorSpace srgb so we can keep the consistency
            const ctx = canvas.getContext('2d', {colorSpace: 'srgb'});
            // Draw image onto canvas
            // we MUST draw img, otherwise canvas won't contain any data
            ctx.drawImage(img, 0, 0);
            
            const canvas_image = ctx.getImageData(0, 0, self.img_width, self.img_height);
            self.img_data_length = canvas_image.data.length;
            
            self.buffer_ptr = await instance._mrwasm_create_image_buffer(self.img_data_length);
            await instance.HEAP8.set(canvas_image.data, self.buffer_ptr);
            
            self.image_ptr = await instance._mrwasm_create_image_ptr(self.buffer_ptr, self.img_data_length, self.img_width, self.img_height);
          } catch (error) {
            reject(await get_cpp_exception(error));
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
            if (self.data_url) {
              await URL.revokeObjectURL(self.data_url);
            }
            await instance._mrwasm_destroy_image(self.buffer_ptr, self.image_ptr);
          }
          
          self.img_width = null;
          self.img_height = null;
          self.data_url = null;
          self.img_data_length = null;
          self.buffer_ptr = null;
          self.image_ptr = null;
          
          resolve(true);
        } catch(error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  /**
   * Get Uint8Array of pixel data
   * 
   * @returns {Promise<Uint8Array>} Uint8Array
   */
  async get_uint8_array() {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          resolve(new Uint8Array(
            instance.HEAP8.buffer,
            self.buffer_ptr,
            self.img_data_length
          ));
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  /**
   * Get Uint8ClampedArray of pixel data
   * 
   * @returns {Promise<Uint8ClampedArray>} Uint8ClampedArray
   */
  async get_uint8_clamped_array() {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          resolve(new Uint8ClampedArray(
            instance.HEAP8.buffer,
            self.buffer_ptr,
            self.img_data_length
          ));
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  /**
   * Convert ImageHandler to ImageData object for Canvas API
   * 
   * @returns {Promise<ImageData>} ImageData object
   */
  async to_ImageData() {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          let data = await self.get_uint8_clamped_array();
          resolve(new ImageData(
            data,
            self.img_width,
            self.img_height
          ));
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  /**
   * Convert ImageHandler to Blob object
   * 
   * @returns {Promise<Blob>} Blob object
   */
  async to_Blob() {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          let canvas = document.createElement('Canvas');
          canvas.width = self.img_width;
          canvas.height = self.img_height;
          let ctx = canvas.getContext('2d', {
            'colorSpace': 'srgb'
          });
          ctx.putImageData(await self.to_ImageData(), 0, 0);
          canvas.toBlob((blob_data) => {
            resolve(blob_data);
          })
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
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
