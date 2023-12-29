#ifndef C_MRAPI_H
#define C_MRAPI_H

#include "macros.h"

// An abstraction of C++ MoonRegistration API for C
// Since C++ version of this library is heavily rely on
// C++ features like namespace & class, plus we relies on
// OpenCV's C++ API. So providing a full API of the library
// for C is time consuming. Thus, we only provide an
// abstracted version of this library.

#if defined(__cplusplus)
extern "C" {
#endif


EXPORT_SYMBOL extern inline const char* mr_version()
{
    return "v0.1.6";
}

// Run mr::MoonDetector::detect_moon() from image filepath
// 
// Parameters:
//   - img_filepath: string image filepath
//   - error_message: pointer to string buffer for error_message, set it to NULL if you don't need it
// 
// Returns:
//   - if success, return int* to 3 int in the heap: [x, y, radius]. You need to manually free it.
//   - if fail, return NULL and set error_message to a string.
EXPORT_SYMBOL int* detect_moon_from_filepath(const char* img_filepath, char** error_message);

// Run mr::MoonDetector::detect_moon() from image binary
// 
// Parameters:
//   - img_binary: image raw bytes
//   - img_size: int size of img_binary buffer
//   - error_message: pointer to string buffer for error_message, set it to NULL if you don't need it
// 
// Returns:
//   - if success, return int* to 3 int in the heap: [x, y, radius]. You need to manually free it.
//   - if fail, return NULL and set error_message to a string.
EXPORT_SYMBOL int* detect_moon_from_binary(const unsigned char* img_binary, const int img_size, char** error_message);


#if defined(__cplusplus)
}
#endif

#endif