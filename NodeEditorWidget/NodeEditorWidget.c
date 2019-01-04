#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NodeEditorWidget.h"

#define NodeTitleBarHeight 30
#define NodeUnitHeight 50

#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#define MAX(A, B) ((A) > (B)) ? (A) : (B)

/* Each node's view for the user is represented by one of these */
typedef struct {

    /* It's position in the graph (of top left point) */
    UICoordinate_t location;
    UIRect_t dimensions;

    /* The node is like a window */
    UIImage_t * image; /* Stored in here to not redraw all the time */
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

    /* How zoomed in the view is */
    double zoom;

} NodeEditor_data_t;

/***************************** NodeEditor methods *****************************/

/* Create UIFrame interface for a node type */
UIFrame_t * NE_create_interface_for_node(PGNodeSpec_t * Spec)
{
    UIFrame_t * main = new_UIFrame(UIDivType(), "Node");

    // int num_units = Spec->NumInputs + Spec->NumParameters + Spec->NumOutputs;

    // UIFrame_t ** units = alloca(num_units * sizeof(UIFrame_t *));

    // /* Create all the units */
    // for (int u = 0; u < num_units; ++u)
    // {
    //     UIFrame_t * unit = new_UIFrame(UIDivType(), "unit");
    //     UIDivSetBackgroundColour(unit, UIMakeColour(0.3,0.3,0.3,1.0));
    //     UIDivSetBorderColour(unit, UIMakeColour(0.6,0.6,0.6,1.0));
    //     UIDivSetBorderThickness(unit, 1.0);
    //     // UIFrameAddSubframe(main, unit);
    //     units[u] = unit;
    // }

    // /* Part 2 */

    // /* Add outputs */
    // for (int o = 0; o < Spec->NumOutputs; ++o)
    // {
    //     UIFrame_t * label = new_UIFrame(UILabelType(), "label");
    //     UIFrameSetXCoordinateRelative(label, 2, 2);
    //     UIFrameSetYCoordinateRelative(label, 2, 2);
    //     UILabelSetText(label, Spec->OutputNames[o]);
    //     UIFrameAddSubframe(units)
    // }

    // /* Add parameters */
    // for (int p = 0; p < Spec->NumParameters; ++p)
    // {
    //     UIFrame_t * label = new_UIFrame(UILabelType(), "label");
    //     UIFrameSetXCoordinateRelative(label, 2, 2);
    //     UIFrameSetYCoordinateRelative(label, 2, 2);
    //     UILabelSetText(label, Spec->Parameters[p].Name);
    // }

    // /* Add outputs */

    return main;
}

/* inverse of previous lol */
void NE_delete_node_interface(UIFrame_t * Interface)
{
    free(Interface);
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
        data->nodes = malloc(num_nodes * sizeof(NodeEditor_node_t *));

        for (int n = 0; n < num_nodes; ++n)
        {
            data->nodes[n] = malloc(sizeof(NodeEditor_node_t));
        }

        /* Set each node's view dimensions and create interface */
        for (int n = 0; n < num_nodes; ++n)
        {
            /* Node height is (number of parameters + number of inputs) x 50 */
            PGNode_t * node = PGGraphGetNode(Graph, n);

            /* On the left (inputs) */
            int num_parameters = PGNodeGetSpec(node)->NumParameters;
            int num_inputs = PGNodeGetNumInputs(node);

            /* On the right (outputs) */
            int num_outputs = PGNodeGetNumOutputs(node);

            NodeEditor_node_t * view_node = data->nodes[n];

            /* How many units high the node is */
            int num_units = num_inputs + num_parameters + num_outputs;
            view_node->dimensions = UIMakeRect(200, num_units*NodeUnitHeight+NodeTitleBarHeight);

            /* Create image */
            view_node->interface = NE_create_interface_for_node(PGNodeGetSpec(node));
            view_node->image = new_UIImage(10,10,Pixels_RGB_24i);
            view_node->scale_fac = -1;
            view_node->zoom = -1;
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

    /* Draw all the nodes */
    for (int n = 0; n < data->num_nodes-3; ++n)
    {
        NodeEditor_node_t * node = data->nodes[n];

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
        puts("Drawn!!!");

        /* Put it on the main image */
        // UIImageOverlayImage( Image,
        //                      node->image,
        //                      ToInteger(node->location.X*node_sf, int),
        //                      ToInteger(node->location.Y*node_sf, int),
        //                      1.0 );

        UIImageDrawRect( Image,
                         ToInteger(node->location.X*node_sf, int),
                         ToInteger(node->location.Y*node_sf, int),
                         ToInteger(node->dimensions.X*node_sf, int),
                         ToInteger(node->dimensions.Y*node_sf, int),
                         UIMakeColour(1.0,0.5,0.5,1.0));
    }
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