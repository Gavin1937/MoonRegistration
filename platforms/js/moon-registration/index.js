export {
  // wasm_loader.js
  instance,
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
