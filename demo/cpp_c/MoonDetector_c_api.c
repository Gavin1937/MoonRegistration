#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>


// MoonRegistration library c api
#include "MoonRegistration/c_mrapi.h"



int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Usage: ./MoonDetector_c_api [IMAGE_PATH]\n");
        return 0;
    }
    
    printf("MoonRegistration Library Version: %s\n", mrc_version());
    
    const char* filepath = argv[1];
    int* final_circle = NULL;
    char* error_message = NULL;
    mat_ptr image = NULL;
    
    // read image from filepath
    image = mrc_read_image_from_filepath(filepath);
    
    
    // // read image from bytes
    // FILE* file_in = fopen(filepath, "rb");
    // if (file_in != NULL)
    // {
    //     fseek(file_in, 0L, SEEK_END);
    //     size_t file_size = ftell(file_in);
    //     unsigned char* buffer = malloc(sizeof(unsigned char) * file_size);
    //     fseek(file_in, 0L, SEEK_SET);
    //     fread(buffer, sizeof(unsigned char), file_size, file_in);
    //     fclose(file_in);
    //     image = mrc_read_image_from_bytes(buffer, file_size);
    //     free(buffer);
    // }
    // else
    // {
    //     printf("Failed to open filepath\n");
    //     return -1;
    // }
    
    // calculate moon position
    final_circle = mrc_detect_moon(image, &error_message /*set this to NULL if you don't need it*/);
    
    // printing out result
    if (final_circle != NULL)
    {
        printf("\n\n\n");
        printf("file: \'%s\'\n", filepath);
        printf( "Circle: (x=%d, y=%d, radius=%d)\n", final_circle[0], final_circle[1], final_circle[2]);
    }
    else
    {
        printf("Failed to detect moon\nError: %s\n", error_message);
        return -1;
    }
    
    free(final_circle);
    mrc_destroy_mat_ptr(image);
    
    return 0;
}
