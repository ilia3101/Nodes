/* A node must be a .so or .dll file */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* This must be included */
#include "../ProcessingGraph.h"

/* From here - line 52 taken from https://github.com/ilia3101/MLV-App/blob/master/src/processing/processing.c */

int wb_kelvin[]   = {  2000,  2500,  3000,  3506,  4000,  4503,  5011,  5517,  6018,  6509,  7040,  7528,  8056,  8534,  9032,  9531, 10000 };
double wb_red[]   = { 1.134, 1.349, 1.596, 1.731, 1.806, 1.954, 2.081, 2.197, 2.291, 2.365, 2.444, 2.485, 2.528, 2.566, 2.612, 2.660, 2.702 };
double wb_green[] = { 1.155, 1.137, 1.112, 1.056, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000, 1.000 };
double wb_blue[]  = { 4.587, 3.985, 3.184, 2.524, 2.103, 1.903, 1.760, 1.641, 1.542, 1.476, 1.414, 1.390, 1.363, 1.333, 1.296, 1.263, 1.229 };

void get_kelvin_multipliers_rgb(double kelvin, double * multiplier_output)
{
    if (kelvin < 2000) kelvin = 2000;
    if (kelvin > 10000) kelvin = 10000;
    int k = 0;

    while (1 < 2)
    {
        if ( kelvin > wb_kelvin[k] && kelvin < wb_kelvin[k+1] ) 
        {
            break;
        }

        /* Doesn't need interpolation */
        else if ( kelvin == wb_kelvin[k] )
        {
            multiplier_output[0] = wb_red[k];
            multiplier_output[1] = wb_green[k];
            multiplier_output[2] = wb_blue[k];
            return;
        }

        k++;
    }

    double diff1 = (double)wb_kelvin[k+1] - (double)wb_kelvin[k];
    double diff2 = (double)wb_kelvin[k+1] - kelvin;

    /* Weight between the two */
    double weight1 = diff2 / diff1;
    double weight2 = 1.0 - weight1;

    multiplier_output[0] = (   wb_red[k] * weight1) + (   wb_red[k+1] * weight2);
    multiplier_output[1] = ( wb_green[k] * weight1) + ( wb_green[k+1] * weight2);
    multiplier_output[2] = (  wb_blue[k] * weight1) + (  wb_blue[k+1] * weight2);
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
    
    float * restrict dest = PGImageGetDataPointer(img);
    float * restrict end = dest + PGImageGetWidth(img)*PGImageGetHeight(img)*4;
    float * restrict src = PGImageGetDataPointer(input);

    /* Becoz it's stops */
    double multipliers[3];
    get_kelvin_multipliers_rgb(PGNodeGetValueParameterValue(Node, 0), multipliers);

    size_t sz = PGImageGetWidth(img)*PGImageGetHeight(img)*4;
    while (dest < end)
    {
        dest[0] = src[0] * multipliers[0];
        dest[1] = src[1] * multipliers[1];
        dest[2] = src[2] * multipliers[2];
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
static char * input_names[] = {"Image"};
static char * output_names[] = {"Image"};

static PGNodeSpec_t spec =
{
    .Name = "White Balance",
    .Description = "Adjust white balance raw",
    .Category = "Basics",

    .NumOutputs = 1,
    .OutputNames = input_names,
    .OutputTypes = {PGNodeImageOutput},

    .NumInputs = 1,
    .InputNames = output_names,
    .InputTypes = {PGNodeImageOutput},

    .HasParameters = 1,
    .NumParameters = 1,
    .Parameters = &(PGNodeParameterSpec_t){ .Name = "Kelvin",
                                            .Description = "White point",
                                            .Type = 0, /* Range */
                                            .Integers = 0, /* Continous */
                                            .LimitRange = 1,
                                            .MinValue = 2000,
                                            .MaxValue = 10000,
                                            .DefaultValue = 5600 },

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