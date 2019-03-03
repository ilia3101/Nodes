#ifndef _NENode_h_
#define _NENode_h_

/* Node editor widget's node class */

/* Each node's view for the user is represented by one of these */
typedef struct {

    /* It's position in the graph (of top left point) */
    UICoordinate_t location;
    UIRect_t dimensions;

    /* The node is like a window */
    UIImage_t * image; /* Stored in here to not redraw all the time */
    double zoom, scale_fac; /* The zoom and scale fac of last drawn buffer */
    UIFrame_t * interface; /* The node interface structure */
    UIFrame_t ** inputs; /* Pointers to all inputs (inside interface) */
    UIFrame_t ** parameters; /* Pointers to all parameters (inside interface) */
    UIFrame_t ** outputs; /* Pointers to all outputs (inside interface) */

    /* The node */
    PGNode_t * node;

} NENode_t;

NENode_t * new_NENode(PGNode_t * ActualNode);
void delete_NENode(NENode_t * Node);

PGNode_t * NENodeGetPGNode(NENode_t * Node);

UICoordinate_t NENodeGetOutputLocation(NENode_t * Node, int OutputIndex);
UICoordinate_t NENodeGetInputLocation(NENode_t * Node, int InputIndex);

#endif