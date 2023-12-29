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

EXPORT_SYMBOL inline const char* mr_version()
{
    return "v0.1.5";
}

EXPORT_SYMBOL int* detect_moon_from_filepath(const char* img_filepath);

EXPORT_SYMBOL int* detect_moon_from_binary(const unsigned char* img_binary, const int img_size);

#if defined(__cplusplus)
}
#endif

#endif