#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NodeEditorWidget.h"
#include "NENode.h"

#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#define MAX(A, B) ((A) > (B)) ? (A) : (B)

/* The node editor main data */
typedef struct {

    /* Set by a method, only NodeEditor should modify it while it's here */
    PGGraph_t * graph;

    /* Node interface */
    int num_nodes;
    NENode_t ** nodes;

    /* How zoomed in the view is */
    double zoom;

} NodeEditor_data_t;

/***************************** NodeEditor methods *****************************/

/* TODO: make this not weird and bad */
void NE_draw_node_connection( UIImage_t * Image,
                              int X1, int Y1,
                              int X2, int Y2,
                              double thickness,
                              UIColour_t Colour )
{
    int radius = (int)(thickness/2.0+0.5);

    if (X1 > X2)
    {
        int temp;
        temp = X1;
        X1 = X2;
        X2 = temp;
        temp = Y1;
        Y1 = Y2;
        Y2 = temp;
    }

    int height = Y1-Y2;
    if (height < 0) height = - height;

    int half_len = (int)((X2-X1)/2.0+0.5);

    UIImageDrawRect(Image, X1, Y1-radius, half_len, radius*2, Colour);
    UIImageDrawRect(Image, X2-half_len, Y2-radius, half_len, radius*2, Colour);
    UIImageDrawRect(Image, X1+half_len-radius, MIN(Y1,Y2)-radius, radius*2, height+radius*2, Colour);

    return;
}


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
        data->nodes = malloc(num_nodes * sizeof(NENode_t *));

        for (int n = 0; n < num_nodes; ++n)
        {
            data->nodes[n] = new_NENode(PGGraphGetNode(Graph, n));
        }

        /* Now set locations */
        if (NodePositions != NULL)
        {
            for (int n = 0; n < num_nodes; ++n)
            {
                NENode_t * node = data->nodes[n];
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

NENode_t * NodeEditor_get_node_for_PGNode(UIFrame_t * NodeEditor, PGNode_t * ActualNode)
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    for (int n = 0; n < data->num_nodes; ++n)
    {
        NENode_t * node = data->nodes[n];
        if (NENodeGetPGNode(node) == ActualNode)
            return node;
    }

    return NULL;
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
    data->zoom = 1.0;
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

    UIColour_t draw_colour = UIMakeColour(0.18,0.18,0.18,1.0);

    // return;

    // UIImageDrawRect(Image, x, y, w, h, draw_colour);

    // UIColour_t border_colour = UIMakeColour(0.3,0.7,0.96,1.0);
    // double border_size = 3*ScaleFactor;
    // if (border_size < 1.0)
    //     border_colour.Alpha *= border_size;
    // int border_pix = ToInteger(border_size, int);

    // /* bottom */
    // UIImageDrawRect(Image, x, y, w, border_pix, border_colour);
    // /* top */
    // UIImageDrawRect(Image, x, y+h-border_pix, w, border_pix, border_colour);

    // /* Cut off areas already covered by top and bottom */
    // int sides_h = h - 2*border_pix;
    // int sides_y = y + border_pix;

    // /* right */
    // UIImageDrawRect(Image, x+w-border_pix, sides_y, border_pix, sides_h, border_colour);
    // /* left */
    // UIImageDrawRect(Image, x, sides_y, border_pix, sides_h, border_colour);

    /* Draw all the nodes */
    for (int n = 0; n < data->num_nodes; ++n)
    {
        NENode_t * node = data->nodes[n];

        double node_sf = ScaleFactor * data->zoom;

        /* If it was previously drawn with a different scale or zoom factor,
         * the buffer needs to be changed in size */
        if (node->scale_fac != ScaleFactor || node->zoom != data->zoom)
        {
            /* Node drawing scale factor = scale factor * zoom */
            node->image = UIImageChange( node->image,
                                         ToInteger(node->dimensions.X*node_sf, int),
                                         ToInteger(node->dimensions.Y*node_sf, int),
                                         UIImageGetPixelFormat(Image) );
        }

        /* Now node is drawn */
        UIFrameDraw( node->interface, node->image, node_sf,
                     UIMakeCoordinate(0,0), node->dimensions, 1);

        /* Put it on the main image */
        UIImageOverlayImage( Image,
                             node->image,
                             ToInteger(node->location.X*node_sf, int),
                             ToInteger(node->location.Y*node_sf, int),
                             1.0 );

        /* Get the actual node in the graph and find where it's connected to
         * to draw those connections */
        PGNode_t * actual_node = NENodeGetPGNode(node);

        for (int i = 0; i < PGNodeGetNumInputs(actual_node); ++i)
        {
            UICoordinate_t inloc = NENodeGetInputLocation(node, i);
            int in_x = ToInteger((inloc.X+node->location.X)*node_sf, int);
            int in_y = ToInteger((inloc.Y+node->location.Y)*node_sf, int);

            PGNode_t * input_node = PGNodeGetInputNode(actual_node, i);

            NENode_t * connect_node = NodeEditor_get_node_for_PGNode(NodeEditor, input_node);
            UICoordinate_t outloc = NENodeGetOutputLocation(connect_node, 0);
            int out_x = ToInteger((outloc.X+connect_node->location.X)*node_sf, int);
            int out_y = ToInteger((outloc.Y+connect_node->location.Y)*node_sf, int);

            puts("hi");

            NE_draw_node_connection( Image, in_x,
                                        in_y,
                                        out_x,
                                        out_y,
                                        3, UIMakeColour(0.95,0.82,0.2,1.0) );
        }
    }
}

// void Nodeeditor_Mouse

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