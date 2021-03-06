/* The main node that goes at the end of the graph */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"

static void init(PGNode_t * Node)
{
    return;
}

static void uninit(PGNode_t * Node)
{
    return;
}

static char * input_names[] = {"Image"};

static PGNodeSpec_t spec =
{
    .Name = "Output",
    .Description = "The endpoint of a graph.",
    .Category = "Graph",

    .NumOutputs = 0,
    .OutputTypes = NULL,

    .NumInputs = 1,
    .InputNames = input_names,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 0,
    .NumParameters = 0,
    .Parameters = NULL,

    .OutputFunctions = NULL,

    .Init = init,
    .UnInit = uninit
};


PGNodeSpec_t * GetNodeSpec()
{
    return &spec;
}