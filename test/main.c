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
#include "../JSONParser/JSONParser.h"
#include "../GraphJSON/GraphJSON.h"


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

    /* Does notihng, just for test purposes */
    // PGGraphAddNode(graph, nodes[1]);
    // PGGraphAddNode(graph, nodes[0]);
    // PGGraphAddNode(graph, nodes[0]);

    PGGraphAddFile(graph, argv[1]);

    #if 0
    /* RAm input */
    int image_input_node_index = PGGraphAddNode(graph, nodes[2]);
    PGNode_t * image_input_node = PGGraphGetNode(graph, image_input_node_index);
    char pointer_as_text[40];
    sprintf(pointer_as_text, "%p", raw_float);
    PGNodeSetTextParameter(image_input_node, 0, pointer_as_text);
    PGNodeSetValueParameter(image_input_node, 1, width);
    PGNodeSetValueParameter(image_input_node, 2, height);
    #else
    int LibRaw_node_index = PGGraphAddNode(graph, nodes[3]);
    PGNode_t * LibRaw_node = PGGraphGetNode(graph, LibRaw_node_index);
    PGNodeSetFileParameter(LibRaw_node, 0, 0);
    #endif

    /* Exposure node */
    PGNode_t * exposure_node = PGGraphGetNode(graph, PGGraphAddNode(graph, nodes[0]));
    PGNodeSetValueParameter(exposure_node, 0, 4.6);


    /* Add output node */
    int output_node_index = PGGraphAddNode(graph, nodes[2]);
    printf("output node index: %i\n", output_node_index);
    PGNode_t * output_node = PGGraphGetNode(graph, output_node_index);
    // PGGraphGetNode(graph, output_node_index);

    /* Output node input #0 to exposure node output #0 */
    PGNodeConnect(output_node, 0, exposure_node, 0);
    /* exposure node input #0 to LibRaw node output #0 */
    PGNodeConnect(exposure_node, 0, LibRaw_node, 0);


    // PGImage_t * a_graph_output_image = PGGraphGetOutput(graph);
    // graph_output_image = PGGraphGetOutput(graph);
    
    /**************************************************************************/



    /**************************** JSON STUFF TEST *****************************/
    JSONBlock_t * jsongraph = PGGraphToJSON(graph, "./");
    WriteJSON(jsongraph, 1, stdout);
    puts("");
    WriteJSON(jsongraph, 0, stdout);
    puts("");

    FILE * jsonfile = fopen("json_graph.json", "wb+");
    WriteJSON(jsongraph, 0, jsonfile);

    fclose(jsonfile);
    jsonfile = fopen("json_graph.json", "r");
    char * text = calloc(100000, 1);
    fseek(jsonfile, 0, SEEK_END);
    int szz = ftell(jsonfile);
    fseek(jsonfile, 0, SEEK_SET);
    fread(text, szz, 1, jsonfile);

    JSONBlock_t * graph_json = ParseJSON(text);
    // WriteJSON(graph_json, 1, jsonfile);

    PGGraph_t * graph_made_from_json = JSONToPGGraph(graph_json, "./");

    /* Now run the graph read from JSON */
    PGImage_t * graph_output_image = PGGraphGetOutput(graph_made_from_json);
    // graph_output_image = PGGraphGetOutput(graph_made_from_json);

    /**************************************************************************/



    /* Now save the output produced from the graph */
    printf("Image: %p\n", graph_output_image);
    width = PGImageGetWidth(graph_output_image);
    height = PGImageGetHeight(graph_output_image);
    uint8_t * bmpimg_2 = malloc(width*height*3);
    float * floatydata = PGImageGetDataPointer(graph_output_image);
    for (int i = 0; i < width*height; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float val = floatydata[i*4+j]*255.0;
            if (val > 255) val = 255;

            bmpimg_2[i*3+j] = val;
        }
    }
    writebmp(bmpimg_2, width, height, "pic_graph.bmp");


    free(bmpimg_2);
    delete_PGImage(test);
    delete_PGGraph(graph);
    delete_PGGraph(graph_made_from_json);

    sleep(100);

    return 0;
}

//cloc ../ProcessingGraph/ ../test/ ../JSONParser/ ../GraphJSON/ ../MemoryBank/