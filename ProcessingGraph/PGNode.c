/* The processing graph library */

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


PGNode_t * new_PGNode(PGNodeSpec_t * Spec)
{
    MemoryBank_t * mb = new_MemoryBank(1);

    PGNode_t * node = MBMalloc(mb, sizeof(PGNode_t));
    node->memory_bank = mb;

    node->spec = Spec;
    node->parameter_states = MBMalloc(mb, Spec->NumParameters * sizeof(PGNodeParameterState_t));
    node->input_nodes = MBMalloc(mb, Spec->NumInputs * sizeof(PGNode_t *));
    node->output_nodes = MBMalloc(mb, Spec->NumOutputs * sizeof(PGNode_t **));
    for (int i = 0; i < Spec->NumOutputs; i++)
        node->output_nodes[i] = MBMalloc(mb, sizeof(PGNode_t *));
    node->output_node_counts = MBZeroAlloc(mb, sizeof(int) * Spec->NumOutputs);

    /* Run the node class's init function */
    Spec->Init(node, mb);

    return node;
}

void delete_PGNode(PGNode_t * Node)
{
    Node->spec->UnInit(Node);
    delete_MemoryBank(Node->memory_bank);
}

PGNodeOutput_t PGNodeGetOutput(PGNode_t * Node, int OutputIndex)
{
    PGNodeOutput_t output = Node->spec->OutputFunctions[OutputIndex](Node);
    return output;
}

void PGNodeFlagChanged(PGNode_t * Node)
{
    if (Node == NULL) return;
    Node->changed = 1;
    for (int i = 0; i < PGNodeGetNumOutputs(Node); ++i)
    {
        for (int j = 0; j < PGNodeGetNumNodesAtOutput(Node, i); ++j)
        {
            PGNodeFlagChanged(PGNodeGetOutputNode(Node,i,j));
        }
    }
}

int PGNodeGetNumInputs(PGNode_t * Node)
{
    return Node->spec->NumInputs;
}

PGNode_t * PGNodeGetInputNode(PGNode_t * Node, int InputIndex)
{
    return Node->input_nodes[InputIndex];
}

int PGNodeGetNumOutputs(PGNode_t * Node)
{
    return Node->spec->NumOutputs;
}

int PGNodeGetNumNodesAtOutput(PGNode_t * Node, int OutputIndex)
{
    return Node->output_node_counts[OutputIndex];
}

PGNode_t * PGNodeGetOutputNode(PGNode_t * Node, int OutputIndex, int NodeIndex)
{
    return Node->output_nodes[OutputIndex][NodeIndex];
}