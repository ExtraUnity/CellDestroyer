#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];

// Check the four borders for white pixels
int excludeCell(int x, int y)
{
    // Check left exclusion border
    for (int dy = -6; dy <= 7; dy++)
    {
        if (x - 6 < 0)
        {
            break;
        }
        if (y + dy < 0 || y + dy > BMP_HEIGHT - 1)
        {
            continue;
        }

        if (greyscale_image[x - 6][y + dy] > 0)
        {
            return 1;
        }
    }

    // Check right exclusion border
    for (int dy = -6; dy <= 7; dy++)
    {
        if (x + 7 > BMP_WIDTH - 1)
        {
            break;
        }
        if (y + dy < 0 || y + dy > BMP_HEIGHT - 1)
        {
            continue;
        }

        if (greyscale_image[x + 7][y + dy] > 0)
        {
            return 1;
        }
    }

    // Check top exclusion border
    for (int dx = -6; dx <= 7; dx++)
    {
        if (y - 6 < 0)
        {
            break;
        }
        if (x + dx < 0 || x + dx > BMP_WIDTH - 1)
        {
            continue;
        }

        if (greyscale_image[x + dx][y - 6] > 0)
        {
            return 1;
        }
    }

    // Check bottom exclusion border
    for (int dx = -6; dx <= 7; dx++)
    {
        if (y + 7 > BMP_HEIGHT - 1)
        {
            break;
        }
        if (x + dx < 0 || x + dx > BMP_WIDTH - 1)
        {
            continue;
        }

        if (greyscale_image[x + dx][y + 7] > 0)
        {
            return 1;
        }
    }

    return 0;
}

int cellInFrame(int x, int y)
{
    for (int dx = -5; dx <= 6; dx++)
    {
        if (x + dx < 0 || x + dx > BMP_WIDTH - 1)
        { // Check if on x-edge
            continue;
        }
        for (int dy = -5; dy <= 6; dy++)
        {
            if (y + dy < 0 || y + dy > BMP_HEIGHT - 1)
            { // Check if on y-edge
                continue;
            }

            if (greyscale_image[x + dx][y + dy] > 0)
            {
                /*
                *
                * POSSIBLY REMOVE CELL FROM IMAGE HERE
                * 
                */
                return 1;
            }
        }
    }
    return 0;
}

void detectCells()
{
    // Loop through all pixels
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            // Check exclusion border first
            if (excludeCell)
            {
                continue;
            }
            if(cellInFrame(x,y)) {
                /*
                *
                *
                * 
                */
            }
        }
    }
}

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

void erodeImage()
{
    int erosionNumber = 0;
    int hasEroded = 1;

    int kernel[3][3] = {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0},
    };

    unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT];
    char fileName[256];
    while (hasEroded)
    {
        erosionNumber++;
        hasEroded = 0;
        // Copy original
        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                erodedImage[i][j] = greyscale_image[i][j];
            }
        }

        // Loop through all pixels
        for (int x = 0; x < BMP_WIDTH; x++)
        {
            for (int y = 0; y < BMP_HEIGHT; y++)
            {
                // If pixel is black then continue
                if (greyscale_image[x][y] == 0)
                {
                    continue;
                }

                int erode = 0;

                // Apply kernel for selected pixel
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

                        if (kernel[kx + 1][ky + 1] == 0)
                        { // If 0 in kernel then no need to check
                            continue;
                        }

                        if (greyscale_image[x + kx][y + ky] == 0) // If value is not 1 in place where it needs to be then erode
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
                greyscale_image[i][j] = erodedImage[i][j];
            }
        }

        // Save erosion image to file and detect cells
        if (hasEroded)
        {
            sprintf(fileName, "../out/eroded%d.bmp", erosionNumber);
            formatOutputImage(erodedImage);
            write_bitmap(output_image, fileName);
            detectCells();
        }
    }
}

void binaryThreshold()
{
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            if (greyscale_image[i][j] >= 90)
            {
                greyscale_image[i][j] = 255;
            }
            if (greyscale_image[i][j] < 90)
            {
                greyscale_image[i][j] = 0;
            }
        }
    }
}

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <output file path> <output file path>\n",
                argv[0]);
        exit(1);
    }

    // Load image from file
    read_bitmap(argv[1], input_image);

    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            greyscale_image[i][j] =
                (input_image[i][j][0] + input_image[i][j][1] + input_image[i][j][2]) /
                3;
        }
    }

    /*
    Find all cells
    */
    binaryThreshold();
    printf("Done with thresholding");
    erodeImage();
    printf("Done with eroding");
    formatOutputImage(greyscale_image);

    // Save image to file
    write_bitmap(output_image, argv[2]);
    printf("Number of cells has not been counted yet :(");
    return 0;
}