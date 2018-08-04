/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>

#include "../MemoryBank/MemoryBank.h"

#include "types/PGTypes.h"
#include "ProcessingGraph.h"
#include "PGGraph.h"


PGGraph_t * new_PGGraph(char * NodeDirectory)
{
    if (!ProcessingGraphInitialised())
    {
        fprintf(stderr, "ProcessingGraph library not initialised.\n");
        return NULL;
    }

    MemoryBank_t * mb = new_MemoryBank(1);
    PGGraph_t * graph = MBMalloc(mb, sizeof(PGGraph_t));
    graph->memory_bank = mb;

    graph->num_nodes = 0;
    graph->nodes = MBMalloc(mb, sizeof(PGNode_t *));

    return graph;
}


void PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * Type)
{
    Graph->nodes = MBRealloc( Graph->memory_bank, Graph->nodes,
                              sizeof(PGNode_t *) * (Graph->num_nodes+1) );

    Graph->nodes[Graph->num_nodes] = new_PGNode(Type, Graph);

    ++Graph->num_nodes;
}


/* Gets the output of a node */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph)
{
    // PGNode_Output_t output = Node->output_functions[OutputIndex](Node);
    // return output;
    return NULL;
}