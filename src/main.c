#include "cbmp.h"
#include "init.c"
#include "preprocess.c"
#include "erosion.c"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
 

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <difficulty mode> <start index (1-10)> <end index (1-10)>\n", argv[0]);
        exit(1);
    }
    char *mode = argv[1];
    for (int i = atoi(argv[2]); i <= atoi(argv[3]); i++)
    {
        int totalCells;
        // Load image to input_image
        char buf[256];
        char *copy;
        strcpy(copy,mode);
        snprintf(buf, 256, "../assets/samples/%s/%i%s.bmp", strlwr(copy), i, strupr(mode));
        read_bitmap(buf, input_image);
        
        // Start clock for time analysis
        clock_t start, end;
        double cpu_time_used;
        double preprocess_time;
        start = clock();

        // Transform image to grey and threshold it using a experimentally found 'low' threshold
        greyTransform(input_image, greyscale_image);
        binaryThreshold(greyscale_image, 90);

        // Calculate the distance transform of the binary image and threshold using otsu's method
         distanceTransform(greyscale_image);
         // formatOutputImage(greyscale_image, output_image);
         // write_bitmap(output_image, "../out/dist.bmp");
         binaryThreshold(greyscale_image, otsu_threshold(greyscale_image));

        // Total time for pre-processing
        end = clock();
        preprocess_time = end - start;

        /*
        Find all cells - hopefully :D
        */

        totalCells = erodeImage(greyscale_image, output_image);
        // Total time of algorithm
        end = clock();
        cpu_time_used = end - start;
        printf("%fms  %fms ", preprocess_time * 1000.0 / CLOCKS_PER_SEC, cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

        // Save image to file
        snprintf(buf, 48, "../out/%i%sfinal.bmp", i, strupr(mode));
        write_bitmap(input_image, buf);
        printf("%i\n", totalCells);
    }
    return 0;
}