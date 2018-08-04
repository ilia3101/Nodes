#ifndef _PGNode_types_h_
#define _PGNode_types_h_

#include "../../MemoryBank/MemoryBank.h"

/* A kind of forward declaration */
typedef struct PGNodeSpec PGNodeSpec_t;


/* This is the output type, they can be ORed to represent a node that can
 * output / accept as input different types */
typedef int PGNodeDataType_t;
#define PGNodeImageOutput 0x01
#define PGNodeColourOutput 0x02
#define PGNodeArrayOutput 0x04
#define PGNodeValueOutput 0x04
/* ... 0x08, 0x10, 0x20, 0x40, 0x80 */


typedef union {
    float value; /* Range */
    int option; /* Options */
    int state; /* Toggle (0=off,1=on) */
    char * text; /* Memory owned by object */
    char * filepath; /* Same as previous */
} PGNodeParameterState_t;

/* What a node returns as output */
typedef struct {
    PGNodeDataType_t type; /* If this is 0, this output is empty */
    union {
        PGImage_t * image;
        PGColour_t colour;
        float * array;
        float value;
    } value;
} PGNodeOutput_t;

/* The node itself */
typedef struct PGNode
{
    /* Graph which owns this node */
    /* PGGraph_t */void * graph;

    int changed; /* Nodes going to it have been changed, so re-render */


    /* Outputs and inputs are indexes of other nodes (in the graph),
     * an index value of -1 represents no connected node */

    /* Inputs */
    int * input_nodes; /* Which nodes are inputs */
    int * input_node_output_index; /* Which output of which node is used */

    /* Outputs */
    int ** output_nodes; /* One output can go to many nodes' inputs */
    int * output_node_counts; /* Number of nodes getting each output */
    PGNodeOutput_t * outputs; /* The node's outputs are here */


    /* Parameters */
    PGNodeParameterState_t * parameter_states;

    /* Node class may put any of it's own data here */
    void * own_data;

    /* The node spec */
    PGNodeSpec_t * spec;

    /* Memory things */
    MemoryBank_t * memory_bank;

} PGNode_t;



typedef struct
{
    char * Name;
    char * Description; /* NULL if u want */

    /* 0=Range, 1=Options(drop down), 2=Toggle(on/off), 3=Text, 4=FilePath */
    int Type;

    /* Range (slider) */
    int Integers; /* If this is true, it will al be in whole numbers */
    float MinValue;
    float MaxValue;
    float DefaultValue;

    /* Options */
    int NumOptions;
    char ** Options;

    /* Toggle ... (no special info required) */

    /* Text */
    int MultiLineInput;

    /* File - no info required */
} PGNodeParameterSpec_t;


/* A node is defined using a PGNodeSpec_t, plugins return a pointer to theirs */
struct PGNodeSpec
{
    /* Name of the node */
    char * Name;
    char * Description; /* NULL if u want */
    char * Category; /*  */

    int NumOutputs;
    PGNodeDataType_t * OutputTypes; /* What data type each output returns */
    int NumInputs;
    PGNodeDataType_t * InputTypes; /* What data type each input accepts */

    /* Parameters for this node (to be used for interface) */
    int HasParameters; /* If this is true, node must do its own interface */
    int NumParameters;
    PGNodeParameterSpec_t * Parameters;

    /* Array of output functions */
    void (** OutputFunctions)(PGNode_t *, PGNodeOutput_t *);

    /* "init" - called at the end of intialisation */
    void (* Init)(PGNode_t *);
    /* "UnInit" - called at end, before any memory freed */
    void (* UnInit)(PGNode_t *);

};


#endif