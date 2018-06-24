/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>

extern void writebmp(uint8_t * data, int width, int height, char * imagename);

#include "../librawheaders/libraw.h"
#include "../ProcessingGraph/ProcessingGraph.h"


int main(int argc, char ** argv)
{
    /* Default plugin directory */
    // char * NodeDirectory = ".";

    puts("");
    printf("Libraw version: \"%s\"\n", libraw_version());
    libraw_data_t * Raw = libraw_init(0);
    // puts("1");
    libraw_open_file(Raw, "IMG_20180606_104935.dng");
    // puts("2");
    libraw_unpack(Raw);
    // puts("3");
    // libraw_raw2image(Raw);
    // libraw_dcraw_process(Raw);

    /* This is the bayer image */
    uint16_t * bayerimage = Raw->rawdata.raw_image;
    int width = libraw_get_raw_width(Raw);
    int height = libraw_get_raw_height(Raw);

    uint8_t * bmpimg = malloc(width*height*3);

    for (int i = 0; i < width*height*3; i+=3)
    {
        bmpimg[i] = bayerimage[i/3]>>2;
        bmpimg[i+1] = bayerimage[i/3]>>2;
        bmpimg[i+2] = bayerimage[i/3]>>2;
    }

    writebmp(bmpimg, width, height, "pic.bmp");


    // puts("4");
    libraw_recycle(Raw);
    // puts("5");
    libraw_close(Raw); // this is basically a destructor
    // puts("6");
    puts("Done LibRawing");




    return 0;
}

// compile with: "gcc -c -std=c99 main.c && gcc main.o libraw_r.a -lstdc++ -lm -fopenmp -o test"