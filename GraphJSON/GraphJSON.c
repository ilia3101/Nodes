#include <stdio.h>
#include "GraphJSON.h"

JSONBlock_t * PGGraphToJSON(PGGraph_t * Graph)
{
    int num_nodes = PGGraphGetNumNodes(Graph);

    JSONBlock_t * all_nodes = new_EmptyJSONArray(NULL, num_nodes);
    
    for (int i = 0; i < num_nodes; ++i)
    {
        PGNode_t * node = PGGraphGetNode(Graph, i);
        JSONBlock_t * json_node = new_EmptyJSONObject(all_nodes, 0);

        JSONObjectAppendAttribute(json_node, new_JSONString(json_node, PGNodeGetSpec(node)->Name), "type");

        /* Now get the object's inputs */
        int num_inputs = PGNodeGetSpec(node)->NumInputs;
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

        JSONArraySetElement(all_nodes, i, json_node);
    }

    return all_nodes;
}