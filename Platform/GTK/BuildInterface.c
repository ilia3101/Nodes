#include "../../UILibrary/UILibrary.h"
#include "../../NodeEditorWidget/NodeEditorWidget.h"

double randoMm()
{
    return pow((rand()%3)/2.0, 1.5)/2.0 + 0.5;
}

// typedef struct {
//     int number;
// } user_data_test_t;

int num12121212 = 0;

void button_user_func(UIFrame_t * Button)
{
    printf("\n\n\n\n\n\n\n\n\n\nBUTTON pressed: %s\n\n\n\n\n\n\n\n\n\n\n\n\n\n", UIFrameGetID(Button));
}

typedef struct {
    UIFrame_t * main_div;
} AppWindow_t;


UIFrame_t * new_AppWindow()
{
    UIFrame_t * main_div = new_UIFrame(UIDivType(), "Main");

    UIFrame_t * node_graph_widget = new_UIFrame(NodeEditorWidgetType(), "node_editor");

    /* 0 px margin around all edges (full window) */
    UIFrameSetXCoordinateRelative(node_graph_widget, 0, 0);
    UIFrameSetYCoordinateRelative(node_graph_widget, 0, 0);

    NodeEditorSetGraph(node_graph_widget, graph, positions);

    UIFrameAddSubframe(main_div, node_graph_widget);

    return main_div;
}