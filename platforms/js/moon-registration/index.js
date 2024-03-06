export {
  // wasm_loader.js
  instance,
  // image_handler.js
  ImageHandler,
  // shapes.js
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
  // MoonDetect.js
  detect_moon,
  // MoonRegistrate.js
  draw_layer_image,
};

import { instance } from './wasm_loader.js';
import { ImageHandler } from './image_handler.js';
import {
  Circle, Square, Rectangle,
  circle_to_square,
  circle_to_rectangle,
} from './shapes.js';
import { detect_moon } from './MoonDetect.js';
import { draw_layer_image } from './MoonRegistrate.js';
