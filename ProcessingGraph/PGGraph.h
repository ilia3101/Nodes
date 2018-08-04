#ifndef _PGGraph_h_
#define _PGGraph_h_

#include "types/PGGraph_types.h"
#include "types/PGTypes.h"

/* New graph, needs node directory to function */
PGGraph_t * new_PGGraph(char * NodeDirectory);

/* Gets the output of a node */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph);

/* Will probably be one of the node types obtained from above functions */
void PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * NodeType);

#endif