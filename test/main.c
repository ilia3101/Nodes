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
    InitialiseProcessingGraphLibrary("/home/ilia/ProcessingGraphApp/test/buildoutput/DefaultNodes");

    if (argc == 1) {
        puts("No file specified");
        return 0;
    } else printf("file: %s\n", argv[1]);

    printf("Libraw version: \"%s\"\n", libraw_version());
    libraw_data_t * Raw = libraw_init(0);
    // puts("1");
    if (libraw_open_file(Raw, argv[1])) puts("failed to open file");
    // puts("2");
    if (libraw_unpack(Raw)) puts("failed to unpack");
    // puts("3");
    // libraw_raw2image(Raw);
    // libraw_dcraw_process(Raw);

    /* This is the bayer image */
    uint16_t * bayerimage = Raw->rawdata.raw_image;
    int width = libraw_get_raw_width(Raw);
    int height = libraw_get_raw_height(Raw);
    // puts("3");

    uint8_t * bmpimg = malloc(width*height*3);

    // int p = bayerimage[];

    int shift = 6;
    for (size_t i = 0; i < width*height*3; ++i)
    {
        bmpimg[i] = bayerimage[i/3]>>shift;
        bmpimg[++i] = bayerimage[i/3]>>shift;
        bmpimg[++i] = bayerimage[i/3]>>shift;
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
