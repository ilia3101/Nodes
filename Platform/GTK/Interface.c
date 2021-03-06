#include "../../UILibrary/UILibrary.h"
#include "../../NodeEditorWidget/NodeEditorWidget.h"
#include <pthread.h>

/* Ok the interface is based on global variables this is temporary */

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

    GraphOutputImage = PGGraphGetOutput(graph);

    uint8_t * data = PGImageGetDataPointer(GraphOutputImage);
    float * fdata = PGImageGetDataPointer(GraphOutputImage);

    for (int i = 3; i < GraphOutputImage->width*GraphOutputImage->height*4; i += 4)
    {
        fdata[i]=1;
    }

    ImageHasChanged++;

    pthread_mutex_unlock(&GraphMutex);
}

void add_node_button_func(UIFrame_t * AddNodeButton)
{
    UIFrame_t * node_editor = UIFrameGetSubframeByID(mainDiv, "node_editor");

    puts(UIButtonGetText(AddNodeButton));

    NodeEditorAddNode(node_editor, UIButtonGetText(AddNodeButton));
}

void open_button_func(UIFrame_t * SaveButton)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // scalefac+=0.1;
    // gtk_widget_queue_draw(main_window);
    // return;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           main_window,
                                           action,
                                           "_Cancel",
                                           GTK_RESPONSE_CANCEL,
                                           "_Open",
                                           GTK_RESPONSE_ACCEPT,
                                           NULL );

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);


        FILE * jsonfile = fopen(filename, "r");
        char * text = calloc(100000, 1); /* Enough for some text */
        fseek(jsonfile, 0, SEEK_END);
        int szz = ftell(jsonfile);
        fseek(jsonfile, 0, SEEK_SET);
        fread(text, szz, 1, jsonfile);

        JSONBlock_t * json_parsed = ParseJSON(text);

        graph = JSONToPGGraph(json_parsed, filename);

        UIFrame_t * node_editor = UIFrameGetSubframeByID(mainDiv, "node_editor");

        NodeEditorSetGraph(node_editor, graph, NULL);
    }

    gtk_widget_destroy (dialog);
}

#define ASDFGHJKL // remove some time

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

    /* Button that runs the program */
    UIFrame_t * run_button = new_UIFrame(UIButtonType(), "run_button");
    UIButtonSetClickFunction(run_button, run_button_func);
    UIButtonSetText(run_button, "Run");
    UIFrameSetXCoordinateAbsolute(run_button, 10, 100, 1);
    UIFrameSetYCoordinateAbsolute(run_button, 10, 24, 0);
    UIFrameAddSubframe(main_div, run_button);

    UIFrame_t * open_button = new_UIFrame(UIButtonType(), "open_button");
    UIButtonSetText(open_button, "Open");
    UIFrameSetXCoordinateAbsolute(open_button, 10, 100, 1);
    UIFrameSetYCoordinateAbsolute(open_button, 10, 24, 1);
    UIButtonSetClickFunction(open_button, open_button_func);
    UIFrameAddSubframe(main_div, open_button);
    UIFrame_t * save_button = new_UIFrame(UIButtonType(), "save_button");
    UIButtonSetText(save_button, "Save");
    UIFrameSetXCoordinateAbsolute(save_button, 10, 100, 1);
    UIFrameSetYCoordinateAbsolute(save_button, 39, 24, 1);
    UIFrameAddSubframe(main_div, save_button);

    /* Create node adding menu */
    PGNodeSpec_t ** node_types = ProcessingGraphGetNodeTypes();
    int num_node_types = ProcessingGraphGetNumNodeTypes();

#ifdef ASDFGHJKL
    char * names[] = {"saturation", "contrast"};
#endif

    UIFrame_t * add_node_section = new_UIFrame(UIDivType(), "add_node_section");
    UIFrameSetXCoordinateAbsolute(add_node_section, 15, 210, 0);
#ifdef ASDFGHJKL
    UIFrameSetYCoordinateAbsolute(add_node_section, 15, 30+(num_node_types+(sizeof(names)/sizeof(names[1])))*26, 1);
#else
    UIFrameSetYCoordinateAbsolute(add_node_section, 15, 30+num_node_types*26, 1);
#endif
    UIDivSetBackgroundColour(add_node_section, UIMakeColour(0.2,0.2,0.2,0.7));
    UIFrame_t * add_node_label = new_UIFrame(UILabelType(), "add_node_label");
    UILabelSetText(add_node_label, "Add node");
    UIFrameSetXCoordinateAbsolute(add_node_label, 5, 200, 0);
    UIFrameSetYCoordinateAbsolute(add_node_label, 2, 22, 1);
    UILabelSetTextColour(add_node_label, UIMakeColour(0.9,0.9,0.9,1));
    UIFrameAddSubframe(add_node_section, add_node_label);
#ifdef ASDFGHJKL
    for (int i = 0; i < num_node_types+(sizeof(names)/sizeof(names[1])); ++i)
#else
    for (int i = 0; i < num_node_types; ++i)
#endif
    {
        UIFrame_t * button = new_UIFrame(UIButtonType(), "add_node_button");
#ifdef ASDFGHJKL
        if (i >= num_node_types)
            UIButtonSetText(button, names[i-num_node_types]);
        else
#endif
        UIButtonSetText(button, node_types[i]->Name);
        UIFrameSetXCoordinateAbsolute(button, 5, 200, 0);
        UIFrameSetYCoordinateAbsolute(button, 25+i*26, 24, 1);
        UIButtonSetClickFunction(button, add_node_button_func);
        UIButtonSetBackgroundColour(button, UIMakeColour(0.3,0.3,0.3,0.5));
        UIButtonSetBackgroundMouseOverColour(button, UIMakeColour(0.2,0.2,0.2,0.7));
        UIButtonSetBackgroundMouseDownColour(button, UIMakeColour(0.3,0.3,0.3,0.5));
        UIButtonSetTextColour(button, UIMakeColour(0.9,0.9,0.9,1));
        UIFrameAddSubframe(add_node_section, button);
    }
    UIFrameAddSubframe(main_div, add_node_section);

    return main_div;
}