#ifndef _PGNode_h_
#define _PGNode_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>

#include "../MemoryBank/MemoryBank.h"

#include "PGNode.h"
#include "types/PGTypes.h"

/* MemoryBank can be left NULL */
PGNode_t * new_PGNode(PGNodeSpec_t * Spec, PGGraph_t * Graph);
void delete_PGNode(PGNode_t * Node);

/* Called when parameters changed- flags all nodes connected to it (forwards) */
void PGNodeFlagChanged(PGNode_t * Node);

PGNodeOutput_t PGNodeGetOutput(PGNode_t * Node, int OutputIndex);
int PGNodeGetNumInputs(PGNode_t * Node);
PGNode_t * PGNodeGetInputNode(PGNode_t * Node, int InputIndex);
int PGNodeGetInputNodeOutputIndex(PGNode_t * Node, int InputIndex);
int PGNodeGetNumOutputs(PGNode_t * Node);
int PGNodeGetNumNodesAtOutput(PGNode_t * Node, int OutputIndex);
PGNode_t * PGNodeGetOutputNode(PGNode_t * Node, int OutputIndex, int NodeIndex);

#endif