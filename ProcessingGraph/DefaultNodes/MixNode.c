/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


/* This node only needs one output function */
static void output_function(PGNode_t * Node, PGNodeOutput_t * Ouptut)
{
    PGNodeOutput_t * input1 = PGNodeGetInput(Node, 0);
    PGNodeOutput_t * input2 = PGNodeGetInput(Node, 1);

    return input1;

    // if (input1->type == PGNodeImageOutput && input2->type == PGNode)
}

static void init(PGNode_t * Node)
{
    return;
}

static void uninit(PGNode_t * Node)
{
    return;
}

static void (* output_functions[])(PGNode_t *) = {&output_function};

static PGNodeParameterSpec_t parameters[] =
{
    {
        .Name = "Blend mode",
        .Description = "How to blend the two images",
        .Type = PGNodeOptionsParameter,
        .NumOptions = 4,
        .Options = {"Normal", "Add", "Subtract", "Multiply", "Difference"}
    },
    {
        .Name = "Factor",
        .Description = "Mix factor",
        .Type = 0, /* Range */
        .Integers = 0, /* Continous */
        .LimitRange = 1,
        .MinValue = 0.0,
        .MaxValue = 1.0,
        .DefaultValue = 0.5
    }
};

static PGNodeSpec_t spec =
{
    .Name = "Mix Node",
    .Description = "Mix images/colours",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput|PGNodeColourOutput},
    .NumInputs = 1,
    .InputTypes = { PGNodeImageOutput|PGNodeColourOutput,
                    PGNodeImageOutput|PGNodeColourOutput },

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){  },

    .OutputFunctions = {&output_function},

    .Init = &init,
    .UnInit = &uninit
};


PGNodeSpec_t * GetNodeSpec()
{
    spec.OutputFunctions = output_functions;
    printf("%p\n",spec.OutputFunctions[0]);
    /* Set pointer in output function array to the output function */
    return &spec;
}