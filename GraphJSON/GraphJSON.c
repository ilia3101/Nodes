#include <stdio.h>
#include "GraphJSON.h"

JSONBlock_t * PGGraphToJSON(PGGraph_t * Graph, char * JSONFilePath)
{
    JSONBlock_t * graph = new_EmptyJSONObject(NULL, 0);

    /********************************* FILES **********************************/

    int num_files = PGGraphGetNumFiles(Graph);
    JSONBlock_t * files = new_EmptyJSONArray(graph, num_files);

    printf("%i files\n", num_files);
    for (int f = 0; f < num_files; ++f)
    {
        JSONBlock_t * file = new_EmptyJSONObject(files, 1);
        JSONObjectSetAttributeName(file, 0, "path_absolute");
        JSONObjectSetAttributeByIndex(file, 0, new_JSONString(file, PGGraphGetFilePathByIndex(Graph, f)));
        if (JSONFilePath != NULL)
        {
            char * relative_path = PGGraphGetFilePathByIndex(Graph, f);
            JSONObjectAppendAttribute(file, new_JSONString(file, relative_path), "path_relative");
        }
        JSONArraySetElement(files, f, file);
    }

    JSONObjectAppendAttribute(graph, files, "files");

    /********************************* NODES **********************************/

    int num_nodes = PGGraphGetNumNodes(Graph);
    JSONBlock_t * nodes = new_EmptyJSONArray(graph, num_nodes);

    for (int i = 0; i < num_nodes; ++i)
    {
        PGNode_t * node = PGGraphGetNode(Graph, i);
        JSONBlock_t * json_node = new_EmptyJSONObject(nodes, 0);
        PGNodeSpec_t * spec = PGNodeGetSpec(node);

        JSONObjectAppendAttribute(json_node, new_JSONString(json_node, PGNodeGetName(node)), "name");
        JSONObjectAppendAttribute(json_node, new_JSONString(json_node, spec->Name), "type");


        /* Now get the object's inputs */
        int num_inputs = spec->NumInputs;
        if (num_inputs > 0)
        {
            // JSONBlock_t * blocks = new_EmptyJSONArray(json_node, num_inputs); /* Which index block goes to input */
            //JSONBlock_t * indexes = new_EmptyJSONArray(json_node, num_inputs); /*Which output of block goes to input*/

            // for (int i = 0; i < num_inputs; ++i)
            // {
            //     PGNode_t * input_node = PGNodeGetInputNode(node, i);
            //     JSONArraySetElement(blocks, i, new_JSONNumber(json_node, PGGraphGetNodeIndex(Graph, input_node)));
            //     JSONArraySetElement(indexes, i, new_JSONNumber(json_node, PGNodeGetInputNodeOutputIndex(node, i)));
            // }

            // JSONObjectAppendAttribute(json_node, blocks, "input_nodes");
            // JSONObjectAppendAttribute(json_node, indexes, "input_node_output_indexes");

            JSONBlock_t * inputs = new_EmptyJSONObject(json_node, num_inputs);
            for (int i = 0; i < num_inputs; ++i)
            {
                JSONObjectSetAttributeName(inputs, i, spec->InputNames[i]);
                PGNode_t * input_node = PGNodeGetInputNode(node, i);
                if (input_node != NULL)
                {
                    JSONBlock_t * input = new_EmptyJSONArray(inputs, 2);
                    // JSONObjectSetAttributeName(input, 0, "node");
                    JSONObjectSetAttributeByIndex(input, 0, new_JSONString(input, PGNodeGetName(input_node)));
                    // JSONObjectSetAttributeName(input, 1, "output");
                    int output_index = PGNodeGetInputNodeOutputIndex(node, i);
                    // JSONObjectSetAttributeByIndex(input, 1, new_JSONNull(input));
                    JSONObjectSetAttributeByIndex(input, 1, new_JSONString(json_node,
                                                  PGNodeGetSpec(input_node)->OutputNames[output_index]));
                    JSONObjectSetAttributeByIndex(inputs, i, input);
                }
                else JSONObjectSetAttributeByIndex(inputs, i, new_JSONNull(inputs));
            }
            JSONObjectAppendAttribute(json_node, inputs, "inputs");
        }


        /* Parameters Now... */
        if (spec->HasParameters)
        {
            int num_parameters = spec->NumParameters;
            JSONBlock_t * parameter_states = new_EmptyJSONObject(json_node, num_parameters);

            for (int p = 0; p < num_parameters; ++p)
            {
                PGNodeParameterSpec_t * paramspec = &spec->Parameters[p];
                JSONBlock_t * value;
                
                switch (paramspec->Type)
                {
                case PGNodeValueParameter:
                    value = new_JSONNumber(json_node, PGNodeGetValueParameterValue(node, p));
                    break;
                case PGNodeOptionsParameter:
                    value = new_JSONNumber(json_node, PGNodeGetOptionParameterValue(node, p));
                    break;
                case PGNodeBooleanParameter:
                    value = new_JSONBoolean(json_node, PGNodeGetBooleanParameterValue(node, p));
                    break;
                case PGNodeStringParameter:
                    value = new_JSONString(json_node, PGNodeGetTextParameterValue(node, p));
                    break;
                case PGNodeFileParameter:
                    value = new_JSONNumber(json_node, PGNodeGetFileParameterValue(node, p));
                    break;
                }

                JSONObjectSetAttributeName(parameter_states, p, paramspec->Name);
                JSONObjectSetAttributeByIndex(parameter_states, p, value);
            }

            JSONObjectAppendAttribute(json_node, parameter_states, "parameter_states");
        }

        JSONArraySetElement(nodes, i, json_node);
    }

    JSONObjectAppendAttribute(graph, nodes, "nodes");

    return graph;
}

