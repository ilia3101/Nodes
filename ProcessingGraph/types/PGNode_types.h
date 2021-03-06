#ifndef _PGNode_types_h_
#define _PGNode_types_h_

#include "../../MemoryBank/MemoryBank.h"


/* This is the output type, they can be ORed to represent a node that can
 * output / accept as input different types, but only one should be used in an
 * actual PGNodeOutput_t return. */
typedef int PGNodeDataType_t;
#define PGNodeImageOutput 0x01
#define PGNodeColourOutput 0x02
#define PGNodeArrayOutput 0x04
#define PGNodeValueOutput 0x04
#define PGNodeNULLOutput 0x00 /* Use in PGNodeOutput_t to represent failure */
/* ... 0x08, 0x10, 0x20, 0x40, 0x80 */

typedef union {
    float value; /* Range */
    int option; /* Options */
    int boolean; /* Toggle (0=off,1=on) */
    char * text; /* Memory owned by object */
    int file_id; /* ID of the file in the graph */
} PGNodeParameterState_t;

/* What a node returns as output */
typedef struct {
    PGNodeDataType_t type; /* If this is PGNodeNULLOutput, output failed */
    union {
        PGImage_t * image;
        PGColour_t colour;
        float * array;
        float value;
    } value;
} PGNodeOutput_t;


/* A kind of forward declaration */
typedef struct PGNodeSpec PGNodeSpec_t;
typedef struct PGNode PGNode_t;


/* The node itself */
typedef struct PGNode
{
    /* This node's unique name. Hopefully no longer than 127 charachters */
    char name[128];

    /* Graph which owns this node... */
    /* PGGraph_t */void * graph;

    int changed; /* Nodes going to it have been changed, so re-render */


    /* Outputs and inputs are indexes of other nodes (in the graph),
     * an index value of -1 represents no connected node */

    /* Inputs */
    struct PGNode ** input_nodes; /* Which nodes are inputs */
    int * input_node_output_indexes; /* Which output of which node is used */

    /* Outputs */
    struct PGNode* **output_nodes; /* One output can go to many nodes' inputs */
    int * output_node_counts; /* Number of nodes getting each output */
    PGNodeOutput_t * outputs; /* The node's outputs are stored here */


    /* Parameters */
    PGNodeParameterState_t * parameter_states;

    /* Node class may put any of it's own data here */
    void * own_data;

    /* The node spec */
    PGNodeSpec_t * spec;

    /* Memory things */
    MemoryBank_t * memory_bank;

};



/* All attributes in PGNodeSpec PGNodeParamaterSpec are public (to read only) */

typedef int PGNodeParamaterType_t;
#define PGNodeValueParameter 0
#define PGNodeOptionsParameter 1
#define PGNodeBooleanParameter 2
#define PGNodeStringParameter 3
#define PGNodeFileParameter 4

typedef struct
{
    char * Name;
    char * Description; /* NULL if u want */

    PGNodeParamaterType_t Type;

    /* Value (slider) */
    int Integers; /* If this is true, it will al be in whole numbers */
    int LimitRange;
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
/* Everything in NodeSpec is public and no methods are used to access it */
struct PGNodeSpec
{
    /* Name of the node */
    char * Name;
    char * Description; /* NULL if u want */
    char * Category; /*  */

    int NumOutputs;
    char ** OutputNames;
    PGNodeDataType_t * OutputTypes; /* What data type each output returns */

    int NumInputs;
    char ** InputNames;
    PGNodeDataType_t * InputTypes; /* What data type each input accepts */

    /* Parameters for this node (to be used for interface) */
    int HasParameters; /* If this is true, node must do its own interface */
    int NumParameters;
    PGNodeParameterSpec_t * Parameters;

    /* Array of output functions */
    void (** OutputFunctions)(PGNode_t *);

    /* "init" - called at the end of intialisation */
    void (* Init)(PGNode_t *);
    /* "UnInit" - called at end, before any memory freed */
    void (* UnInit)(PGNode_t *);

};


#endif