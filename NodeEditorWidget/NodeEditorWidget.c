#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NodeEditorWidget.h"

/* Each node's view for the user is represented by one of these */
typedef struct {

    /* Maybe will be used one day */
    int is_collapsed;

    /* It's position in the graph (top left point) */
    double loc_x, loc_y;
    double width, height;

    /* The node is like a window */
    UIImage_t * buffer; /* Image */
    double zoom, scale_fac; /* The zoom and scale fac of last drawn buffer */
    UIFrame_t * interface; /* The node interface structure */

    /* The node */
    PGNode_t * node;

} NodeEditor_node_t;


/* The node editor main data */
typedef struct {

    /* The graph set by user, which nodeeditor can modify */
    PGGraph_t * graph;

    /* Node interface */
    int num_nodes;
    NodeEditor_node_t ** nodes;

} NodeEditor_data_t;

/***************************** NodeEditor methods *****************************/

void NodeEditorSetGraph(UIFrame_t * NodeEditor, PGGraph_t * Graph)
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    /* Remove current graph */
    for (int i = 0; i < data->num_nodes; ++i)
    {
        /* Free each node object thing here */
    }

    data->graph = Graph;

    if (Graph != NULL)
    {
        /* Connect new graph if there is a new one */
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
    /* Hopefully this is called when graph has been removed */
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

    UIImageDrawRect(Image, x, y, w, h, draw_colour);

    // UIColour_t border_colour = data->border_colour;
    // double border_size = data->border_thickness*ScaleFactor;
    // if (border_size < 1.0)
    //     border_colour.Alpha *= border_size;
    // int border_pix = MAX(1, ToInteger(data->border_thickness*ScaleFactor, int));

    // /* bottom */
    // UIImageDrawRect(Image, x, y, w, border_pix, data->border_colour);
    // /* top */
    // UIImageDrawRect(Image, x, y+h-border_pix, w, border_pix, data->border_colour);

    // /* Cut of areas already covered by top and bottom */
    // int sides_h = h - 2*border_pix;
    // int sides_y = y + border_pix;

    // /* right */
    // UIImageDrawRect(Image, x+w-border_pix, sides_y, border_pix, sides_h, data->border_colour);
    // /* left */
    // UIImageDrawRect(Image, x, sides_y, border_pix, sides_h, data->border_colour);
}

UIFrameType_t NodeEditor_type = {
    .Init = NodeEditor_Init,
    .UnInit = NodeEditor_UnInit,
    .Draw = NodeEditor_Draw
};

UIFrameType_t * NodeEditorWidgetType()
{
    return &NodeEditor_type;
}