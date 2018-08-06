/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


/* This node only needs one output function */
static void output_function(PGNode_t * Node, PGNodeOutput_t * Output)
{
    // PGNodeGetOutput( PGNodeGetInputNode(Node, 0),
    //                  PGNodeGetInputNodeOutputIndex(Node, 0) );
}

static void init(PGNode_t * Node)
{
    return;
}

static void uninit(PGNode_t * Node)
{
    return;
}


static PGNodeSpec_t spec =
{
    .Name = "Exposure",
    .Description = "Adjust exposure of an image",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 1,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){ .Name = "Exposure",
                                            .Description = "Exposure in stops",
                                            .Type = 0, /* Range */
                                            .Integers = 0, /* Continous */
                                            .LimitRange = 1,
                                            .MinValue = -4.0,
                                            .MaxValue = 4.0,
                                            .DefaultValue = 0.0 },

    .OutputFunctions = {&output_function},

    .Init = &init,
    .UnInit = &uninit
};


PGNodeSpec_t * GetNodeSpec()
{
    /* Set pointer in output function array to the output function */
    return &spec;
}