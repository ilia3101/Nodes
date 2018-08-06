#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


typedef struct {
    float * imageinput;
    int width, height;
} input_node_data_t;


void ImageInputNodeSetImageData( PGNode_t * Node, float * Data,
                                 int Width, int Height )
{
    input_node_data_t * data = Node->own_data;

    data->width = Width;
    data->height = Height;
    data->imageinput = Data;
}


/* This node only needs one output function */
static void output_function(PGNode_t * Node, PGNodeOutput_t * Output)
{
    /* If this is output is unused... */
    if (Output->type == 0)
    {
        Output->value.image = new_PGImage(1,1);
        Output->type = PGNodeImageOutput;
    }

    // PGNodeGetOutput( PGNodeGetInputNode(Node, 0),
    //                  PGNodeGetInputNodeOutputIndex(Node, 0) );
}

static void init(PGNode_t * Node)
{
    puts("\n\n\nGREAT SUCCESS!!!\n\n\n");
    Node->own_data = MBZeroAlloc(Node->memory_bank, sizeof(input_node_data_t));
    return;
}

static void uninit(PGNode_t * Node)
{
    return;
}


PGNodeDataType_t output_types[] = {PGNodeImageOutput};

static PGNodeSpec_t spec =
{
    .Name = "Input node",
    .Description = "Gives image input from RAM",
    .Category = "Debug",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 0,
    .InputTypes = NULL,

    .HasParameters = 0,
    .NumParameters = 0,
    .Parameters = NULL,

    .OutputFunctions = {&output_function},

    .Init = init,
    .UnInit = uninit
};


PGNodeSpec_t * PGNodeGetSpec()
{
    // ii_spec.Init = ii_init;
    /* Set pointer in output function array to the output function */
    // ii_output_functions[0] = &ii_output_function;
    return &spec;
}