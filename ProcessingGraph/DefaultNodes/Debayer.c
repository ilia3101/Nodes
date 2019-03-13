/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"

#include "Debayer/AMaZE_algorithm.c"

/* Wraper for the horrible amaze function */
void debayerAmaze(float * __restrict debayerto, float * __restrict bayerdata, int width, int height)
{
    size_t pixelsize = width * height;

    float ** __restrict imagefloat2d = (float **)malloc(height * sizeof(float *));
    for (int y = 0; y < height; ++y) imagefloat2d[y] = (float *)(bayerdata+(y*width));

    float  * __restrict red1d = (float *)malloc(pixelsize * sizeof(float));
    float ** __restrict red2d = (float **)malloc(height * sizeof(float *));
    for (int y = 0; y < height; ++y) red2d[y] = (float *)(red1d+(y*width));
    float  * __restrict green1d = (float *)malloc(pixelsize * sizeof(float));
    float ** __restrict green2d = (float **)malloc(height * sizeof(float *));
    for (int y = 0; y < height; ++y) green2d[y] = (float *)(green1d+(y*width));
    float  * __restrict blue1d = (float *)malloc(pixelsize * sizeof(float));
    float ** __restrict blue2d = (float **)malloc(height * sizeof(float *));
    for (int y = 0; y < height; ++y) blue2d[y] = (float *)(blue1d+(y*width));

    /* run the Amaze */
    AMaZE_demosaic(&(amazeinfo_t){imagefloat2d,red2d,green2d,blue2d,0,0,width,height,0,0});

    for (int i = 0; i < pixelsize; i++)
    {
        size_t j = i*4;
        debayerto[ j ] = red1d[i] / 65535.0f;
        debayerto[j+1] = green1d[i] / 65535.0f;
        debayerto[j+2] = blue1d[i] / 65535.0f;
        debayerto[j+3] = 1.0f;
    }

    free(red1d);
    free(red2d);
    free(green1d);
    free(green2d);
    free(blue1d);
    free(blue2d);
    free(imagefloat2d);
}


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
    
    float * dest = PGImageGetDataPointer(img);
    float * src = PGImageGetDataPointer(input);

    size_t sz = PGImageGetWidth(img)*PGImageGetHeight(img);

    float * rawData = malloc(sizeof(float)*sz);

    for (size_t i = 0; i < sz; ++i)
    {
        rawData[i] = src[i] * 65535.0f;
    }

    debayerAmaze(dest, rawData, PGImageGetWidth(img), PGImageGetHeight(img));

    free(rawData);
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
static char * input_names[] = {"Image"};
static char * output_names[] = {"Image"};

static PGNodeSpec_t spec =
{
    .Name = "Demosaic",
    .Description = "Convert black and white raw data to colour.",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputNames = output_names,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 1,
    .InputNames = input_names,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 0,
    .NumParameters = 0,

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