#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ProcessingGraph/ProcessingGraph.h"
#include "../UILibrary/UILibrary.h"

#include "NENode.h"


#define NodeTitleBarHeight 30
#define NodeUnitHeight 50

/* All updating done in here */
void NENode_update_interface(NENode_t * Node)
{
    PGNode_t * actual_node = NENodeGetPGNode(Node);
    PGNodeSpec_t * spec = PGNodeGetSpec(actual_node);

    for (int p = 0; p < spec->NumParameters; ++p)
    {
        switch (spec->Parameters[p].Type)
        {
        case PGNodeFileParameter:;
            char string[1000];
            char * filename = PGNodeGetFileParameterPath(Node->node, p);
            filename = filename + strlen(filename) - 1;
            while(filename[-1]!='/') filename--;
            sprintf(string, "%s: %s", spec->Parameters[p].Name, filename);
            UILabelSetText(UIFrameGetSubframeByID(Node->parameters[p], "label"), string);
            break;
        case PGNodeValueParameter:;
            double value = UISliderGetPosition(UIFrameGetSubframeByID(Node->parameters[p], "slider")) *
                            (spec->Parameters[p].MaxValue - spec->Parameters[p].MinValue) + spec->Parameters[p].MinValue;
            PGNodeSetValueParameter(Node->node, p, value);
            sprintf(string, "%s: %+.2f", spec->Parameters[p].Name, PGNodeGetValueParameterValue(Node->node, p));
            UILabelSetText(UIFrameGetSubframeByID(Node->parameters[p], "label"), string);
            // UISliderSetPosition(UIFrameGetSubframeByID(Node->parameters[p], "slider"),
            //                   (PGNodeGetValueParameterValue(Node->node, p)- spec->Parameters[p].MinValue) /
            //                   (spec->Parameters[p].MaxValue - spec->Parameters[p].MinValue) );
            break;
        }
    }
    return;
}

void NENode_value_slider_function(UIFrame_t * Slider)
{
    /* As the user data is just set to the node */
    NENode_update_interface(UIFrameGetUserData(Slider));
}

