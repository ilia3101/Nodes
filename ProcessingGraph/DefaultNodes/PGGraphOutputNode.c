/* The main node that goes at the end of the graph */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


PGImage_t * PGOutputNodeGetOutput()
{
    return NULL;
}


static void init(PGNode_t * Node)
{
    puts("\n\n\nCreated output node!!!\n\n\n");
}


static void uninit(PGNode_t * Node)
{
    return;
}


static PGNodeSpec_t spec =
{
    .Name = "Output",
    .Description = "The endpoint of a graph.",
    .Category = "Graph",

    .NumOutputs = 0,
    .OutputTypes = NULL,
    .NumInputs = 1,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 0,
    .NumParameters = 0,
    .Parameters = NULL,

    .OutputFunctions = NULL,

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