export {instance};

import Module from './lib/MoonRegistration.js';


// Loading EMScripten WASM module
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
