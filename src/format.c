#include "cbmp.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

//Convert string to lowercase
char* strlwr(unsigned char str[]) {
    for(int i = 0; i < strlen(str); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

//Convert string to uppercase
char* strupr(unsigned char str[]) {
    for(int i = 0; i< strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
    return str;
}