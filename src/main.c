#include "cbmp.h"
#include "erosion.c"
#include "init.c"
#include "preprocess.c"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Usage: %s <directory> <start> <end>\n", argv[0]);
    return 1;
  }

  char *directory = argv[1];
  int start = atoi(argv[2]);
  int end = atoi(argv[3]);

  // Convert the first argument (directory name) to uppercase
  char uppercasedDirectory[256];
  strcpy(uppercasedDirectory, directory);
  strToUpper(uppercasedDirectory);
  strToLower(directory);

  for (int i = start; i <= end; i++) {
    int totalCells;

    // Load image to input_image
    char buf[256];
    snprintf(buf, 256, "../assets/samples/%s/%i%s.bmp", directory, i,uppercasedDirectory);
    read_bitmap(buf, input_image);

    // Start clock for time analysis
    clock_t start, end;
    double cpu_time_used;
    double preprocess_time;
    start = clock();

    // Transform image to grey and threshold it using a experimentally found
    // 'low' threshold
    greyTransform(input_image, working_image);
    binaryThreshold(working_image, 90);

    // Calculate the distance transform of the binary image and threshold
    // using otsu's method
    distanceTransform(working_image);
    formatOutputImage(working_image, output_image);
    write_bitmap(output_image, "../out/dist.bmp");
    binaryThreshold(working_image, otsu_threshold(working_image));

    // Total time for pre-processing
    end = clock();
    preprocess_time = end - start;

    /*
    Find all cells - hopefully :D
    */

    totalCells = erodeImage(working_image, output_image);
    // Total time of algorithm
    end = clock();
    cpu_time_used = end - start;
    printf("%fms  %fms ", preprocess_time * 1000.0 / CLOCKS_PER_SEC,
           cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

    // Save image to file
    snprintf(buf, 48, "../out/%iEASYfinal.bmp", i);
    write_bitmap(input_image, buf);
    printf("%i\n", totalCells);
  }
  return 0;
}
