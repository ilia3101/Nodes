/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


/* This node only needs one output function */
static void output_function(PGNode_t * Node)
{
    PGImage_t * input0 = PGNodeGetInput(Node, 0)->value.image;
    PGImage_t * input1 = PGNodeGetInput(Node, 1)->value.image;

    PGNodeOutput_t * output = &Node->outputs[0];

    /* If this is output is unused... */
    if (output->type == 0)
    {
        output->value.image = new_PGImage(1,1);
        output->type = PGNodeImageOutput;
    }

    PGImage_t * img = output->value.image;
    
    /* Make image be right size */
    PGImageSetDimensions(img, PGImageGetWidth(input0), PGImageGetHeight(input0));

    float * dst = PGImageGetDataPointer(img);

    float * img0 = PGImageGetDataPointer(input0);
    float * img1 = PGImageGetDataPointer(input1);

    float mix0 = PGNodeGetValueParameterValue(Node, 0);
    float mix1 = 1.0 - mix0;

    size_t sz = PGImageGetWidth(img)*PGImageGetHeight(img)*4;

    /* TODO: proper alpha blending */
    for (size_t i = 0; i < sz; i+=4)
    {
        dst[ i ] = img0[ i ]*mix0 + img1[ i ]*mix1;
        dst[i+1] = img0[i+1]*mix0 + img1[i+1]*mix1;
        dst[i+2] = img0[i+2]*mix0 + img1[i+2]*mix1;
        dst[i+3] = img0[i+3]*mix0 + img1[i+3]*mix1;
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
static char * input_names[] = {"Image 0","Image 1"};
static char * output_names[] = {"Image"};

static PGNodeSpec_t spec =
{
    .Name = "Mix",
    .Description = "Adjust exposure of an image",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputNames = output_names,
    .OutputTypes = {PGNodeImageOutput},

    .NumInputs = 2,
    .InputNames = input_names,
    .InputTypes = {PGNodeImageOutput,PGNodeImageOutput},

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){ .Name = "Mix",
                                            .Description = "Mix factor",
                                            .Type = 0, /* Range */
                                            .Integers = 0, /* Continous */
                                            .LimitRange = 1,
                                            .MinValue = 0.0,
                                            .MaxValue = 1.0,
                                            .DefaultValue = 1.0 },

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