/* Create UIFrame interface for a node type */
void NENode_create_interface(NENode_t * Node)
{
    PGNode_t * actual_node = NENodeGetPGNode(Node);
    PGNodeSpec_t * spec = PGNodeGetSpec(actual_node);

    int num_units = spec->NumInputs + spec->NumParameters + spec->NumOutputs;

    double border_thickness = 1.7;

    UIFrame_t * main = new_UIFrame(UIDivType(), "Node");
    UIFrameSetXCoordinateAbsolute(main, 0, 0, 0);
    UIFrameSetYCoordinateAbsolute(main, 0, NodeUnitHeight*num_units+20, 0);
    UIDivSetBorderColour(main, UIMakeColour(0.9,0.6,0.3,1.0));
    UIDivSetBorderThickness(main, border_thickness);
    UIDivSetBackgroundColour(main, UIMakeColour(0.3,0.3,0.3,1.0));

    UIFrame_t ** units = alloca(num_units * sizeof(UIFrame_t *));

    /* Create all the units */
    for (int u = 0; u < num_units; ++u)
    {
        UIFrame_t * unit = new_UIFrame(UIDivType(), "unit");
        UIDivSetBackgroundColour(unit, UIMakeColour(0.2,0.2,0.2,1.0));
        UIDivSetBorderColour(unit, UIMakeColour(0.9,0.6,0.3,1.0));
        UIDivSetBorderThickness(unit, border_thickness);
        UIFrameSetXCoordinateRelative(unit, 0, 0);
        UIFrameSetYCoordinateAbsolute(unit, NodeUnitHeight*u+NodeTitleBarHeight-border_thickness*2, NodeUnitHeight, 1);
        UIFrameAddSubframe(main, unit);
        units[u] = unit;
    }

    /* Add node type label */
    UIFrame_t * name_label = new_UIFrame(UILabelType(), "node name");
    UIFrameSetXCoordinateRelative(name_label, 0, 0);
    UIFrameSetYCoordinateAbsolute(name_label, 0, 26, 1);
    UIFrameAddSubframe(main, name_label);
    UILabelSetTextColour(name_label, UIMakeColour(0.8,0.8,0.8,1.0));
    UILabelSetText(name_label, spec->Name);

    /* Add outputs/inputs/parameters, outputs at top, inputs bottom */

    /* Add outputs */
    for (int o = 0; o < spec->NumOutputs; ++o)
    {
        UIFrame_t * unit = units[o];

        UIFrame_t * label = new_UIFrame(UILabelType(), "label");
        UILabelSetTextColour(label, UIMakeColour(0.8,0.8,0.8,1.0));
        UIFrameSetXCoordinateRelative(label, 2, 2);
        UIFrameSetYCoordinateRelative(label, 2, 2);
        UILabelSetText(label, spec->OutputNames[o]);
        UIFrameAddSubframe(unit, label);

        /* Add area to connect to */
        UIFrame_t * connection = new_UIFrame(UIDivType(), "connection");
        UIDivSetBackgroundColour(connection, UIMakeColour(0.9,0.82,0.2,1.0));
        UIDivSetBorderColour(connection, UIMakeColour(0.8,0.25,0.1,1.0));
        UIDivSetBorderThickness(connection, 2);
        UIFrameSetXCoordinateAbsolute(connection, 3, 20, 1);
        UIFrameSetYCoordinateRelative(connection, 9, 9);
        UIFrameAddSubframe(unit,connection);

        /* Outputs are red */
        UIDivSetBorderColour(unit, UIMakeColour(0.8,0.3,0.3,1.0));

        Node->outputs[o] = unit;
    }

    /* Add parameters */
    for (int p = 0; p < spec->NumParameters; ++p)
    {
        UIFrame_t * unit = units[spec->NumOutputs+p];
        UIFrame_t * label = new_UIFrame(UILabelType(), "label");
        UILabelSetTextColour(label, UIMakeColour(0.8,0.8,0.8,1.0));
        UIFrameSetXCoordinateRelative(label, 2, 2);
        UIFrameSetYCoordinateRelative(label, 2, 2);
        UILabelSetText(label, spec->Parameters[p].Name);
        UIFrameAddSubframe(unit, label);
        /* Parameters are green */
        UIDivSetBorderColour(unit, UIMakeColour(0.22,0.6,0.22,1.0));

        switch (spec->Parameters[p].Type)
        {
            case PGNodeValueParameter:
                UIFrameSetXCoordinateRelative(label, 2, 2);
                UIFrameSetYCoordinateRelative(label, 21.6, 0);
                UIFrame_t * slider = new_UIFrame(UISliderType(), "slider");
                UISliderSetKnobDimensions(slider, 10, 13.5);
                UIFrameSetXCoordinateRelative(slider, 8, 8);
                UIFrameSetYCoordinateRelative(slider, 0, 19);
                UIFrameAddSubframe(unit, slider);
                UIFrameSetUserData(slider, Node);
                UISliderSetOnChangeFunction(slider, NENode_value_slider_function);
                // UIFrameAddSubframe
                break;
        }

        Node->parameters[p] = unit;
    }

    /* Add inputs */
    for (int i = 0; i < spec->NumInputs; ++i)
    {
        UIFrame_t * unit = units[spec->NumOutputs+spec->NumParameters+i];

        UIFrame_t * label = new_UIFrame(UILabelType(), "label");
        UILabelSetTextColour(label, UIMakeColour(0.8,0.8,0.8,1.0));
        UIFrameSetXCoordinateRelative(label, 2, 2);
        UIFrameSetYCoordinateRelative(label, 2, 2);
        UILabelSetText(label, spec->InputNames[i]);
        UIFrameAddSubframe(unit, label);

        /* Add area to connect to */
        UIFrame_t * connection = new_UIFrame(UIDivType(), "connection");
        UIDivSetBackgroundColour(connection, UIMakeColour(0.9,0.82,0.2,1.0));
        UIDivSetBorderColour(connection, UIMakeColour(0.8,0.25,0.1,1.0));
        UIDivSetBorderThickness(connection, 2);
        UIFrameSetXCoordinateAbsolute(connection, 3, 20, 0);
        UIFrameSetYCoordinateRelative(connection, 9, 9);
        UIFrameAddSubframe(unit,connection);
        Node->inputs[i] = unit;

        /* Inputs are blue */
        UIDivSetBorderColour(units[spec->NumOutputs+spec->NumParameters+i], UIMakeColour(0.3,0.6,0.9,1.0));
    }

    Node->interface = main;
}

