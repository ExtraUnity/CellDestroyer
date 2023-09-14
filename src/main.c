#include "cbmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Declaring the array to store the image (unsigned char = unsigned 8 bit)
unsigned char input_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
unsigned char greyscale_image[BMP_WIDTH][BMP_HEIGHT];
unsigned char output_image[BMP_WIDTH][BMP_HEIGHT][BMP_CHANNELS];
int totalCells = 0;
int threshold_value = 0;


//Better threshold value based on Otsu Method
int otsu_threshold() {
  int histogram[256] = {0}; // Assuming 8-bit grayscale
  int totalPixels = BMP_WIDTH * BMP_HEIGHT;

  // Initialize histogram of greyscale values:
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      histogram[greyscale_image[i][j]]++;
    }
  }

  // Calculate the threshold using Otsu's method
  int sum = 0;
  for (int i = 0; i < 256; i++) {
    sum += i * histogram[i];
  }

  int sumB = 0;
  int wB = 0;
  int wF = 0;
  int mB, mF;
  double maxVariance = 0.0;
  int threshold = 0;

  for (int i = 0; i < 256; i++) {
    wB += histogram[i];
    if (wB == 0)
      continue;

    wF = totalPixels - wB;
    if (wF == 0)
      break;

    sumB += i * histogram[i];
    mB = sumB / wB;
    mF = (sum - sumB) / wF;

    //Uses the Otsu Formula to calculate the "in-between" Variance:
    double varianceBetween = (double)wB * wF * (mB - mF) * (mB - mF) / (wB + wF);
    if (varianceBetween > maxVariance) {
      maxVariance = varianceBetween;
      threshold = i;
    }
  }

//Change the global variable of threshold value
printf("\nBinary Threshold Value: %d\n",threshold);
return threshold;
}


// Marks the cell with a red cross in output_image
void markCell(int x, int y) {

  // Cross sign with size and "Red" color (R, G, B)
  unsigned char color[3] = {255, 0, 0};
  int size = 5;

  for (int dx = -size; dx <= size; dx++) {
    // Ensures inside border edge horizontally (Redundant?)
    if (x + dx >= 0 && x + dx < BMP_WIDTH) {
      // Draws the horizontal line
      input_image[x + dx][y][0] = color[0];
      input_image[x + dx][y][1] = color[1];
      input_image[x + dx][y][2] = color[2];
    }
  }
  for (int dy = -size; dy <= size; dy++) {
    // Ensures inside border edge vertically (Redundant?)
    if (y + dy >= 0 && y + dy < BMP_HEIGHT) {
      // Draws the vertical line
      input_image[x][y + dy][0] = color[0];
      input_image[x][y + dy][1] = color[1];
      input_image[x][y + dy][2] = color[2];
    }
  }
}

// Check the four borders for white pixels
int excludeCell(int x, int y) {
  // Check left exclusion border
  for (int dy = -6; dy <= 7; dy++) {
    if (x - 6 < 0) {
      break;
    }
    if (y + dy < 0 || y + dy > BMP_HEIGHT - 1) {
      continue;
    }

    if (greyscale_image[x - 6][y + dy] == 255) {
      return 1;
    }
  }
  // Check right exclusion border
  for (int dy = -6; dy <= 7; dy++) {
    if (x + 7 > BMP_WIDTH - 1) {
      break;
    }
    if (y + dy < 0 || y + dy > BMP_HEIGHT - 1) {
      continue;
    }

    if (greyscale_image[x + 7][y + dy] == 255) {
      return 1;
    }
  }

  // Check top exclusion border
  for (int dx = -6; dx <= 7; dx++) {
    if (y - 6 < 0) {
      break;
    }
    if (x + dx < 0 || x + dx > BMP_WIDTH - 1) {
      continue;
    }

    if (greyscale_image[x + dx][y - 6] == 255) {
      return 1;
    }
  }

  // Check bottom exclusion border
  for (int dx = -6; dx <= 7; dx++) {
    if (y + 7 > BMP_HEIGHT - 1) {
      break;
    }
    if (x + dx < 0 || x + dx > BMP_WIDTH - 1) {
      continue;
    }

    if (greyscale_image[x + dx][y + 7] == 255) {
      return 1;
    }
  }

  return 0;
}

void removeCell(int x, int y) {
  for (int dx = -5; dx <= 6; dx++) {
    if (x + dx < 0 || x + dx > BMP_WIDTH - 1) { // Check if on x-edge
      continue;
    }
    for (int dy = -5; dy <= 6; dy++) {
      if (y + dy < 0 || y + dy > BMP_HEIGHT - 1) { // Check if on y-edge
        continue;
      }
      greyscale_image[x + dx][y + dy] = 0;
    }
  }
}

