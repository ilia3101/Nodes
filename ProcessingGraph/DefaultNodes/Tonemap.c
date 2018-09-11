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
    PGImage_t * input = PGNodeGetInput(Node, 0)->value.image;

    PGNodeOutput_t * output = &Node->outputs[0];

    /* If this is output is unused... */
    if (output->type == 0)
    {
        output->value.image = new_PGImage(1,1);
        output->type = PGNodeImageOutput;
    }

    PGImage_t * img = output->value.image;
    
    /* Make image be right size */
    PGImageSetDimensions(img, PGImageGetWidth(input), PGImageGetHeight(input));
    
    float * restrict dest = PGImageGetDataPointer(img);
    float * restrict end = dest + PGImageGetWidth(img)*PGImageGetHeight(img)*4;
    float * restrict src = PGImageGetDataPointer(input);

    printf("source: %p, %f\n", src, src[3]);

    size_t sz = PGImageGetWidth(img)*PGImageGetHeight(img)*4;
    while (dest < end)
    {
        dest[0] = src[0] / (1.0f + src[0]);
        dest[1] = src[1] / (1.0f + src[1]);
        dest[2] = src[2] / (1.0f + src[2]);
        dest[3] = src[3];

        dest += 4;
        src += 4;
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

static PGNodeSpec_t spec =
{
    .Name = "Tonemap",
    .Description = "Softly rolls off the highlights instead of harsh clipping.",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 1,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 0,

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