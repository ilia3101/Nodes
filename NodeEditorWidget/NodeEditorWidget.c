#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NodeEditorWidget.h"

#define NodeTitleBarHeight 30

/* Each node's view for the user is represented by one of these */
typedef struct {

    /* It's position in the graph (of top left point) */
    UICoordinate_t location;
    UIRect_t dimensions;

    /* The node is like a window */
    UIImage_t * buffer; /* Stored in here to not redraw all the time */
    double zoom, scale_fac; /* The zoom and scale fac of last drawn buffer */
    UIFrame_t * interface; /* The node interface structure */

    /* The node */
    PGNode_t * node;

} NodeEditor_node_t;


/* The node editor main data */
typedef struct {

    /* Set by a method, only NodeEditor should modify it while it's here */
    PGGraph_t * graph;

    /* Node interface */
    int num_nodes;
    NodeEditor_node_t ** nodes;

} NodeEditor_data_t;

/***************************** NodeEditor methods *****************************/

void NodeEditorSetGraph( UIFrame_t * NodeEditor,
                         PGGraph_t * Graph,
                         UICoordinate_t * NodePositions )
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    /* Remove current graph, if there is one */
    if (data->graph != NULL)
    {
        /* Free each node object thing here */
        for (int n = 0; n < data->num_nodes; ++n)
            free(data->nodes[n]);
        free(data->nodes);
    }

    data->graph = Graph;

    /* Connect new graph if there is a new one */
    if (Graph != NULL)
    {
        int num_nodes = PGGraphGetNumNodes(Graph);

        data->num_nodes = num_nodes;
        data->nodes = malloc(num_nodes * sizeof(NodeEditor_node_t *));

        for (int n = 0; n < num_nodes; ++n)
        {
            data->nodes[n] = malloc(sizeof(NodeEditor_node_t));
        }

        /* Set each node's view dimensions (height/ width) */
        for (int n = 0; n < num_nodes; ++n)
        {
            /* Node height is (number of parameters + number of inputs) x 50 */
            PGNode_t * node = PGGraphGetNode(Graph, n);
            int num_parameters = PGNodeGetSpec(node)->NumParameters;
            int num_inputs = PGNodeGetNumInputs(node);
            data->nodes[n]->dimensions = UIMakeRect(200, (num_parameters+num_inputs)*50+NodeTitleBarHeight);
        }

        /* Now set locations */
        if (NodePositions != NULL)
        {
            for (int n = 0; n < num_nodes; ++n)
            {
                NodeEditor_node_t * node = data->nodes[n];
                node->location = NodePositions[n];
            }
        }
        else
        {
            for (int n = 0; n < num_nodes; ++n)
                data->nodes[n]->location = UIMakeCoordinate(0,0);
        }
    }
}

/************************** Defualt UIFrame methods ***************************/

size_t NodeEditor_MemoryNeeded()
{
    return sizeof(NodeEditor_data_t);
}

void NodeEditor_Init(UIFrame_t * NodeEditor)
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);
    data->graph = NULL;
    data->nodes = NULL;
    data->num_nodes = 0;
    return;
}

void NodeEditor_UnInit(UIFrame_t * NodeEditor)
{
    NodeEditorSetGraph(NodeEditor, NULL, NULL);
    return;
}

void NodeEditor_Draw( UIFrame_t * NodeEditor,
                      UIImage_t * Image,
                      double ScaleFactor,
                      UICoordinate_t Location,
                      UIRect_t Rect,
                      int Cleared )
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    int x = ToInteger(Location.X*ScaleFactor, int);
    int y = ToInteger(Location.Y*ScaleFactor, int);
    int w = ToInteger(Rect.X*ScaleFactor, int);
    int h = ToInteger(Rect.Y*ScaleFactor, int);

    UIColour_t draw_colour = UIMakeColour(0.2,0.2,0.2,1.0);

    // UIImageDrawRect(Image, x, y, w, h, draw_colour);

    UIColour_t border_colour = UIMakeColour(0.3,0.7,0.96,1.0);
    double border_size = 3*ScaleFactor;
    if (border_size < 1.0)
        border_colour.Alpha *= border_size;
    int border_pix = ToInteger(border_size, int);

    /* bottom */
    UIImageDrawRect(Image, x, y, w, border_pix, border_colour);
    /* top */
    UIImageDrawRect(Image, x, y+h-border_pix, w, border_pix, border_colour);

    /* Cut off areas already covered by top and bottom */
    int sides_h = h - 2*border_pix;
    int sides_y = y + border_pix;

    /* right */
    UIImageDrawRect(Image, x+w-border_pix, sides_y, border_pix, sides_h, border_colour);
    /* left */
    UIImageDrawRect(Image, x, sides_y, border_pix, sides_h, border_colour);
}

UIFrameType_t NodeEditor_type = {
    .Init = NodeEditor_Init,
    .UnInit = NodeEditor_UnInit,
    .Draw = NodeEditor_Draw,
    .MemoryNeeded = NodeEditor_MemoryNeeded
};

UIFrameType_t * NodeEditorWidgetType()
{
    return &NodeEditor_type;
}