PGGraph_t * JSONToPGGraph(JSONBlock_t * GraphJSON, char * JSONFilePath)
{
    PGGraph_t * graph = new_PGGraph();

    JSONBlock_t * files = JSONObjectGetAttributeByName(GraphJSON, "files");
    int num_files = JSONArrayGetLength(files);

    for (int f = 0; f < num_files; ++f)
    {
        /* TODO: add handling of relative and absolute path */
        JSONBlock_t * file = JSONArrayGetElement(files, f);
        JSONBlock_t * path = JSONObjectGetAttributeByName(file, "path_absolute");
        PGGraphAddFile(graph, JSONStringGetTextPointer(path));
    }

    JSONBlock_t * nodes = JSONObjectGetAttributeByName(GraphJSON, "nodes");
    int num_nodes = JSONArrayGetLength(nodes);

    /* Add every node */
    for (int n = 0; n < num_nodes; ++n)
    {
        JSONBlock_t * node_json = JSONArrayGetElement(nodes, n);

        int index = PGGraphAddNodeByTypeName( graph, 
                                              JSONStringGetTextPointer(JSONObjectGetAttributeByName(node_json, "type"))
                                            );

        PGNode_t * node = PGGraphGetNode(graph, index);

        /* Set name */
        PGNodeSetName(node, JSONStringGetTextPointer(JSONObjectGetAttributeByName(node_json, "name")));

        /* Now set parameters if node has them and JSON has info about them */
        PGNodeSpec_t * spec = PGNodeGetSpec(node);
        JSONBlock_t * parameters_json = JSONObjectGetAttributeByName(node_json, "parameter_states");

        if (spec->HasParameters && parameters_json != NULL)
        {
            int num_parameters = JSONObjectGetNumAttributes(parameters_json);
            for (int p = 0; p < num_parameters; ++p)
            { 
                /* Find which parameter this is (order is not guranteed to be correct as users may edit the JSON)
                 * Everything in NodeSpec is public so this is allowed. */
                JSONBlock_t * parameter = JSONObjectGetAttributeByIndex(parameters_json, p);
                PGNodeParameterSpec_t * parameter_spec = NULL;
                int parameter_index = -1;
                char * parameter_name = JSONObjectGetAttributeName(parameters_json, p);

                for (int q = 0; q < spec->NumParameters; ++q)
                {
                    PGNodeParameterSpec_t * param = &spec->Parameters[q];
                    if (!strcmp(param->Name, parameter_name))
                    {
                        parameter_spec = param;
                        parameter_index = q;
                        break;
                    }
                }

                if (parameter_spec)
                {
                    switch (parameter_spec->Type)
                    {
                    case PGNodeValueParameter:
                        PGNodeSetValueParameter(node, parameter_index, JSONNumberGetValue(parameter));
                        break;
                    case PGNodeOptionsParameter:
                        PGNodeSetOptionParameter(node, parameter_index, (int)JSONNumberGetValue(parameter));
                        break;
                    case PGNodeBooleanParameter:
                        PGNodeSetBooleanParameter(node, parameter_index, JSONBooleanGetValue(parameter));
                        break;
                    case PGNodeStringParameter:
                        PGNodeSetTextParameter(node, parameter_index, JSONStringGetTextPointer(parameter));
                        break;
                    case PGNodeFileParameter:
                        PGNodeSetFileParameter(node, parameter_index, (int)JSONNumberGetValue(parameter));
                        break;
                    }
                }
            }
        }
    }

    /* Now connect all the nodes */
    for (int n = 0; n < num_nodes; ++n)
    {
        PGNode_t * node = PGGraphGetNode(graph, n);
        PGNodeSpec_t * spec = PGNodeGetSpec(node);
        JSONBlock_t * node_json = JSONArrayGetElement(nodes, n);

        JSONBlock_t * inputs = JSONObjectGetAttributeByName(node_json, "inputs");

        if (inputs != NULL)
        {
            int num_connected_inputs = JSONObjectGetNumAttributes(inputs);
            for (int i = 0; i < num_connected_inputs; ++i)
            {
                JSONBlock_t * input = JSONObjectGetAttributeByIndex(inputs, i);
                char * input_name = JSONObjectGetAttributeName(inputs, i);
                char * input_node_name = JSONStringGetTextPointer(JSONArrayGetElement(input, 0));
                PGNode_t * input_node = PGGraphGetNodeByName(graph, input_node_name);
                char * input_node_output_name = JSONStringGetTextPointer(JSONArrayGetElement(input, 1));

                PGNodeConnectByInputOutputNames(node, input_name, input_node, input_node_output_name);
            }
        }
    }

    return graph;
}