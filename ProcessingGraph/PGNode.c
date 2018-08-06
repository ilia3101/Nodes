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


PGNode_t * new_PGNode(PGNodeSpec_t * Spec, PGGraph_t * Graph)
{
    MemoryBank_t * mb = new_MemoryBank(1);

    PGNode_t * node = MBMalloc(mb, sizeof(PGNode_t));
    node->memory_bank = mb;

    node->graph = Graph;
    node->spec = Spec;
    node->input_nodes = MBMalloc(mb, Spec->NumInputs * sizeof(PGNode_t *));
    for (int i = 0; i < Spec->NumInputs; ++i) node->input_nodes[i] = -1;
    node->input_node_output_indexes = MBMalloc(mb, Spec->NumInputs*sizeof(int));
    node->output_nodes = MBMalloc(mb, Spec->NumOutputs * sizeof(PGNode_t **));
    for (int i = 0; i < Spec->NumOutputs; ++i)
        node->output_nodes[i] = MBZeroAlloc(mb, sizeof(PGNode_t *));
    node->output_node_counts = MBZeroAlloc(mb, sizeof(int) * Spec->NumOutputs);
    node->parameter_states = MBZeroAlloc(mb,
                            Spec->NumParameters*sizeof(PGNodeParameterState_t));

    /* Run the node class's init function */
    Spec->Init(node);

    return node;
}

void delete_PGNode(PGNode_t * Node)
{
    PGNodeGetSpec(Node)->UnInit(Node);
    delete_MemoryBank(Node->memory_bank);
}

PGNodeOutput_t PGNodeGetOutput(PGNode_t * Node, int OutputIndex)
{
    /* TODO: add check if changed here for efficiency */
    PGNodeGetSpec(Node)->OutputFunctions[OutputIndex](Node);
    return Node->outputs[OutputIndex];
}

void PGNodeFlagChanged(PGNode_t * Node)
{
    Node->changed = 1;
    for (int i = 0; i < PGNodeGetNumOutputs(Node); ++i)
    {
        for (int j = 0; j < PGNodeGetNumNodesAtOutput(Node, i); ++j)
        {
            PGNodeFlagChanged(PGNodeGetOutputNode(Node,i,j));
        }
    }
}

void PGNodeConnect( PGNode_t * Node,
                    int NodeInputIndex,
                    PGNode_t * InputNode,
                    int InputNodeOutputIndex )
{
    /* Set node input pointer */
    Node->input_nodes[NodeInputIndex] = InputNode;
    Node->input_node_output_indexes[NodeInputIndex] = InputNodeOutputIndex;

    /* Now make it so InputNode is aware it's output of index
     * InputNodeOutputIndex goes to Node. Ao it knows which nodes to mark as
     * changed when it changes */
    int nodes = ++InputNode->output_node_counts[InputNodeOutputIndex];
    InputNode->output_nodes[InputNodeOutputIndex] =
                       MBRealloc( Node->memory_bank,
                                  InputNode->output_nodes[InputNodeOutputIndex],
                                  sizeof(PGNode_t *) * nodes );
    InputNode->output_nodes[InputNodeOutputIndex][nodes-1] = Node;
}

void PGNodeDisconnect( PGNode_t * Node,
                       int NodeInputIndex,
                       PGNode_t * InputNode,
                       int InputNodeOutputIndex )
{
    /* Disconnect on Node */
    Node->input_nodes[NodeInputIndex] = NULL;

    /* Disconnect on InputNode */
    int num = InputNode->output_node_counts[InputNodeOutputIndex];
    int index = 0;
    while(index<num&&InputNode->output_nodes[InputNodeOutputIndex][index]!=Node)
        ++index;
    memmove(InputNode->output_nodes+index, InputNode->output_nodes+index+1,
                                        (num-index) * sizeof(PGNode_t *) );
    InputNode->output_nodes[InputNodeOutputIndex] =
                       MBRealloc( Node->memory_bank,
                                  InputNode->output_nodes[InputNodeOutputIndex],
                                  sizeof(PGNode_t *) * num-1 );
}

/* TODO: FIX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
PGNode_t * PGNodeGetInputNode(PGNode_t * Node, int InputIndex)
{
    return Node->input_nodes[InputIndex];
}

PGNode_t * PGNodeGetOutputNode(PGNode_t * Node, int OutputIndex, int NodeIndex)
{
    return Node->output_nodes[OutputIndex][NodeIndex];
}

int PGNodeGetNumNodesAtOutput(PGNode_t * Node, int OutputIndex)
{
    return Node->output_node_counts[OutputIndex];
}


int PGNodeGetInputNodeOutputIndex(PGNode_t * Node, int InputIndex)
{
    return Node->input_node_output_indexes[InputIndex];
}



int PGNodeGetNumInputs(PGNode_t * Node)
{
    return PGNodeGetSpec(Node)->NumInputs;
}

int PGNodeGetNumOutputs(PGNode_t * Node)
{
    return PGNodeGetSpec(Node)->NumOutputs;
}

PGNodeSpec_t * PGNodeGetSpec(PGNode_t * Node)
{
    return Node->spec;
}