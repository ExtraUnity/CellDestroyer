#include "cbmp.h"

void formatOutputImage(unsigned char input[BMP_WIDTH][BMP_HEIGHT])
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