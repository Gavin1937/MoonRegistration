#ifndef MOONREGISTRATE_C_API_H
#define MOONREGISTRATE_C_API_H

#include "MoonRegistration/macros.h"
#include "MoonRegistration/c_mrapi/utils_c_api.h"


#if defined(__cplusplus)
extern "C" {
#endif

// defining mr::RegistrationAlgorithms
// Note that OpenCV non-free modules does not supporting C.
// At least I cannot link opencv_xfeatures2d with any of the C program.
#define MRC_SIFT                      0x100
#define MRC_ORB                       0x101
#define MRC_AKAZE                     0x102
#define MRC_BRISK                     0x103
#define MRC_EMPTY_ALGORITHM           0x001
#define MRC_INVALID_ALGORITHM         0x000


// Run mr::MoonRegistrar::draw_layer_image() from input image
// 
// Parameters:
//   - user_image: a mat_ptr to user image, you can read image using mrc_read_image_from_... functions
//   - model_image: a mat_ptr to model image, you can read image using mrc_read_image_from_... functions
//   - layer_image: a mat_ptr to layer image, you can read image using mrc_read_image_from_... functions
//   - mrc_algorithm: int value representing mr::RegistrationAlgorithms, can be:
//     MRC_SIFT, MRC_ORB, MRC_AKAZE, MRC_BRISK, MRC_EMPTY_ALGORITHM, MRC_INVALID_ALGORITHM
//   - layer_image_transparency: a 0~1 float percentage changing layer image's transparency
//   - filter_px: 4 1-byte numbers representing BGRA value of a pixel, function will use it
//     to filter the pixel in layer image. A pixel will be ignore when all of its values
//     is <= filter_px. Set it to NULL if you don't need it
//   - error_message: pointer to string buffer for error_message, set it to NULL if you don't need it
// 
// Returns:
//   - if success, return a mat_ptr.
//   - if fail, return NULL and set error_message to a string.
// 
// Note:
//   - this function is designed to work with different number of color channels
//   - when using filter_px, we only read number of elements corresponding to
//     number of channels in layer image. if layer image has 3 channels, we only read
//     first 3 elements of filter_px. rest of elements will be ignored.
EXPORT_SYMBOL mat_ptr mrc_registrar_draw_layer_image(
    mat_ptr user_image,
    mat_ptr model_image,
    mat_ptr layer_image,
    const int mrc_algorithm,
    const float layer_image_transparency,
    const unsigned char* filter_px,
    char** error_message
);

#if defined(__cplusplus)
}
#endif

#endif