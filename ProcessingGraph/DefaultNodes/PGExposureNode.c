/* A node must be a .so file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"

PGNodeOutput_t en_output_function(PGNode_t * Node)
{
    /* PGNodeGetInput(index 0) */
    PGNodeOutput_t output = { .type = PGNodeImageOutput, 
                              .value.image = new_PGImage(600, 600) };

    return output;
}

void en_init(PGNode_t * Node, MemoryBank_t * MemoryBank)
{
    return;
}

void en_uninit(PGNode_t * Node)
{
    return;
}

PGNodeDataType_t en_output_types[] = {PGNodeImageOutput};
PGNodeOutput_t (* en_output_functions[])(PGNode_t *) = {&en_output_function};

PGNodeSpec_t pg_exposure_node_spec =
{
    .Name = "Exposure",
    .Description = "This node adjusts the exposure of an image",
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
    return &pg_exposure_node_spec;
}