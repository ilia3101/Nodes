#ifndef _PGGraph_types_h_
#define _PGGraph_types_h_

#include "../../MemoryBank/MemoryBank.h"

#include "PGNode_types.h"

/* The processing graph */
typedef struct
{
    /* All the nodes in the graph */
    int num_nodes;
    PGNode_t ** nodes;

    /* Types of available nodes */
    int num_node_types;
    PGNodeSpec_t ** node_types;

    /* Source of memory */
    MemoryBank_t * memory_bank;

    /* Put openCL stuff like contexts here ... */

} PGGraph_t;

#endif