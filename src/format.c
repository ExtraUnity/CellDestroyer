#include "cbmp.h"
#include <ctype.h>

void strToUpper(char *str) {
  while (*str) {
    *str = toupper((unsigned char)*str);
    str++;
  }
}

void strToLower(char *str) {
  while (*str) {
    *str = tolower((unsigned char)*str);
    str++;
  }
}


//Expand 1 channel to 3 channels for writing the bitmap
void formatOutputImage(unsigned char input[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
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
