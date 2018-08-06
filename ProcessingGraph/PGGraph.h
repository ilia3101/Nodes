#ifndef _PGGraph_h_
#define _PGGraph_h_

#include "types/PGGraph_types.h"
#include "types/PGTypes.h"

/* New graph, needs node directory to function */
PGGraph_t * new_PGGraph();

/* Gets the output of a node */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph);

/* Return value is index of the node */
int PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * NodeType);

/* Get pointer to node */
PGNode_t * PGGraphGetNode(PGGraph_t * Graph, int Index);

#endif