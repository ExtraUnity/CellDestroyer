#include "cbmp.h"
#include <math.h>

void greyTransform(unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS], unsigned char grey_image[BMP_WIDTH][BMP_HEIGHT])
{
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            grey_image[i][j] =
                (input_image[i][j][0] + input_image[i][j][1] + input_image[i][j][2]) /
                3;
        }
    }
}

// Computes the distance transform of the binary image using parallel computing
// Based on Borgefors, G. (1986). Distance transformations in digital images. Computer Vision, Graphics, and Image Processing
void distanceTransform(unsigned char dist[BMP_WIDTH][BMP_HEIGHT])
{

    char mask[DIST_MASK_SIZE][DIST_MASK_SIZE] = {
        {-1, 11, -1, 11, -1},
        {11, 7, 5, 7, 11},
        {-1, 5, 0, 5, -1},
        {11, 7, 5, 7, 11},
        {-1, 11, -1, 11, -1},
    };

    // unsigned char mask[3][3] = {
    //     {4, 3, 4},
    //     {3, 0, 3},
    //     {4, 3, 4}
    // };
    char changed = 1;
    while (changed)
    {
        changed = 0;

        unsigned char newDist[BMP_WIDTH][BMP_HEIGHT];

        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                unsigned char min = 255;
                unsigned char oldVal = dist[i][j];
                for (char ky = -2; ky <= 2; ky++)
                {

                    if (j + ky < 0 || j + ky > BMP_HEIGHT - 1)
                    { // Check if on y-edge
                        continue;
                    }

                    for (char kx = -2; kx <= 2; kx++)
                    {
                        if (mask[kx + 2][ky + 2] == -1)
                        {
                            continue;
                        }
                        if (i + kx < 0 || i + kx > BMP_WIDTH - 1)
                        { // Check if on x-edge
                            continue;
                        }

                        int val = dist[i + kx][j + ky] + mask[kx + 2][ky + 2];
                        if (val < min)
                        {
                            min = val;
                        }
                    }
                }
                newDist[i][j] = min;
                if (min != dist[i][j])
                {
                    changed = 1;
                }
            }
        }
        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                dist[i][j] = newDist[i][j];
            }
        }
    }
}

// Blurs the greyscale_image using a gaussian filter
void gaussianBlur(unsigned char img[BMP_WIDTH][BMP_HEIGHT])
{
    unsigned char blurred[BMP_WIDTH][BMP_HEIGHT];

    int kernel[3][3] = {
        // the kernel to be used
        {-1, -2, -1},
        {-2, 24, -2},
        {-1, -2, -1},
    };

    // Copy original image to working image
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            blurred[i][j] = img[i][j];
        }
    }

    // Loop through all pixels
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            int sum = 0;     // The weighted sum of the pixels and surrounding area
            int weight = 12; // the total weight of the kernel

            // Apply kernel for selected pixel
            for (int kx = -1; kx <= 1; kx++)
            {

                for (int ky = -1; ky <= 1; ky++)
                {

                    if (x + kx < 0 || x + kx > BMP_WIDTH - 1)
                    {                                     // Check if on x-edge
                        weight -= kernel[kx + 1][ky + 1]; // the weight then shouldnt be counted
                        continue;
                    }

                    if (y + ky < 0 || y + ky > BMP_HEIGHT - 1)
                    { // Check if on y-edge
                        weight -= kernel[kx + 1][ky + 1];
                        continue;
                    }

                    // compute the weighted sum
                    sum += img[x + kx][y + ky] * kernel[kx + 1][ky + 1];
                }
            }

            // compute the weighted average
            int average = sum / weight;
            // printf("before %i    after %i\n",greyscale_image[x][y],average);

            // set the brightness of the pixel
            blurred[x][y] = average < 0 ? 0 : average;
        }
    }

    // Update original image
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            img[i][j] = blurred[i][j];
        }
    }
}

// Better threshold value based on Otsu Method
unsigned char otsu_threshold(unsigned char img[BMP_WIDTH][BMP_HEIGHT])
{

    // Initialize histogram of size 256 and store each pixels greyscale intensity value (0-255):
    int histogram[256] = {0};
    for (int x = 0; x < BMP_WIDTH; x++)
    {
        for (int y = 0; y < BMP_HEIGHT; y++)
        {
            histogram[img[x][y]]++;
        }
    }

    // Calculates the sum of (greyscale intensity * num of pixel with this intensity).
    // Makes it easier to calculate muB and muF later.
    int sum = 0;
    for (int p = 0; p < 256; p++)
    {
        sum += p * histogram[p];
    }

    // The variables used in Otsu Method/Formula
    int sumB = 0;
    int wB = 0;               // Num of pixels in Background (black) out of total pixels
    int wF = 0;               // Num of pixels in Foreground (white) out of total pixels
    int muB, muF;             // Average of wB and wF respectively
    double maxVariance = 0.0; // "between class" maxVariance
    unsigned char threshold = 0;

    // Loops through histogram and performs Otsu Method:
    int totalPixels = BMP_WIDTH * BMP_HEIGHT;
    for (unsigned char i = 0; i < 256; i++)
    {
        wB = wB + histogram[i];
        if (wB == 0)
            continue;

        wF = totalPixels - wB;
        if (wF == 0)
            break;

        sumB += i * histogram[i];
        muB = sumB / wB;
        muF = (sum - sumB) / wF;

        // Otsu Formula [maxVar² = wB*wF*(muB-muF)²] calculate the "in-between" Variance:
        double varianceBetween = (double)wB * wF * ((muB - muF)*(muB - muF));
        if (varianceBetween > maxVariance)
        {
            maxVariance = varianceBetween;
            threshold = i;
        }
    }

    // Change the global variable of threshold value
    // printf("\nBinary Threshold Value: %d\n", threshold);
    return threshold;
}

// Binary Threshold based on global value "threshold_value"
void binaryThreshold(unsigned char img[BMP_WIDTH][BMP_HEIGHT], unsigned char threshold_value)
{
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            img[i][j] = img[i][j] >= threshold_value ? 255 : 0;
        }
    }
}