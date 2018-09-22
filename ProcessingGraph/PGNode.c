/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#ifdef __linux__
#include <alloca.h>
#endif

#include "../MemoryBank/MemoryBank.h"

#include "PGNode.h"
#include "PGGraph.h"
#include "types/PGTypes.h"


PGNode_t * new_PGNode(PGNodeSpec_t * Spec, PGGraph_t * Graph)
{
    MemoryBank_t * mb = new_MemoryBank(1);

    PGNode_t * node = MBMalloc(mb, sizeof(PGNode_t));
    node->memory_bank = mb;

    PGNodeSetName(node, Spec->Name);

    node->graph = Graph;
    node->spec = Spec;
    node->input_nodes = MBZeroAlloc(mb, Spec->NumInputs * sizeof(PGNode_t *));
    for (int i = 0; i < Spec->NumInputs; ++i) node->input_nodes[i] == NULL;
    if (Spec->NumInputs)
        if (node->input_nodes[0] != NULL) printf("%s NOT NULL, has %i inputs\n", Spec->Name, Spec->NumInputs);
    // if (node->input_nodes == NULL) puts("malloc failes");
    node->input_node_output_indexes=MBZeroAlloc(mb,Spec->NumInputs*sizeof(int));
    node->output_nodes = MBMalloc(mb, Spec->NumOutputs * sizeof(PGNode_t **));
    for (int i = 0; i < Spec->NumOutputs; ++i)
        node->output_nodes[i] = MBZeroAlloc(mb, sizeof(PGNode_t *));
    node->output_node_counts = MBZeroAlloc(mb, sizeof(int) * Spec->NumOutputs);
    node->outputs = MBZeroAlloc(mb, sizeof(PGNodeOutput_t) * Spec->NumOutputs);
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

void PGNodeSetName(PGNode_t * Node, char * Name)
{
    strncpy(Node->name, Name, sizeof(Node->name));
}

char * PGNodeGetName(PGNode_t * Node)
{
    return Node->name;
}

PGNodeOutput_t * PGNodeGetOutput(PGNode_t * Node, int OutputIndex)
{
    /* TODO: add check if changed here for efficiency */
    PGNodeGetSpec(Node)->OutputFunctions[OutputIndex](Node);
    // printf("finished getting output from node %s\n", PGNodeGetName(Node));
    return &Node->outputs[OutputIndex];
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

void PGNodeSetGraph(PGNode_t * Node, PGGraph_t * Graph)
{
    Node->graph = Graph;
}

void PGNodeConnect( PGNode_t * Node,
                    int NodeInputIndex,
                    PGNode_t * InputNode,
                    int InputNodeOutputIndex )
{
    /* First, disconnect if connected */
    if (Node->input_nodes[NodeInputIndex] != NULL)
    {
        printf("%s input %i not NULL: %p, disconnecting\n", PGNodeGetSpec(Node)->Name, NodeInputIndex, Node->input_nodes[NodeInputIndex]);
        PGNodeDisconnect( Node,
                          NodeInputIndex,
                          Node->input_nodes[NodeInputIndex],
                          Node->input_node_output_indexes[NodeInputIndex] );
    }

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

PGNodeOutput_t * PGNodeGetInput(PGNode_t * Node, int InputIndex)
{
    return PGNodeGetOutput( PGNodeGetInputNode(Node, InputIndex),
                            PGNodeGetInputNodeOutputIndex(Node, InputIndex) );
}

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

void PGNodeSetValueParameter(PGNode_t * Node, int ParameterIndex, float Value)
{
    Node->parameter_states[ParameterIndex].value = Value;
}

void PGNodeSetOptionParameter(PGNode_t * Node, int ParameterIndex, int Option)
{
    Node->parameter_states[ParameterIndex].option = Option;
}

void PGNodeSetBooleanParameter(PGNode_t * Node, int ParameterIndex, int Value)
{
    Node->parameter_states[ParameterIndex].boolean = Value;
}

void PGNodeSetTextParameter(PGNode_t * Node, int ParameterIndex, char * Text)
{
    PGNodeParameterState_t * param = &Node->parameter_states[ParameterIndex];
    if (param->text) MBFree(Node->memory_bank, param->text);
    param->text = MBMalloc(Node->memory_bank, strlen(Text) * sizeof(char) + 1);
    strcpy(param->text, Text);
}

void PGNodeSetFileParameter(PGNode_t * Node, int ParameterIndex, int FileID)
{
    PGNodeParameterState_t * param = &Node->parameter_states[ParameterIndex];
    param->file_id = FileID;
}

float PGNodeGetValueParameterValue(PGNode_t * Node, int ParameterIndex)
{
    return Node->parameter_states[ParameterIndex].value;
}

int PGNodeGetOptionParameterValue(PGNode_t * Node, int ParameterIndex)
{
    return Node->parameter_states[ParameterIndex].option;
}

int PGNodeGetBooleanParameterValue(PGNode_t * Node, int ParameterIndex)
{
    return Node->parameter_states[ParameterIndex].boolean;
}

char * PGNodeGetTextParameterValue(PGNode_t * Node, int ParameterIndex)
{
    return Node->parameter_states[ParameterIndex].text;
}

int PGNodeGetFileParameterValue(PGNode_t * Node, int ParameterIndex)
{
    return Node->parameter_states[ParameterIndex].file_id;
}

char * PGNodeGetFileParameterPath(PGNode_t * Node, int ParameterIndex)
{
    int file_id = Node->parameter_states[ParameterIndex].file_id;
    char * path = PGGraphGetFilePath(Node->graph, file_id);
    return path;
}