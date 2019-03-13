
#include "../../librawheaders/libraw.h"

void SaveImage(PGImage_t * Image, char * Name)
{
    int width = PGImageGetWidth(Image);
    int height = PGImageGetHeight(Image);
    uint8_t * bmpimg_2 = malloc(width*height*3);
    float * floatdata = PGImageGetDataPointer(Image);
    for (int i = 0; i < width*height; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float val = pow(floatdata[i*4+j], 0.45)*255.0;
            if (val > 255) val = 255;

            bmpimg_2[i*3+j] = val;
        }
    }
    writebmp(bmpimg_2, width, height, Name);
    free(bmpimg_2);
}

PGGraph_t * GraphTest(char * File)
{

    /******************************* LibRaw Test ******************************/

    if (File == NULL) {
        puts("No file specified");
        return 0;
    } else printf("file: %s\n", File);

    printf("Libraw version: \"%s\"\n", libraw_version());
    libraw_data_t * Raw = libraw_init(0);
    // puts("1");
    if (libraw_open_file(Raw, File)) puts("failed to open file");
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
    libraw_close(Raw); // basically a destructor
    // puts("6");
    puts("Done LibRawing");

    /**************************************************************************/



    /************************** ProcessingGraph Test **************************/

    void * test = new_PGImage(1,1);
    void * pointer = PGImageGetDataPointer(test);

    /* Guess node directory by using argv[0] */
    char * node_dir = "DefaultNodes";

    ProcessingGraphInitialiseLibrary(node_dir);

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

    PGGraphAddFile(graph, File);

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
    int LibRaw_node_index = PGGraphAddNodeByTypeName(graph, "LibRaw");
    PGNode_t * LibRaw_node = PGGraphGetNode(graph, LibRaw_node_index);
    PGNodeSetFileParameter(LibRaw_node, 0, 0);
    // PGNodeSetName(LibRaw_node, "ilia");
    #endif

    /* Exposure node */
    PGNode_t * demosaic_node = PGGraphGetNode(graph, PGGraphAddNodeByTypeName(graph, "Demosaic"));
    // PGNodeSetValueParameter(exposure_node, 0, 0);

    PGNode_t * exposure_node = PGGraphGetNode(graph, PGGraphAddNodeByTypeName(graph, "Exposure"));
    // PGNodeSetValueParameter(exposure_node2, 0, 3.0);

    PGNode_t * wb_node = PGGraphGetNode(graph, PGGraphAddNodeByTypeName(graph, "White Balance"));

    /* Exposure node */
    PGNode_t * tonemap_node = PGGraphGetNode(graph, PGGraphAddNodeByTypeName(graph, "Tonemap"));
    PGNodeSetName(tonemap_node, "tone mapping node");


    /* Add output node */
    int output_node_index = PGGraphAddNodeByTypeName(graph, "Output");
    printf("output node index: %i\n", output_node_index);
    PGNode_t * output_node = PGGraphGetNode(graph, output_node_index);


    PGNodeConnect(output_node, 0, tonemap_node, 0);
    PGNodeConnect(tonemap_node, 0, exposure_node, 0);
    PGNodeConnect(exposure_node, 0, wb_node, 0);
    PGNodeConnect(wb_node, 0, demosaic_node, 0);
    PGNodeConnect(demosaic_node, 0, LibRaw_node, 0);
    puts("HELLO this is text");


    // PGImage_t * a_graph_output_image = PGGraphGetOutput(graph);
    // graph_output_image = PGGraphGetOutput(graph);
    
    /**************************************************************************/



    /**************************** JSON STUFF TEST *****************************/
    JSONBlock_t * jsongraph = PGGraphToJSON(graph, "./");
    WriteJSON(jsongraph, 1, stdout);
    puts("");
    WriteJSON(jsongraph, 0, stdout);

    /* Just as a test...b
    save graph to JSON */
    FILE * jsonfile = fopen("json_graph.json", "wb+");
    WriteJSON(jsongraph, 0, jsonfile);
    fclose(jsonfile);

    /* Read that JSON to recreate it */
    jsonfile = fopen("json_graph.json", "r");
    char * text = calloc(100000, 1); /* Enough for some text */
    fseek(jsonfile, 0, SEEK_END);
    int szz = ftell(jsonfile);
    fseek(jsonfile, 0, SEEK_SET);
    fread(text, szz, 1, jsonfile);

    JSONBlock_t * graph_json = ParseJSON(text);
    // WriteJSON(graph_json, 1, jsonfile);

    PGGraph_t * graph_made_from_json = JSONToPGGraph(graph_json, "./");
    WriteJSON(PGGraphToJSON(graph_made_from_json, ""), 1, stdout);
    puts("\nwrote the JSONN");

    /* Now run the graph read from JSON */
    PGImage_t * graph_output_image = PGGraphGetOutput(graph_made_from_json);
    puts("hello");
    // delete_PGImage(graph_output_image);

    // graph_output_image = PGGraphGetOutput(graph_made_from_json);

    /**************************************************************************/



    /* Now save the output produced from the graph */
    printf("Image: %p\n", graph_output_image);
    SaveImage(graph_output_image, "Graph Pic.bmp");

    delete_PGImage(test);
    delete_PGGraph(graph);

    puts("GraphTest complete.");

    // delete_PGGraph(graph_made_from_json);
    return graph_made_from_json;

}