int cellInFrame(int x, int y) {
  for (int dx = -5; dx <= 6; dx++) {
    if (x + dx < 0 || x + dx > BMP_WIDTH - 1) { // Check if on x-edge
      continue;
    }
    for (int dy = -5; dy <= 6; dy++) {
      if (y + dy < 0 || y + dy > BMP_HEIGHT - 1) { // Check if on y-edge
        continue;
      }

      if (greyscale_image[x + dx][y + dy] > 0) {

        return 1;
      }
    }
  }
  return 0;
}

void detectCells() {
  // Loop through all pixels
  for (int x = 0; x < BMP_WIDTH; x++) {
    for (int y = 0; y < BMP_HEIGHT; y++) {
      // Check exclusion border first
      if (excludeCell(x, y)) {
        continue;
      }
      if (cellInFrame(x, y)) {

        // Marks the cells with a red cross
        markCell(x, y);
        removeCell(x, y);
        totalCells++;
        /*
         *
         * INCREMENT COUNTER,
         * ADD COORDINATES TO ARRAY
         * ADD RED CROSS TO OUTPUT IMAGE (in-progress)
         *
         */
      }
    }
  }
}

void formatOutputImage(unsigned char input[BMP_WIDTH][BMP_HEIGHT]) {
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      for (int k = 0; k < BMP_CHANNELS; k++) {

        output_image[i][j][k] = input[i][j];
      }
    }
  }
}

void erodeImage() {
  int erosionNumber = 0;
  int hasEroded = 1;

  int kernel[3][3] = {
      {0, 1, 0},
      {1, 1, 1},
      {0, 1, 0},
  };

  unsigned char erodedImage[BMP_WIDTH][BMP_HEIGHT];
  char fileName[256];
  while (hasEroded) {
    erosionNumber++;
    hasEroded = 0;
    // Copy original
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGHT; j++) {
        erodedImage[i][j] = greyscale_image[i][j];
      }
    }

    // Loop through all pixels
    for (int x = 0; x < BMP_WIDTH; x++) {
      for (int y = 0; y < BMP_HEIGHT; y++) {
        // If pixel is black then continue
        if (greyscale_image[x][y] == 0) {
          continue;
        }

        int erode = 0;

        // Apply kernel for selected pixel
        for (int kx = -1; kx <= 1; kx++) {
          if (x + kx < 0 || x + kx > BMP_WIDTH - 1) { // Check if on x-edge
            continue;
          }

          for (int ky = -1; ky <= 1; ky++) {
            if (y + ky < 0 || y + ky > BMP_HEIGHT - 1) { // Check if on y-edge
              continue;
            }

            if (kernel[kx + 1][ky + 1] ==
                0) { // If 0 in kernel then no need to check
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
        if (erode) {
          erodedImage[x][y] = 0;
        }
      }
    }

    // Copy eroded image to working image
    for (int i = 0; i < BMP_WIDTH; i++) {
      for (int j = 0; j < BMP_HEIGHT; j++) {
        greyscale_image[i][j] = erodedImage[i][j];
      }
    }

    // Save erosion image to file and detect cells
    if (hasEroded) {
      sprintf(fileName, "../out/eroded%d.bmp", erosionNumber);
      formatOutputImage(erodedImage);
      write_bitmap(output_image, fileName);
      detectCells();
    }
  }
}

//Binary Threshold based on global value "threshold_value"
void binaryThreshold() {
  threshold_value = otsu_threshold();  //Change to 90 to get the previos "Standard" threshold
  for (int i = 0; i < BMP_WIDTH; i++) {
    for (int j = 0; j < BMP_HEIGHT; j++) {
      if (greyscale_image[i][j] >= threshold_value) {
        greyscale_image[i][j] = 255; // white
      }
      if (greyscale_image[i][j] < threshold_value) {
        greyscale_image[i][j] = 0; // black
      }
    }
  }
}

int main(int argc, char **argv) {

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <output file path> <output file path>\n",
            argv[0]);
    exit(1);
  }

    // Load image from file
    for (int i = 1; i <= 10; i++)
    {
        totalCells = 0;
        char buf[48];

        snprintf(buf, 48, "../assets/samples/easy/%iEASY.bmp", i);
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

        /*
        Find all cells
        */
        binaryThreshold();
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
        erodeImage();
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);
        formatOutputImage(greyscale_image);
        // Save image to file
        snprintf(buf, 48, "../out/%iEASYFinal.bmp", i);
        write_bitmap(input_image, buf);
        printf("%i\n", totalCells);
    }
    // printf("Number of cells has not been counted yet :(");
    return 0;
}