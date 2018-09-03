#ifndef _PGGraph_types_h_
#define _PGGraph_types_h_

#include "../../MemoryBank/MemoryBank.h"

#include "PGNode_types.h"

typedef struct {
    char * path;
    int id;
} pg_file_t;

/* The processing graph */
typedef struct
{
    /* All the nodes in the graph */
    int num_nodes;
    PGNode_t ** nodes;

    /* All files used in graph */
    int num_files;
    pg_file_t * files;
    int current_file_id; /* What file id we are up to */

    /* Variables */
    char ** names;
    float * values;

    /* Source of memory */
    MemoryBank_t * memory_bank;

    /* Put any openCL stuff like contexts here ... */

} PGGraph_t;

#endif