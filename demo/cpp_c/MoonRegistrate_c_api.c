#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// MoonRegistration library c api
#include "MoonRegistration/c_mrapi.h"



int main(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Usage: ./MoonRegistrar_c_api [USER_IMAGE_PATH] [MODEL_IMAGE_PATH] [LAYER_IMAGE_PATH]\n");
        return 0;
    }
    
    printf("MoonRegistration Library Version: %s\n", mrc_version());
    
    mat_ptr user_image = mrc_read_image_from_filepath(argv[1]);
    mat_ptr model_image = mrc_read_image_from_filepath(argv[2]);
    mat_ptr layer_image = mrc_read_image_from_filepath(argv[3]);
    
    
    char* error_message = NULL;
    // Here we use an int to represent a 4-bytes unsigned char* array.
    // This is a cv::Vec4b pixel value with color channel ordered in BGRA.
    // And following integer is representing pixel value (0,0,0,255) in little-endian.
    int filter_px = 0xff000000;
    mat_ptr output_image = mrc_registrar_draw_layer_image(
        user_image, model_image, layer_image,
        MRC_ORB, 1.0f,
        (unsigned char*)&filter_px,
        &error_message
    );
    
    
    // write output image to file
    if (output_image != NULL)
    {
        if (mrc_write_image_to_filepath(output_image, "./output/registration.png"))
            printf("Write output image to ./output/registration.png\n");
        else
            printf("Failed to write output image\n");
    }
    else
    {
        printf("Failed to draw layer image\nError: %s\n", error_message);
        return -1;
    }
    
    mrc_destroy_mat_ptr(output_image);
    
    return 0;
}
