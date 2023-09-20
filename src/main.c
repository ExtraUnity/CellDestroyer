#include "cbmp.h"
#include "init.c"
#include "preprocess.c"
#include "erosion.c"
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{

    for (int i = 1; i <= 10; i++)
    {
        int totalCells;

        //Load image to input_image
        char buf[256];
        snprintf(buf, 256, "../assets/samples/hard/%iHARD.bmp", i);
        read_bitmap(buf, input_image);

        //Start clock for time analysis
        clock_t start, end;
        double cpu_time_used;
        start = clock();

        //Transform image to grey and threshold it using a experimentally found 'low' threshold
        greyTransform(input_image, greyscale_image);
        binaryThreshold(greyscale_image, 90);
        
        //Calculate the distance transform of the binary image and threshold using otsu's method
        distanceTransform(greyscale_image);
        binaryThreshold(greyscale_image, otsu_threshold(greyscale_image));

        //Total time for pre-processing
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);


        /*
        Find all cells - hopefully :D
        */
        
        totalCells = erodeImage(greyscale_image, output_image);

        //Total time of algorithm
        end = clock();
        cpu_time_used = end - start;
        printf("%fms ", cpu_time_used * 1000.0 / CLOCKS_PER_SEC);

        // Save image to file
        snprintf(buf, 48, "../out/%ifinal.bmp", i);
        write_bitmap(input_image, buf);
        printf("%i\n", totalCells);
    }
    return 0;
}