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
    clock_t begin = clock();

    libraw_data_t * Raw = libraw_init(0);
    if (libraw_open_file(Raw, PGNodeGetFilePathParameterValue(Node, 0))) puts("failed to open file");
    if (libraw_unpack(Raw)) puts("failed to unpack");

    /* This is the bayer data */
    uint16_t * bayerimage = Raw->rawdata.raw_image;
    int width = libraw_get_raw_width(Raw);
    int height = libraw_get_raw_height(Raw);

    /* Subtract black or whatever */
    libraw_subtract_black(Raw);

    /* Make image be right size */
    PGImageSetDimensions(img, width, height);
    printf("width: %i height: %i\n", PGImageGetWidth(img), PGImageGetHeight(img));
    float * raw_float = PGImageGetDataPointer(img);


    /* Look up table int 0-65535 -> float 0.0-1.0 */
    float lookup[65536];
    for (int i = 0; i < 65536; ++i) lookup[i] = ((float)i) / 65535.0;

    /* transter to float */
    uint16_t * restrict bayerpix = bayerimage;
    float * restrict pix = raw_float;
    float * restrict pixend = raw_float + width*height*4;

    while (pix < pixend)
    {
        float value = lookup[*bayerpix];
        pix[0] = value;
        pix[1] = value;
        pix[2] = value;
        pix[3] = 1.0;
        pix += 4;
        ++bayerpix;
    }

    libraw_recycle(Raw);
    libraw_close(Raw);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0;
    printf("time spent reading raw file: %f\n", time_spent);
    /* sfsdfadsfasdfkjsdlkflkjhsadlkjfhkjsalhfkjasfshdlkhdsfhalkhfksalhadsfsdfas */
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
        .Name = "Raw File",
        .Description = "A raw file to read",
        .Type = PGNodeFilePathParameter
    }
};

static void (* output_functions[])(PGNode_t *) = {&output_function};

static PGNodeSpec_t spec =
{
    .Name = "LibRaw",
    .Description = "Reads a camera raw file using LibRaw",
    .Category = "Input",

    .NumOutputs = 3,
    .OutputTypes = {PGNodeImageOutput, PGNodeValueOutput, PGNodeValueOutput},
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