// This is an abstraction of C++ MoonRegistration API for C
// Since C++ version of this library is heavily rely on
// C++ features like namespace & class, plus we relies on
// OpenCV's C++ API. So providing a full API of the library
// for C is time consuming. Thus, we only provide an
// abstracted version of this library.

#ifndef C_MRAPI_H
#define C_MRAPI_H

#include "MoonRegistration/macros.h"
#include "MoonRegistration/c_mrapi/MoonDetect_c_api.h"
#include "MoonRegistration/c_mrapi/MoonRegistrate_c_api.h"
#include "MoonRegistration/c_mrapi/utils_c_api.h"


#if defined(__cplusplus)
extern "C" {
#endif

EXPORT_SYMBOL extern inline const char* mrc_version();

#if defined(__cplusplus)
}
#endif

#endif