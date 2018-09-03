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
        JSONBlock_t * file = new_EmptyJSONObject(files, 2);
        JSONObjectSetAttributeName(file, 0, "path_absolute");
        JSONObjectSetAttributeByIndex(file, 0, new_JSONString(file, PGGraphGetFilePathByIndex(Graph, f)));
        /* Path relative to JSONFilePath */
        JSONObjectSetAttributeName(file, 1, "path_relative");
        JSONObjectSetAttributeByIndex(file, 1, new_JSONString(file, PGGraphGetFilePathByIndex(Graph, f)));
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

        JSONObjectAppendAttribute(json_node, new_JSONString(json_node, spec->Name), "type");


        /* Now get the object's inputs */
        int num_inputs = spec->NumInputs;
        if (num_inputs > 0)
        {
            JSONBlock_t * blocks = new_EmptyJSONArray(json_node, num_inputs); /* Which index block goes to input */
            JSONBlock_t * indexes = new_EmptyJSONArray(json_node, num_inputs); /* Which output of block goes to input */

            for (int i = 0; i < num_inputs; ++i)
            {
                PGNode_t * input_node = PGNodeGetInputNode(node, i);
                JSONArraySetElement(blocks, i, new_JSONNumber(json_node, PGGraphGetNodeIndex(Graph, input_node)));
                JSONArraySetElement(indexes, i, new_JSONNumber(json_node, PGNodeGetInputNodeOutputIndex(node, i)));
            }

            JSONObjectAppendAttribute(json_node, blocks, "input_nodes");
            JSONObjectAppendAttribute(json_node, indexes, "input_node_output_indexes");
        }


        /* Parameters Now... */
        if (spec->HasParameters)
        {
            int num_parameters = spec->NumParameters;
            JSONBlock_t * parameter_states = new_EmptyJSONObject(json_node, num_parameters);

            for (int p = 0; p < num_parameters; ++p)
            {
                PGNodeParameterSpec_t * paramspec = &spec->Parameters[p];
                PGNodeParameterState_t * param = &node->parameter_states[p];
                JSONBlock_t * value;
                
                switch (paramspec->Type)
                {
                case PGNodeValueParameter:
                    value = new_JSONNumber(json_node, param->value);
                    break;
                case PGNodeOptionsParameter:
                    value = new_JSONNumber(json_node, param->option);
                    break;
                case PGNodeBooleanParameter:
                    value = new_JSONBoolean(json_node, param->boolean);
                    break;
                case PGNodeStringParameter:
                    value = new_JSONString(json_node, param->text);
                    break;
                case PGNodeFilePathParameter:
                    value = new_JSONString(json_node, param->filepath);
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
    JSONBlock_t * nodes = JSONObjectGetAttributeByName(GraphJSON, "nodes");
    int num_nodes = JSONArrayGetLength(nodes);

    PGGraph_t * graph = new_PGGraph();

    /* Add every node */
    for (int n = 0; n < num_nodes; ++n)
    {
        JSONBlock_t * node_json = JSONArrayGetElement(nodes, n);

        int index = PGGraphAddNodeByTypeName( graph, 
                                              JSONStringGetTextPointer(JSONObjectGetAttributeByName(node_json, "type"))
                                            );

        PGNode_t * node = PGGraphGetNode(graph, index);

        PGNodeSpec_t * spec = PGNodeGetSpec(node);
        JSONBlock_t * parameters_json = JSONObjectGetAttributeByName(node_json, "parameter_states");

        /* Now set parameters if node has them and JSON has info about them */
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
                    case PGNodeFilePathParameter:
                        PGNodeSetFilePathParameter(node, parameter_index, JSONStringGetTextPointer(parameter));
                        break;
                    }
                }
            }
        }
    }

    /* Now set inputs up */
    for (int n = 0; n < num_nodes; ++n)
    {
        // puts("Node");
        PGNode_t * node = PGGraphGetNode(graph, n);
        PGNodeSpec_t * spec = PGNodeGetSpec(node);
        // puts(spec->Name);
        JSONBlock_t * node_json = JSONArrayGetElement(nodes, n);

        JSONBlock_t * input_nodes = JSONObjectGetAttributeByName(node_json, "input_nodes");
        JSONBlock_t * input_node_output_indexes = JSONObjectGetAttributeByName(node_json, "input_node_output_indexes");

        if ( input_nodes != NULL && input_node_output_indexes != NULL &&
             JSONArrayGetLength(input_nodes) == spec->NumInputs )
        {
            for (int i = 0; i < spec->NumInputs; ++i)
            {
                PGNodeConnect( node, i,
                               PGGraphGetNode(graph, JSONNumberGetValue(JSONArrayGetElement(input_nodes, i))),
                               JSONNumberGetValue(JSONArrayGetElement(input_node_output_indexes, i))
                               );
            }
        }
    }

    return graph;
}