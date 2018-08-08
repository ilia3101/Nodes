#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../librawheaders/libraw.h"
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

    /* fkjdshafkjlhsakjflhfdklajsfdhfalkjhlskdjhflkjdsahfalkjdsfhdslkjfhlkjsad */

    libraw_data_t * Raw = libraw_init(0);
    if (libraw_open_file(Raw, PGNodeGetFilePathParameterValue(Node, 0))) puts("failed to open file");
    if (libraw_unpack(Raw)) puts("failed to unpack");

    /* This is the bayer image */
    uint16_t * bayerimage = Raw->rawdata.raw_image;
    int width = libraw_get_raw_width(Raw);
    int height = libraw_get_raw_height(Raw);

    float * raw_float = malloc(width*height*3*sizeof(float));

    int shift = 6;
    for (size_t i = 0; i < width*height*3; i+=3)
    {
        raw_float[i] = bayerimage[i/3] / 65536.0;
        raw_float[i+1] = bayerimage[i/3] / 65536.0;
        raw_float[i+2] = bayerimage[i/3] / 65536.0;
    }

    libraw_recycle(Raw);
    libraw_close(Raw);

    /* sfsdfadsfasdfkjsdlkflkjhsadlkjfhkjsalhfkjasfshdlkhdsfhalkhfksalhadsfsdfas */
    
    /* Make image be right size */
    PGImageSetDimensions(img, width, height);
    printf("width: %i height: %i\n", PGImageGetWidth(img), PGImageGetHeight(img));

    float * imagedata = PGImageGetDataPointer(img);

    float * srcdata = raw_float;
    memcpy(imagedata, srcdata, width*height*sizeof(float)*3);
    free(raw_float);
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
        .Name = "File Path",
        .Description = "Path to a raw file",
        .Type = PGNodeFilePathParameter
    }
};

static void (* output_functions[])(PGNode_t *) = {&output_function};

static PGNodeSpec_t spec =
{
    .Name = "LibRaw",
    .Description = "Reads a camera raw file using LibRaw",
    .Category = "Input",

    .NumOutputs = 1,
    .OutputTypes = {PGNodeImageOutput},
    .NumInputs = 0,
    .InputTypes = NULL,

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = parameters,

    .OutputFunctions = NULL,

    .Init = init,
    .UnInit = uninit
};

PGNodeSpec_t * GetNodeSpec()
{
    spec.OutputFunctions = output_functions;
    return &spec;
}