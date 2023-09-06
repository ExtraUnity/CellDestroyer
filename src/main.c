#include <stdlib.h>
#include <stdio.h>
#include "cbmp.h"

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];

unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];

void formatOutputImage(char input[BMP_WIDTH][BMP_HEIGHT])
{
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            for (int k = 0; k < BMP_CHANNELS; k++)
            {
                output_image[i][j][k] = input[i][j];
            }
        }
    }
}

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n", argv[0]);
        exit(1);
    }

    // Load image from file
    read_bitmap(argv[1], input_image);

    unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];

    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            greyscale_image[i][j] = (input_image[i][j][0] + input_image[i][j][1] + input_image[i][j][2]) / 3;
        }
    }
    /*
    Find all cells
    */

    formatOutputImage(greyscale_image);

    // Save image to file
    write_bitmap(output_image, argv[2]);
    printf("Number of cells has not been counted yet :(");
    return 0;
}