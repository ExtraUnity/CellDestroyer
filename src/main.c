#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];
unsigned char output_image[BMP_WIDTH][BMP_HEIGTH][BMP_CHANNELS];

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        exit(1);
    }

    // Load image from file
    read_bitmap(argv[1], input_image);

    /*
    Find all cells
    */


    // Save image to file
    write_bitmap(output_image, argv[2]);
    printf("Number of cells has not been counted yet :(");
    return 0;
}