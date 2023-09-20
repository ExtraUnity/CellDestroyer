#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DIST_MASK_SIZE 5
// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
int totalCells = 0;

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

int max(unsigned char arr[BMP_WIDTH][BMP_HEIGHT])
{
    int max = 0;
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            if (arr[i][j] > max)
            {
                max = arr[i][j];
            }
        }
    }
    return max;
}

// Computes the distance transform of the binary image using parallel computing
//Based on Borgefors, G. (1986). Distance transformations in digital images. Computer Vision, Graphics, and Image Processing
void distanceTransform(unsigned char dist[BMP_WIDTH][BMP_HEIGHT], unsigned char binary[BMP_WIDTH][BMP_HEIGHT])
{

    // initialize dist array
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            dist[i][j] = binary[i][j] == 0 ? 0 : 255;
        }
    }

    unsigned char mask[DIST_MASK_SIZE][DIST_MASK_SIZE] = {
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
    int changed = 1;
    while (changed)
    {
        changed = 0;

        unsigned char newDist[BMP_WIDTH][BMP_HEIGHT];

        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                int min = 10000;
                int oldVal = dist[i][j];
                for (int ky = -2; ky <= 2; ky++)
                {

                    if (j + ky < 0 || j + ky > BMP_HEIGHT - 1)
                    { // Check if on y-edge
                        continue;
                    }

                    for (int kx = -2; kx <= 2; kx++)
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
int otsu_threshold(unsigned char img[BMP_WIDTH][BMP_HEIGHT])
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
    int threshold = 0;

    // Loops through histogram and performs Otsu Method:
    int totalPixels = BMP_WIDTH * BMP_HEIGHT;
    for (int i = 0; i < 256; i++)
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

// Marks the cell with a red cross in output_image
void markCell(int x, int y)
{

    // Cross sign with size and "Red" color (R, G, B)
    unsigned char color[3] = {255, 0, 0};
    int size = 5;

    for (int dx = -size; dx <= size; dx++)
    {
        // Ensures inside border edge horizontally (Redundant?)
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
        // Ensures inside border edge vertically (Redundant?)
        if (y + dy >= 0 && y + dy < BMP_HEIGHT)
        {
            // Draws the vertical line
            input_image[x][y + dy][0] = color[0];
            input_image[x][y + dy][1] = color[1];
            input_image[x][y + dy][2] = color[2];
        }
    }
}

void findAllMaximum(unsigned char img[BMP_WIDTH][BMP_HEIGHT])
{
    int thresh = otsu_threshold(img);
    for (int i = 1; i < BMP_WIDTH - 1; i++)
    {
        for (int j = 1; j < BMP_HEIGHT - 1; j++)
        {

            char val = img[i][j];
            if (val == 0)
            {
                continue;
            }
            char max = 1;
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (val < img[i + dx][j + dy] || val < thresh)
                    {
                        max = 0;
                    }
                }
            }

            if (max)
            {
                markCell(i, j);
            }
        }
    }
}

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

        if (greyscale_image[x - 6][y + dy] == 255)
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

        if (greyscale_image[x + 7][y + dy] == 255)
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

        if (greyscale_image[x + dx][y - 6] == 255)
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

        if (greyscale_image[x + dx][y + 7] == 255)
        {
            return 1;
        }
    }

    return 0;
}

void removeCell(int x, int y)
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
            greyscale_image[x + dx][y + dy] = 0;
        }
    }
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
            if (excludeCell(x, y))
            {
                continue;
            }
            if (cellInFrame(x, y))
            {

                // Marks the cells with a red cross
                markCell(x, y);
                //printf("x: %i, y: %i \n", x, y);
                removeCell(x, y);
                totalCells++;
            }
        }
    }
}

int erode(unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT])
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

                    if (kernel[kx + 1][ky + 1] ==
                        0)
                    { // If 0 in kernel then no need to check
                        continue;
                    }

                    if (greyscale_image[x + kx][y + ky] ==
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
            greyscale_image[i][j] = erodedImage[i][j];
        }
    }

    return hasEroded;
}

void erodeImage()
{
    int erosionNumber = 0;
    int hasEroded;
    int hasDilated;

    unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT];
    char fileName[256];
    while (hasEroded)
    {
        detectCells();
        erosionNumber++;
        hasEroded = erode(erodedImage);
        // Save erosion image to file and detect cells
        if (hasEroded)
        {
            sprintf(fileName, "../out/eroded%d.bmp", erosionNumber);
            formatOutputImage(erodedImage);
            write_bitmap(output_image, fileName);
        }
    }
}
// Binary Threshold based on global value "threshold_value"
void binaryThreshold(unsigned char img[BMP_WIDTH][BMP_HEIGHT], int threshold_value)
{
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            if (img[i][j] >= threshold_value)
            {
                img[i][j] = 255; // white
            }
            if (img[i][j] < threshold_value)
            {
                img[i][j] = 0; // black
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

    unsigned char dist[BMP_WIDTH][BMP_HEIGHT];
    // distanceTransform(dist);
    unsigned char distString[921500] = "";
    for (int i = 0; i < BMP_WIDTH; i++)
    {
        for (int j = 0; j < BMP_HEIGHT; j++)
        {
            printf("%i ", dist[i][j]);
        }
        printf("\n");
    }

    // Load image from file

    for (int i = 1; i <= 10; i++)
    {
        totalCells = 0;
        char buf[48];

        snprintf(buf, 48, "../assets/samples/hard/%iHARD.bmp", i);
        // printf(buf);
        read_bitmap(buf, input_image);
        clock_t start, end;
        double cpu_time_used;

        start = clock();
        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                greyscale_image[i][j] =
                    (input_image[i][j][0] + input_image[i][j][1] + input_image[i][j][2]) /
                    3;
            }
        }
        // Blur the image slighty to reduce brightness of outer pixels
        // gaussianBlur();
        /*
        Find all cells
        */
        binaryThreshold(greyscale_image, 90); // Change to 90 to get the previos "Standard" threshold
        unsigned char dist[BMP_WIDTH][BMP_HEIGHT];
        distanceTransform(dist, greyscale_image);
        binaryThreshold(dist, otsu_threshold(dist));
        formatOutputImage(dist);
        write_bitmap(output_image, "../out/distThresh.bmp");
        distanceTransform(dist, dist);
        gaussianBlur(dist);
        // findAllMaximum(dist);
        formatOutputImage(dist);
        write_bitmap(output_image, "../out/distThresh2.bmp");
        binaryThreshold(dist, otsu_threshold(dist));
        formatOutputImage(dist);
        write_bitmap(output_image, "../out/distThresh3.bmp");

        for (int i = 0; i < BMP_WIDTH; i++)
        {
            for (int j = 0; j < BMP_HEIGHT; j++)
            {
                greyscale_image[i][j] = dist[i][j];
            }
        }
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

        erodeImage();
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
        formatOutputImage(greyscale_image);
        // Save image to file
        snprintf(buf, 48, "../out/%ifinal.bmp", i);
        write_bitmap(input_image, buf);
        printf("%i\n", totalCells);
    }
    return 0;
}