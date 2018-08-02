/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


/* This node only needs one output function */
void en_output_function(PGNode_t * Node, PGNodeOutput_t * Output)
{
    PGNodeGetOutput( PGNodeGetInputNode(Node, 0),
                     PGNodeGetInputNodeOutputIndex(Node, 0) );
}

// PGNodeOutput_t PGNodeSpecGetOutputFunction(PGNode_t * Node, int FunctionIndex)
// {
//     /* PGNodeGetInput(index 0) */
//     PGNodeOutput_t output = { .type = PGNodeImageOutput, 
//                               .value.image = new_PGImage(600, 600) };

//     return output;
// }

void en_init(PGNode_t * Node, MemoryBank_t * MemoryBank)
{
    return;
}

void en_uninit(PGNode_t * Node)
{
    return;
}


static PGNodeDataType_t en_output_types[] = {PGNodeImageOutput};
static void (* en_output_functions[1])(PGNode_t *, PGNodeOutput_t *);

static PGNodeSpec_t en_spec =
{
    .Name = "Exposure",
    .Description = "Adjust exposure of an image",
    .Category = "Basics",

    .NumOutputs = 1,
    .NumInputs = 1,
    .OutputTypes = en_output_types,

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){ .Name = "Exposure",
                                            .Description = "Exposure in stops",
                                            .Type = 0, /* Range */
                                            .Integers = 0, /* Continous */
                                            .MinValue = -4.0,
                                            .MaxValue = 4.0,
                                            .DefaultValue = 0.0 },

    .OutputFunctions = en_output_functions,

    .Init = &en_init,
    .UnInit = &en_uninit

};


PGNodeSpec_t * PGNodeGetSpec()
{
    /* Set pointer in output function array to the output function */
    en_output_functions[0] = &en_output_function;
    return &en_spec;
}