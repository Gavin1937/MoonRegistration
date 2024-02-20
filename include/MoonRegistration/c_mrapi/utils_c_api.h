#ifndef UTILS_C_API_H
#define UTILS_C_API_H

#include <stdbool.h>
#include <cstddef>

#include "MoonRegistration/macros.h"



#if defined(__cplusplus)
extern "C" {
#endif

EXPORT_SYMBOL typedef void* mat_ptr;


EXPORT_SYMBOL mat_ptr mrc_read_image_from_filepath(const char* filepath);

EXPORT_SYMBOL mat_ptr mrc_read_image_from_bytes(const unsigned char* bytes, const size_t byte_length);

EXPORT_SYMBOL bool mrc_write_image_to_filepath(mat_ptr image, const char* filepath);

EXPORT_SYMBOL bool mrc_write_image_to_bytes(mat_ptr image, const char* fileext, unsigned char** buffer, size_t* buffer_length);

EXPORT_SYMBOL void mrc_destroy_mat_ptr(mat_ptr ptr);

#if defined(__cplusplus)
}
#endif

#endif