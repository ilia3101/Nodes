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
    PGNodeOutput_t * input1 = PGNodeGetInput(Node, 0);
    PGNodeOutput_t * input2 = PGNodeGetInput(Node, 1);

    return input1;

    // if (input1->type == PGNodeImageOutput && input2->type == PGNode)
}

/* The cleanup function that frees the output or whatever */
static void output_cleanup_function(PGNode_t * Node, PGNodeOutput_t * Output)
{
    /* Only needed if it is an image */
    if (Output->type == PGNodeImageOutput)
    {
        delete_PGImage(Output->value.image);
    }
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
        .Description = "How the inputs are blended",
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
    .OutputNames = {"Image"},
    .OutputTypes = {PGNodeImageOutput|PGNodeColourOutput},

    .NumInputs = 1,
    .InputNames = {"Image A", "Image B"},
    .InputTypes = { PGNodeImageOutput|PGNodeColourOutput,
                    PGNodeImageOutput|PGNodeColourOutput },

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){  },

    .OutputFunctions = {&output_function},
    .OutputCleanupFunctions = {&output_cleanup_function},

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