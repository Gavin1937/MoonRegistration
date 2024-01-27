#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// MoonRegistration library api
#include "MoonRegistration/c_mrapi.h"



int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: ./MoonDetector_c_api /path/to/image.jpg\n");
        return 0;
    }
    
    printf("MoonRegistration Library Version: %s\n", mr_version());
    
    const char* filepath = argv[1];
    int* final_circle = NULL;
    char* error_message = NULL;
    
    
    // calculate moon position from filepath
    final_circle = detect_moon_from_filepath(filepath, &error_message /*set this to NULL if you don't need it*/);
    
    
    // // calculate moon position from image bytes
    // FILE* file_in = fopen(filepath, "rb");
    // if (file_in != NULL)
    // {
    //     fseek(file_in, 0L, SEEK_END);
    //     size_t file_size = ftell(file_in);
    //     unsigned char* buffer = malloc(sizeof(unsigned char) * file_size);
    //     fseek(file_in, 0L, SEEK_SET);
    //     fread(buffer, sizeof(unsigned char), file_size, file_in);
    //     fclose(file_in);
    //     final_circle = detect_moon_from_binary(buffer, (const int)file_size, &error_message /*set this to NULL if you don't need it*/);
    //     free(buffer);
    // }
    // else
    // {
    //     printf("Failed to open filepath\n");
    //     return -1;
    // }
    
    
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
    
    return 0;
}
