export {
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
};

import { instance } from './wasm_loader.js';
import { get_cpp_exception } from './internal.js';


/**
 * A simple wrapper around mr::Circle
 */
class Circle {
  constructor() {
    this.x = -1;
    this.y = -1;
    this.radius = -1;
  }
  
  /**
   * Load Circle data from a result pointer returned from C++ representing mr::Circle
   * 
   * @param {int} ptr a result pointer returned from C++ representing mr::Circle
   * @returns {Promise<boolean>} true if success
   */
  async load_from_ptr(ptr) {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          const data_list = new Uint32Array(instance.HEAP32.buffer, ptr, 3)
          self.x = data_list[0];
          self.y = data_list[1];
          self.radius = data_list[2];
          
          resolve(true);
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  toString() {
    return `(x=${this.x}, y=${this.y}, radius=${this.radius})`;
  }
};

/**
 * A simple wrapper around mr::Square
 */
class Square {
  constructor() {
    this.x = -1;
    this.y = -1;
    this.width = -1;
  }
  
  /**
   * Load Square data from a result pointer returned from C++ representing mr::Square
   * 
   * @param {int} ptr a result pointer returned from C++ representing mr::Square
   * @returns {Promise<boolean>} true if success
   */
  async load_from_ptr(ptr) {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          const data_list = new Uint32Array(instance.HEAP32.buffer, ptr, 3);
          self.x = data_list[0];
          self.y = data_list[1];
          self.width = data_list[2];
          
          resolve(true);
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  toString() {
    return `(x=${this.x}, y=${this.y}, width=${this.width})`;
  }
};

/**
 * A simple wrapper around mr::Rectangle
 */
class Rectangle {
  constructor() {
    this.top_left_x = -1;
    this.top_left_y = -1;
    this.bottom_right_x = -1;
    this.bottom_right_y = -1;
  }
  
  /**
   * Load Rectangle data from a result pointer returned from C++ representing mr::Rectangle
   * 
   * @param {int} ptr a result pointer returned from C++ representing mr::Rectangle
   * @returns {Promise<boolean>} true if success
   */
  async load_from_ptr(ptr) {
    let self = this;
    return new Promise((resolve, reject) => {
      instance.ready.then(async function() {
        try {
          const data_list = new Uint32Array(instance.HEAP32.buffer, ptr, 4);
          self.top_left_x = data_list[0];
          self.top_left_y = data_list[1];
          self.bottom_right_x = data_list[2];
          self.bottom_right_y = data_list[3];
          
          resolve(true);
        } catch (error) {
          reject(await get_cpp_exception(error));
        }
      });
    });
  }
  
  toString() {
    return `(top_left_x=${this.top_left_x}, top_left_y=${this.top_left_y}, bottom_right_x=${this.bottom_right_x}, bottom_right_y=${this.bottom_right_y})`;
  }
};

/**
 * Convert Circle object to Square object
 * 
 * @param {Circle} circle input circle object
 * @returns {Promise<Square>} a new Square object
 */
async function circle_to_square(circle) {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let result_ptr = await instance._mrwasm_circle_to_square(
          circle.x, circle.y, circle.radius
        );
        
        let ret = new Square();
        await ret.load_from_ptr(result_ptr);
        resolve(ret);
      } catch(error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}

/**
 * Convert Circle object to Rectangle object
 * 
 * @param {Circle} circle input Circle object
 * @returns {Promise<Rectangle>} a new Rectangle object
 */
async function circle_to_rectangle(circle) {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let result_ptr = await instance._mrwasm_circle_to_rectangle(
          circle.x, circle.y, circle.radius
        );
        
        let ret = new Rectangle();
        await ret.load_from_ptr(result_ptr);
        resolve(ret);
      } catch(error) {
        reject(await get_cpp_exception(error));
      }
    });
  });
}
