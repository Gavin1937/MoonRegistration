#ifndef MOONDETECT_C_API_H
#define MOONDETECT_C_API_H

#include "MoonRegistration/macros.h"
#include "MoonRegistration/c_mrapi/utils_c_api.h"


#if defined(__cplusplus)
extern "C" {
#endif

// Run mr::MoonDetector::detect_moon() from input image
// 
// Parameters:
//   - image: a mat_ptr to the image, you can read image using mrc_read_image_from_... functions
//   - error_message: pointer to string buffer for error_message, set it to NULL if you don't need it
// 
// Returns:
//   - if success, return int* to 3 int in the heap: [x, y, radius]. You need to manually free it.
//   - if fail, return NULL and set error_message to a string.
EXPORT_SYMBOL int* mrc_detect_moon(mat_ptr image, char** error_message);

#if defined(__cplusplus)
}
#endif

#endif