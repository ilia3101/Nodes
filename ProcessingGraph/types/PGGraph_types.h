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

    /* Source of memory */
    MemoryBank_t * memory_bank;

    /* Put any openCL stuff like contexts here ... */

} PGGraph_t;

#endif