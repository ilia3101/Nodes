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
    return;
}


/* This node only needs one output function */
static void ii_output_function(PGNode_t * Node, PGNodeOutput_t * Output)
{
    /* If this is output is unused... */
    if (Output->type == 0)
    {
        Output->value.image = new_PGImage(100,100);
        Output->type = PGNodeImageOutput;
    }

    PGNodeGetOutput( PGNodeGetInputNode(Node, 0),
                     PGNodeGetInputNodeOutputIndex(Node, 0) );
}

static void ii_init(PGNode_t * Node)
{
    Node->own_data = MBZeroAlloc(Node->memory_bank, sizeof(input_node_data_t));
    return;
}

static void ii_uninit(PGNode_t * Node)
{
    return;
}


static PGNodeDataType_t ii_output_types[] = {PGNodeImageOutput};
static void (*output_func[1])(PGNode_t*, PGNodeOutput_t*)={&ii_output_function};

static PGNodeSpec_t ii_spec =
{
    .Name = "Input node",
    .Description = "Gives image input from RAM",
    .Category = "Debug",

    .NumOutputs = 1,
    .OutputTypes = ii_output_types,
    .NumInputs = 0,
    .InputTypes = NULL,

    .HasParameters = 0,
    .NumParameters = 0,
    .Parameters = NULL,

    .OutputFunctions = output_func,

    .Init = &ii_init,
    .UnInit = &ii_uninit

};


PGNodeSpec_t * PGNodeGetSpec()
{
    /* Set pointer in output function array to the output function */
    // ii_output_functions[0] = &ii_output_function;
    return &ii_spec;
}