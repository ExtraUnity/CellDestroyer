#include "cbmp.h"
// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];