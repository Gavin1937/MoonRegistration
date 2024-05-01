export { 
  MR_VERSION_STR,
  MR_HAVE_HOUGH_GRADIENT_ALT,
  MR_ENABLE_OPENCV_NONFREE,
  MR_HAVE_OPENCV_NONFREE,
};

import { instance } from './wasm_loader.js';
import { get_c_str } from './internal.js';

async function __MR_VERSION_STR() {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let ptr = await instance.___MR_VERSION_STR__();
        let val = await get_c_str(ptr, 10);
        resolve(val);
      } catch (error) {
        reject(error);
      }
    })
  });
};
async function __MR_HAVE_HOUGH_GRADIENT_ALT() {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let val = await instance.___MR_HAVE_HOUGH_GRADIENT_ALT__();
        resolve(Boolean(val));
      } catch (error) {
        reject(error);
      }
    })
  });
};
async function __MR_ENABLE_OPENCV_NONFREE() {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let val = await instance.___MR_ENABLE_OPENCV_NONFREE__();
        resolve(Boolean(val));
      } catch (error) {
        reject(error);
      }
    })
  });
};
async function __MR_HAVE_OPENCV_NONFREE() {
  return new Promise((resolve, reject) => {
    instance.ready.then(async function() {
      try {
        let val = await instance.___MR_HAVE_OPENCV_NONFREE__();
        resolve(Boolean(val));
      } catch (error) {
        reject(error);
      }
    })
  });
};

const MR_VERSION_STR = await __MR_VERSION_STR();
const MR_HAVE_HOUGH_GRADIENT_ALT = await __MR_HAVE_HOUGH_GRADIENT_ALT();
const MR_ENABLE_OPENCV_NONFREE = await __MR_ENABLE_OPENCV_NONFREE();
const MR_HAVE_OPENCV_NONFREE = await __MR_HAVE_OPENCV_NONFREE();