#include "cbmp.h"
#include "init.c"
#include <stdio.h>
// Marks the cell with a red cross in output_image
void markCell(int x, int y)
{

    // Cross sign with size and "Red" color (R, G, B)
    unsigned char color[3] = {255, 0, 0};
    int size = 5;

    for (int dx = -size; dx <= size; dx++)
    {
        // Ensures inside border edge horizontally
        if (x + dx >= 0 && x + dx < BMP_WIDTH)
        {
            // Draws the horizontal line
            input_image[x + dx][y][0] = color[0];
            input_image[x + dx][y][1] = color[1];
            input_image[x + dx][y][2] = color[2];
        }
    }
    for (int dy = -size; dy <= size; dy++)
    {
        // Ensures inside border edge vertically
        if (y + dy >= 0 && y + dy < BMP_HEIGHT)
        {
            // Draws the vertical line
            input_image[x][y + dy][0] = color[0];
            input_image[x][y + dy][1] = color[1];
            input_image[x][y + dy][2] = color[2];
        }
    }
}

// Check the four sides of exclusion border for white pixels
char excludeCell(unsigned char img[BMP_WIDTH][BMP_HEIGHT], int x, int y)
{
    // Check left exclusion border
    for (char dy = -6; dy <= 7; dy++)
    {
        if (x < 6)
        { // avoid segmentation fault
            break;
        }
        if (y + dy < 0 || y + dy > BMP_HEIGHT - 1)
        {
            continue;
        }

        // Return true if found a white pixel
        if (img[x - 6][y + dy])
        {
            return 1;
        }
    }
    // Check right exclusion border
    for (char dy = -6; dy <= 7; dy++)
    {
        if (x > BMP_WIDTH - 8)
        { // avoid segmentation fault
            break;
        }
        if (y + dy < 0 || y + dy > BMP_HEIGHT - 1)
        {
            continue;
        }

        // Return true if found a white pixel
        if (img[x + 7][y + dy])
        {
            return 1;
        }
    }

    // Check top exclusion border
    for (char dx = -5; dx <= 6; dx++)
    {
        if (y < 6)
        { // avoid segmentation fault
            break;
        }
        if (x + dx < 0 || x + dx > BMP_WIDTH - 1)
        {
            continue;
        }

        // Return true if found a white pixel
        if (img[x + dx][y - 6])
        {
            return 1;
        }
    }

    // Check bottom exclusion border
    for (char dx = -5; dx <= 6; dx++)
    {
        if (y + 7 > BMP_HEIGHT - 1)
        { // avoid segmentation fault
            break;
        }
        if (x + dx < 0 || x + dx > BMP_WIDTH - 1)
        {
            continue;
        }

        // Return true if found a white pixel
        if (img[x + dx][y + 7])
        {
            return 1;
        }
    }

    return 0;
}

//Remove cell from image
void removeCell(unsigned char img[BMP_WIDTH][BMP_HEIGHT], int x, int y)
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
            //Turn pixel black
            img[x + dx][y + dy] = 0;
        }
    }
}

//Search in frame for a cell
char cellInFrame(unsigned char img[BMP_WIDTH][BMP_HEIGHT], int x, int y)
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

            //Return true if a white pixel is found
            if (img[x + dx][y + dy])
            {
                return 1;
            }
        }
    }
    return 0;
}


//Search entire image for cells
int detectCells(unsigned char img[BMP_WIDTH][BMP_HEIGHT], unsigned char blackArea[BMP_WIDTH][BMP_HEIGHT])
{
    int cellsFound = 0;
    // Loop through all pixels
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            if (blackArea[x][y])
            {
                continue;
            }
            // Check exclusion border first
            if (excludeCell(img, x, y))
            {
                continue;
            }
            if (cellInFrame(img, x, y))
            {

                // Marks the cells with a red cross
                markCell(x, y);
                removeCell(img, x, y);
                cellsFound++;
                // Print the cell coordinates
                printf("Cell found at: [%i; %i]\n", x, y);
            }
            blackArea[x][y] = 1;
        }
    }
    return cellsFound;
}
