#ifndef _NodeEditorWidget_h_
#define _NodeEditorWidget_h_

/* A UILibrary compatible wadget for editing a graph */

#include "../ProcessingGraph/ProcessingGraph.h"
#include "../UILibrary/UILibrary.h"

/* Sets the graph of the NodeEditor, if Graph is NULL it will become empty,
 * if Locations array is NULL, node locations will be decided automatically */
void NodeEditorSetGraph( UIFrame_t * NodeEditor,
                         PGGraph_t * Graph,
                         UICoordinate_t * Locations );

UIFrameType_t * NodeEditorWidgetType();

#endif