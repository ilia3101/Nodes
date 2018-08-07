#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"


/* This node only needs one output function */
static void output_function(PGNode_t * Node)
{
    PGNodeOutput_t * output = &Node->outputs[0];

    /* If this is output is unused... */
    if (output->type == 0)
    {
        output->value.image = new_PGImage(1,1);
        output->type = PGNodeImageOutput;
    }

    PGImage_t * img = output->value.image;

    int width = PGNodeGetValueParameterValue(Node, 1);
    int height = PGNodeGetValueParameterValue(Node, 2);
    
    /* Make image be right size */
    PGImageSetDimensions(img, width, height);
    
    float * imagedata = PGImageGetDataPointer(img);

    float * srcdata;
    sscanf(PGNodeGetTextParameterValue(Node, 0), "%p", &srcdata);
    memcpy(imagedata, srcdata, width*height*sizeof(float)*3);
}


static void init(PGNode_t * Node)
{
    return;
}


static void uninit(PGNode_t * Node)
{
    return;
}

static PGNodeParameterSpec_t parameters[] =
{
    {
        .Name = "Pointer",
        .Description = "Pointer to RAM location of image",
        .Type = PGNodeStringParameter
    },
    {
        .Name = "Width",
        .Description = "How many pixels wide the image is",
        .Type = PGNodeValueParameter,
        .Integers = 1,
        .LimitRange = 0,
        .DefaultValue = 0
    },
    {
        .Name = "Height",
        .Description = "How many pixels high the image is",
        .Type = PGNodeValueParameter,
        .Integers = 1,
        .LimitRange = 0,
        .DefaultValue = 0
    }
};

static void (* output_functions[])(PGNode_t *) = {&output_function};

static PGNodeSpec_t spec =
{
    .Name = "RAM Input",
    .Description = "Gives image input from RAM",
    .Category = "Debug",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 0,
    .InputTypes = NULL,

    .HasParameters = 1,
    .NumParameters = 3,
    .Parameters = parameters,

    .OutputFunctions = NULL,

    .Init = init,
    .UnInit = uninit
};

PGNodeSpec_t * GetNodeSpec()
{
    spec.OutputFunctions = output_functions;
    // ii_spec.Init = ii_init;
    /* Set pointer in output function array to the output function */
    // ii_output_functions[0] = &ii_output_function;
    return &spec;
}