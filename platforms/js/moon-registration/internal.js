export { MAX_C_STRING_LENGTH, get_cpp_exception, get_c_str };

import { instance } from './wasm_loader.js';


const MAX_C_STRING_LENGTH = 512;

/**
 * Turn a C string char* into JS String
 * 
 * @param {int} str_ptr C string pointer as integer 
 * @param {int} length length of bytes to read
 * @returns {Promise<String>} JS String
 */
async function get_c_str(str_ptr, length = MAX_C_STRING_LENGTH) {
  return new Promise((resolve, _) => {
    instance.ready.then(async function() {
      let data = new Uint8Array(instance.HEAP8.buffer, str_ptr, length);
      let terminator = data.indexOf(0);
      let decoder = new TextDecoder();
      resolve(decoder.decode(data.subarray(0, terminator)));
    });
  });
}

/**
 * Turn numeric exception from C++ to JS Error.
 * Sometimes, C++ will throw its std::exception in the form of a pointer to js,
 * so you will see it as a number print out on the console. This function will
 * try to retrieve the exception message from pointer.
 * See {@link https://github.com/emscripten-core/emscripten/issues/6330}
 * 
 * @param {Object} exception A JS Error object or a numeric string throw from C++
 * @returns {Promise<Error>} JS Error
 */
async function get_cpp_exception(exception) {
  return new Promise((resolve, _) => {
    // exception is numeric, assume its a pointer to std::exception
    if (!isNaN(exception)) {
      instance.ready.then(async function() {
        let msg_ptr = await instance._mrwasm_get_last_exception_msg(Number(exception));
        let output = await get_c_str(msg_ptr);
        output = 'std::exception: ' + output;
        
        resolve(new Error(output));
      });
    } else {
      resolve(exception);
    }
  });
}