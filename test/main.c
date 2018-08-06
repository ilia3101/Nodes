/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>


#include <dlfcn.h>


extern void writebmp(uint8_t * data, int width, int height, char * imagename);

#include "../librawheaders/libraw.h"
#include "../ProcessingGraph/ProcessingGraph.h"


int main(int argc, char ** argv)
{

    /******************************* LibRaw Test ******************************/

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
    float * raw_float = malloc(width*height*3*sizeof(float));

    // int p = bayerimage[];

    int shift = 6;
    for (size_t i = 0; i < width*height*3; i+=3)
    {
        bmpimg[i] = bayerimage[i/3]>>shift;
        bmpimg[i+1] = bayerimage[i/3]>>shift;
        bmpimg[i+2] = bayerimage[i/3]>>shift;

        raw_float[i] = bayerimage[i/3] / 65536.0;
        raw_float[i+1] = bayerimage[i/3] / 65536.0;
        raw_float[i+2] = bayerimage[i/3] / 65536.0;
    }

    writebmp(bmpimg, width, height, "pic.bmp");


    // puts("4");
    libraw_recycle(Raw);
    // puts("5");
    libraw_close(Raw); // this is basically a destructor
    // puts("6");
    puts("Done LibRawing");

    /**************************************************************************/




    /************************** ProcessingGraph Test **************************/

    void * test = new_PGImage(1,1);
    void * pointer = PGImageGetDataPointer(test);

    ProcessingGraphInitialiseLibrary("/home/ilia/ProcessingGraphApp/test/buildoutput/DefaultNodes");

    PGGraph_t * graph = new_PGGraph();

    int num_nodes = ProcessingGraphGetNumNodeTypes();
    PGNodeSpec_t ** nodes = ProcessingGraphGetNodeTypes();

    puts("\nAvailable Nodes:");
    for (int i = 0; i < num_nodes; ++i) {
        printf("\n%s\n%s\n", nodes[i]->Name, nodes[i]->Description);
    }
    puts(" ");


    int node_index = PGGraphAddNode(graph, nodes[1]);

    void * dll = ProcessingGraphGetNodeDLLHandle(1);
    void (* set_image_data)(PGNode_t *, float *, int, int) = dlsym(dll, "ImageInputNodeSetImageData");

    // if (PGGraphGetNode(graph, node_index) == NULL) puts("GREAT FAIL");


    set_image_data(PGGraphGetNode(graph, node_index), raw_float, width, height);

    /**************************************************************************/


    return 0;
}
