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

/* Name getter setter. Name should be shorter than 127 characters */
void PGNodeSetName(PGNode_t * Node, char * Name);
char * PGNodeGetName(PGNode_t * Node);

/* Getters */
PGNodeOutput_t * PGNodeGetOutput(PGNode_t * Node, int OutputIndex);
int PGNodeGetNumInputs(PGNode_t * Node);
PGNode_t * PGNodeGetInputNode(PGNode_t * Node, int InputIndex);
int PGNodeGetInputNodeOutputIndex(PGNode_t * Node, int InputIndex);
int PGNodeGetNumOutputs(PGNode_t * Node);
int PGNodeGetNumNodesAtOutput(PGNode_t * Node, int OutputIndex);
PGNode_t * PGNodeGetOutputNode(PGNode_t * Node, int OutputIndex, int NodeIndex);
PGNodeSpec_t * PGNodeGetSpec(PGNode_t * Node);
PGNodeOutput_t * PGNodeGetInput(PGNode_t * Node, int InputIndex);

/* So node knows the graph that owns it. NECESSARY (for files) */
void PGNodeSetGraph(PGNode_t * Node, PGGraph_t * Graph);

/* Connect/disconnect nodes */
void PGNodeConnect( PGNode_t * Node,
                    int NodeInputIndex,
                    PGNode_t * InputNode,
                    int InputNodeOutputIndex );
void PGNodeDisconnect( PGNode_t * Node,
                       int NodeInputIndex,
                       PGNode_t * InputNode,
                       int InputNodeOutputIndex );
void PGNodeConnectByInputOutputNames( PGNode_t * Node,
                                      char * NodeInputName,
                                      PGNode_t * InputNode,
                                      char * InputNodeOutputName );

/********************************* PARAMETERS *********************************/

/* Parameter setters */
void PGNodeSetValueParameter(PGNode_t * Node, int ParameterIndex, float Value);
void PGNodeSetOptionParameter(PGNode_t * Node, int ParameterIndex, int Option);
void PGNodeSetBooleanParameter(PGNode_t * Node, int ParameterIndex, int Value);
void PGNodeSetTextParameter(PGNode_t * Node, int ParameterIndex, char * Text);
void PGNodeSetFileParameter(PGNode_t* Node, int ParameterIndex, int FileID);
/* Parameter getters */
float PGNodeGetValueParameterValue(PGNode_t * Node, int ParameterIndex);
int PGNodeGetOptionParameterValue(PGNode_t * Node, int ParameterIndex);
int PGNodeGetBooleanParameterValue(PGNode_t * Node, int ParameterIndex);
char * PGNodeGetTextParameterValue(PGNode_t * Node, int ParameterIndex);
int PGNodeGetFileParameterValue(PGNode_t * Node, int ParameterIndex);
char * PGNodeGetFileParameterPath(PGNode_t * Node, int ParameterIndex);


#endif