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
    puts(PGNodeGetFileParameterPath(Node, 0));
    if (libraw_open_file(Raw, PGNodeGetFileParameterPath(Node, 0))) puts("failed to open file");
    if (libraw_unpack(Raw)) puts("failed to unpack");

    /* This is the bayer data */
    uint16_t * bayerimage = Raw->rawdata.raw_image;
    int width = libraw_get_iwidth(Raw);
    int height = libraw_get_iheight(Raw);

    /* Subtract black or whatever */
    libraw_raw2image(Raw);
    libraw_subtract_black(Raw);

    uint16_t * restrict imageRAW = Raw->image;

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

    // while (pix < pixend)
    // {
    //     float value = lookup[*bayerpix] - 0.00177f;
    //     if (value <= 0) value = 0.0f;
    //     pix[0] = value;
    //     pix[1] = value;
    //     pix[2] = value;
    //     pix[3] = 1.0;
    //     pix += 4;
    //     ++bayerpix;
    // }
    for (int i = 0; i < width*height*4; i+=4)
    {
        raw_float[i] = lookup[imageRAW[i+2]] /* - 0.00177f */;
        raw_float[i+1] = lookup[imageRAW[i+1]] /* - 0.00177f */;
        raw_float[i+2] = lookup[imageRAW[i]] /* - 0.00177f */;
        if (imageRAW[i+3] != 0) raw_float[i+1] = lookup[imageRAW[i+3]];
    }

    // for (int y = 0; y < height; ++y)
    // {
    //     size_t loc = width * y * 4;
    //     for (int x = 0; x < width; ++x)
    //     {
    //         pix[loc + x * 4] = bayerimage[loc/8+x];
    //     }
    // }

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
        .Type = PGNodeFileParameter
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