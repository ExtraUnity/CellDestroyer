#include "cbmp.h"
#include "cellDetection.c"
#include "format.c"
#include <stdio.h>
int erode(unsigned char img[BMP_WIDTH][BMP_HEIGHT], unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT])
{
    int erosionNumber = 0;
    int hasEroded = 1;

    int kernel[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0},
    };

    hasEroded = 0;
    // Copy original
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            erodedImage[i][j] = img[i][j];
        }
    }

    // Loop through all pixels
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            // If pixel is black then continue
            if (img[x][y] == 0)
            {
                continue;
            }

            int erode = 0;

            //Apply kernel for selected pixel
            for (int kx = -1; kx <= 1; kx++)
            {
                if (x + kx < 0 || x + kx > BMP_WIDTH - 1)
                { // Check if on x-edge
                    continue;
                }

                for (int ky = -1; ky <= 1; ky++)
                {
                    if (y + ky < 0 || y + ky > BMP_HEIGHT - 1)
                    { // Check if on y-edge
                        continue;
                    }

                    if (kernel[kx + 1][ky + 1] ==
                        0)
                    { // If 0 in kernel then no need to check
                        continue;
                    }

                    if (img[x + kx][y + ky] ==
                        0) // If value is not 1 in place where it needs to be then erode
                    {
                        erode = 1;
                        hasEroded = 1;
                    }
                }
            }
            // erode image if needed
            if (erode)
            {
                erodedImage[x][y] = 0;
            }
        }
    }

    // Copy eroded image to working image
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            img[i][j] = erodedImage[i][j];
        }
    }

    return hasEroded;
}

int erodeImage(unsigned char img[BMP_WIDTH][BMP_HEIGHT], unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS])
{
    int totalCells = 0;
    int erosionNumber = 0;
    char hasEroded = 1;

    unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT];
    char fileName[256];
    while (hasEroded)
    {
        hasEroded = 0;
        totalCells += detectCells(img);
        erosionNumber++;
        hasEroded = erode(img, erodedImage);
        // Save erosion image to file and detect cells
        if (hasEroded)
        {
            sprintf(fileName, "../out/eroded%d.bmp", erosionNumber);
            formatOutputImage(erodedImage, output_image);
            write_bitmap(output_image, fileName);
        }
    }
    return totalCells;
}