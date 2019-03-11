#include "../../UILibrary/UILibrary.h"
#include "../../NodeEditorWidget/NodeEditorWidget.h"
#include <pthread.h>

/* Ok the interface is based on some global variables this is temporary */

/* The program will just lock up while the graph is running and that's that */
pthread_mutex_t GraphMutex = PTHREAD_MUTEX_INITIALIZER;

double randoMm()
{
    return pow((rand()%3)/2.0, 1.5)/2.0 + 0.5;
}

typedef struct {
    UIFrame_t * main_div;
} AppWindow_t;

void run_button_func(UIFrame_t * RunButton)
{
    pthread_mutex_lock(&GraphMutex);

    // if (GraphOutputImage != NULL) delete_PGImage(GraphOutputImage);

    GraphOutputImage = PGGraphGetOutput(graph);

    uint8_t * data = PGImageGetDataPointer(GraphOutputImage);
    float * fdata = PGImageGetDataPointer(GraphOutputImage);

    for (int i = 3; i < GraphOutputImage->width*GraphOutputImage->height*4; i += 4)
    {
        // data[i] = pow(fdata[i], 0.45)*255.0+99;
        // fdata[i]+=0.25;
        fdata[i]=1;
    }
    // GraphOutputImage->width = 100;
    // GraphOutputImage->height = 100;
    // SaveImage(GraphOutputImage, "output.bmp");

    ImageHasChanged++;

    pthread_mutex_unlock(&GraphMutex);
}

UIFrame_t * new_AppWindow()
{
    UIFrame_t * main_div = new_UIFrame(UIDivType(), "Main");

    UIDivSetBackgroundColour(main_div, UIMakeColour(0.0, 0.0, 0.0, 0.0));
    // UIDivSetBackgroundColour(main_div, UIMakeColour(0.1, 0.3, 0.4, 1.0));
    UIDivSetBorderThickness(main_div, 2);

    UIFrame_t * node_graph_widget = new_UIFrame(NodeEditorWidgetType(), "node_editor");

    /* 0 px margin around all edges (full window) */
    UIFrameSetXCoordinateRelative(node_graph_widget, 0, 0);
    UIFrameSetYCoordinateRelative(node_graph_widget, 0, 0);

    NodeEditorSetGraph(node_graph_widget, graph, positions);

    UIFrameAddSubframe(main_div, node_graph_widget);

    UIFrame_t * run_button = new_UIFrame(UIButtonType(), "RunButton");
    UIButtonSetClickFunction(run_button, run_button_func);
    UIButtonSetText(run_button, "Run");
    UIFrameSetXCoordinateAbsolute(run_button, 10, 100, 1);
    UIFrameSetYCoordinateAbsolute(run_button, 10, 24, 0);

    UIFrameAddSubframe(main_div, run_button);

    return main_div;
}