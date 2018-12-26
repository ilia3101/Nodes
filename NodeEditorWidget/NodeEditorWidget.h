#ifndef _NodeEditorWidget_h_
#define _NodeEditorWidget_h_

/* A UILibrary compatible wadget for editing a graph */

#include "../ProcessingGraph/ProcessingGraph.h"
#include "../UILibrary/UILibrary.h"

/* Sets the graph of the NodeEditor, if NULL is passed it will become empty */
void NodeEditorSetGraph(UIFrame_t * NodeEditor, PGGraph_t * Graph);

UIFrameType_t * NodeEditorWidgetType();

#endif