/* inverse of previous lol */
void NENode_delete_node_interface(NENode_t * Node)
{
    // delete_UIFrame(Node->interface);
    free(Node->inputs);
    free(Node->outputs);
    free(Node->parameters);
    /* TODO: proper deletion of everything */
}

NENode_t * new_NENode(PGNode_t * ActualNode)
{
    NENode_t * node = malloc(sizeof(NENode_t));

    PGNodeSpec_t * spec = PGNodeGetSpec(ActualNode);

    /* On the left (inputs) */
    int num_parameters = spec->NumParameters;
    int num_inputs = spec->NumInputs;
    int num_outputs = spec->NumOutputs;

    /* How many units high the node is */
    int num_units = num_inputs + num_parameters + num_outputs;
    node->dimensions = UIMakeRect(220, num_units*NodeUnitHeight+NodeTitleBarHeight);

    /* Create image */
    node->node = ActualNode;
    node->image = new_UIImage(10,10,Pixels_RGB_24i);
    node->scale_fac = -1;
    node->zoom = -1;

    /* Arrays of parameter and stuff */
    node->inputs = malloc(spec->NumInputs * sizeof(UIFrame_t *));
    node->parameters = malloc(spec->NumParameters * sizeof(UIFrame_t *));
    node->outputs = malloc(spec->NumOutputs * sizeof(UIFrame_t *));

    NENode_create_interface(node);
    NENode_update_interface(node);

    return node;
}

void delete_NENode(NENode_t * Node)
{
    NENode_delete_node_interface(Node->interface);
    delete_UIImage(Node->image);
    free(Node->inputs);
    free(Node->parameters);
    free(Node->outputs);
    free(Node);
}

PGNode_t * NENodeGetPGNode(NENode_t * Node)
{
    return Node->node;
}

/* TODO: redo this better (something that wont break) */
UICoordinate_t NENodeGetOutputLocation(NENode_t * Node, int OutputIndex)
{
    UIFrame_t * output = Node->outputs[OutputIndex];

    UIRect_t parent_rect = UIFrameGetRect(Node->interface, UIMakeRect(0,0));

    UICoordinate_t coord = UIFrameGetCoordinate(output, parent_rect);
    UIRect_t rect = UIFrameGetRect(output, parent_rect);

    return UIMakeCoordinate(coord.X+220, coord.Y+rect.Y/1.5);
}

/* TODO: redo this better */
UICoordinate_t NENodeGetInputLocation(NENode_t * Node, int InputIndex)
{
    UIFrame_t * input = Node->inputs[InputIndex];

    UIRect_t parent_rect = UIFrameGetRect(Node->interface, UIMakeRect(0,0));

    UICoordinate_t coord = UIFrameGetCoordinate(input, parent_rect);
    UIRect_t rect = UIFrameGetRect(input, parent_rect);

    return UIMakeCoordinate(coord.X, coord.Y+rect.Y/1.5);
}

int NENodeIsAreaDraggable(NENode_t * Node, UICoordinate_t Coord)
{
    if (Coord.Y > Node->dimensions.Y-NodeTitleBarHeight && Coord.Y < Node->dimensions.Y)
        return 1;
    else
        return 0;
}
