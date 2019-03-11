/* worst code needs */
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
    /* Where the view is right now */
    double view_loc_x;
    double view_loc_y;

    /* Where view was when mouse first went down */
    double view_loc_x_mouse;
    double view_loc_y_mouse;

    /* mouse stuff */
    int mouse_is_down;
    /* Where mouse originally went down */
    double mouse_down_x;
    double mouse_down_y;
    /* Which node it went down on */
    NENode_t * mouse_down_node;
    int dragging_node; /* If it went down on a draggable area of the node */
    UICoordinate_t node_original_loc; /* Where node being moved was before */

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
    if (radius < 1) radius = 1;

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
    if (Y1 < Y2) UIImageDrawRect(Image, X1+half_len-radius, Y1-radius, radius*2, height+radius*2, Colour);
    else UIImageDrawRect(Image, X1+half_len-radius, Y2-radius, radius*2, height+radius*2, Colour);

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
    data->view_loc_x = 0;
    data->view_loc_y = 0;
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

    /* Draw all the nodes (backwards so first one is on top) */
    for (int n = data->num_nodes-1; n >= 0; --n)
    {
        NENode_t * node = data->nodes[n];

        double node_sf = ScaleFactor /* * data->zoom */;
        // node_sf = 1;

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
                             ToInteger((node->location.X+data->view_loc_x)*node_sf, int),
                             ToInteger((node->location.Y+data->view_loc_y)*node_sf, int),
                             1.0 );

        /* Get the actual node in the graph and find where it's connected to
         * to draw those connections */
        PGNode_t * actual_node = NENodeGetPGNode(node);

        for (int i = 0; i < PGNodeGetNumInputs(actual_node); ++i)
        {
            UICoordinate_t inloc = NENodeGetInputLocation(node, i);
            int in_x = ToInteger((inloc.X+node->location.X+data->view_loc_x)*node_sf, int);
            int in_y = ToInteger((inloc.Y+node->location.Y+data->view_loc_y)*node_sf, int);

            PGNode_t * input_node = PGNodeGetInputNode(actual_node, i);

            NENode_t * connect_node = NodeEditor_get_node_for_PGNode(NodeEditor, input_node);
            UICoordinate_t outloc = NENodeGetOutputLocation(connect_node, 0);
            int out_x = ToInteger((outloc.X+connect_node->location.X+data->view_loc_x)*node_sf, int);
            int out_y = ToInteger((outloc.Y+connect_node->location.Y+data->view_loc_y)*node_sf, int);

            NE_draw_node_connection( Image, in_x, in_y, out_x, out_y,
                                     node_sf*2.75, UIMakeColour(0.95,0.82,0.2,1.0) );
        }
    }
}

void NodeEditor_MouseButton( UIFrame_t * NodeEditor,
                             int MouseButton, /* MouseButton 1=Primary, 2=Middle, 3=Secondary */
                             int UpOrDown, /* Up or Down 0=Up, 1=Down */
                             double X, double Y,
                             UIRect_t Rect )
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    data->mouse_is_down = UpOrDown;

    if (UpOrDown == 1)
    {
        data->mouse_down_x = X;
        data->mouse_down_y = Y;
        data->mouse_down_node = NULL;

        /* Check if it is down on any nodes */
        for (int n = 0; n < data->num_nodes; ++n)
        {
            NENode_t * node = data->nodes[n];
            UIRect_t rect = node->dimensions;
            UICoordinate_t loc = node->location;

            /* If it goes down on the node */
            if (X > loc.X+data->view_loc_x && X < loc.X+rect.X+data->view_loc_x
                && Y > loc.Y+data->view_loc_y && Y < loc.Y+rect.Y+data->view_loc_y)
            {
                data->mouse_down_node = node;
                UIFrameMouseButton(node->interface, MouseButton, UpOrDown,
                    X-loc.X-data->view_loc_x, Y-loc.Y-data->view_loc_y, rect);
                if (UIFrameGetNeedsRedraw(node->interface)) UIFrameSetNeedsRedraw(NodeEditor);

                /* In node coordinates */
                UICoordinate_t in_node_loc = UIMakeCoordinate( X-(loc.X+data->view_loc_x),
                                                               Y-(loc.Y+data->view_loc_y) );

                /* Check if node has been clicked in draggable area */
                if (NENodeIsAreaDraggable(node, in_node_loc))
                {
                    data->dragging_node = 1;
                    data->node_original_loc = node->location;
                }
                else
                {
                    data->dragging_node = 0;
                }

                /* Make this node be first one in node array */
                for (int m = n; m >= 1; m--) data->nodes[m] = data->nodes[m-1];
                data->nodes[0] = node;
                UIFrameSetNeedsRedraw(NodeEditor);

                break;
            }
        }

        /* If the mouse did not land on a node, save current location */
        if (data->mouse_down_node == NULL)
        {
            data->view_loc_x_mouse = data->view_loc_x;
            data->view_loc_y_mouse = data->view_loc_y;
            return;
        }
    }
    else if (UpOrDown == 0)
    {
        if (data->mouse_down_node != NULL)
        {
            UIFrameMouseButton( data->mouse_down_node->interface,
                                MouseButton, UpOrDown,
                                X-data->mouse_down_node->location.X-data->view_loc_x,
                                Y-data->mouse_down_node->location.Y-data->view_loc_y,
                                data->mouse_down_node->dimensions );
            if (UIFrameGetNeedsRedraw(data->mouse_down_node->interface))
                UIFrameSetNeedsRedraw(NodeEditor);
        }
    }
}


void NodeEditor_MouseLocation(UIFrame_t * NodeEditor, double X, double Y, UIRect_t Rect)
{
    NodeEditor_data_t * data = UIFrameGetData(NodeEditor);

    /* If the mouse is down and did not land on a node, move */
    if (data->mouse_is_down && data->mouse_down_node == NULL)
    {
        /* TODO: add scaling */
        data->view_loc_x = data->view_loc_x_mouse + (X-data->mouse_down_x);
        data->view_loc_y = data->view_loc_y_mouse + (Y-data->mouse_down_y);
        UIFrameSetNeedsRedraw(NodeEditor);
        return;
    }
    /* Or if it is on a node */
    else if (data->mouse_is_down && data->mouse_down_node != NULL)
    {
        if (data->dragging_node)
        {
            data->mouse_down_node->location = UIMakeCoordinate( data->node_original_loc.X+X-data->mouse_down_x,
                                                                data->node_original_loc.Y+Y-data->mouse_down_y );
        }
        else
        {
            UIFrameSetMouseLocation( data->mouse_down_node->interface,
                                     X-data->mouse_down_node->location.X-data->view_loc_x,
                                     Y-data->mouse_down_node->location.Y-data->view_loc_y,
                                     data->mouse_down_node->dimensions );
        }
        UIFrameSetNeedsRedraw(NodeEditor);
    }
}


UIFrameType_t NodeEditor_type = {
    .Init = NodeEditor_Init,
    .UnInit = NodeEditor_UnInit,
    .Draw = NodeEditor_Draw,
    .MouseButton = NodeEditor_MouseButton,
    .MouseLocation = NodeEditor_MouseLocation,
    .MemoryNeeded = NodeEditor_MemoryNeeded
};

UIFrameType_t * NodeEditorWidgetType()
{
    return &NodeEditor_type;
}