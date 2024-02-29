export {MoonRegistration};

import Module from './MoonRegistration.js';


// Reference:
// https://stackoverflow.com/a/59537780

let instance = {
  ready: new Promise(resolve => {
    Module({
      onRuntimeInitialized() {
        instance = Object.assign(this, {
          ready: Promise.resolve()
        });
        resolve();
      }
    });
  })
};

let MoonRegistration = {
  MoonDetection: async (fileObject, retType, callback) => {
    instance.ready.then(_ => {
      let img = new Image();
      img.src = window.URL.createObjectURL(fileObject);

      img.onload = function () {
        const canvas = document.createElement('canvas');
        // Make canvas same size as image
        canvas.width = img.width;
        canvas.height = img.height;
        // Draw image onto canvas
        // we MUST draw img, otherwise canvas won't contain any data
        const ctx = canvas.getContext('2d');
        ctx.drawImage(img, 0, 0);

        const image = ctx.getImageData(0, 0, img.width, img.height);
        
        const p = instance._mrwasm_create_image_buffer(image.data.length);
        instance.HEAP8.set(image.data, p);

        // run moon_detection
        let detection_result_ptr = instance._mrwasm_detect_moon(p, image.data.length, image.width, image.height);

        // unpack result int*
        const NUM_INT_FIELDS = 22;
        const addr32 = detection_result_ptr / instance.HEAP32.BYTES_PER_ELEMENT;
        const data32 = instance.HEAP32.slice(addr32, addr32 + NUM_INT_FIELDS);
        const circleDataList = data32.slice(0, 3);

        // destroy/free buffer
        instance._mrwasm_destroy_image_buffer(p);


        let ret = null;
        let ret_circle = { "type": "circle", "x": circleDataList[0], "y": circleDataList[1], "radius": circleDataList[2] };

        // modify result base on retType
        if (retType === 'square') {
          let result_ptr = instance._mrwasm_circle_to_square(circleDataList[0], circleDataList[1], circleDataList[2]);
          // unpack result int*
          const NUM_INT_FIELDS = 22;
          const addr32 = result_ptr / instance.HEAP32.BYTES_PER_ELEMENT;
          const data32 = instance.HEAP32.slice(addr32, addr32 + NUM_INT_FIELDS);
          const squareDataList = data32.slice(0, 3);
          ret = {
            type: 'square',
            x: squareDataList[0],
            y: squareDataList[1],
            width: squareDataList[2]
          };
        }
        else if (retType === 'rectangle') {
          let result_ptr = instance._mrwasm_circle_to_rectangle(circleDataList[0], circleDataList[1], circleDataList[2]);
          // unpack result int*
          const NUM_INT_FIELDS = 22;
          const addr32 = result_ptr / instance.HEAP32.BYTES_PER_ELEMENT;
          const data32 = instance.HEAP32.slice(addr32, addr32 + NUM_INT_FIELDS);
          const rectangleDataList = data32.slice(0, 4);
          ret = {
            type: 'rectangle',
            x1: rectangleDataList[0],
            y1: rectangleDataList[1],
            x2: rectangleDataList[2],
            y3: rectangleDataList[3],
          };
        }

        // set ret value via callback function
        callback(ret_circle, ret);
      };
    }).then(result => result);
  }
};
