#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#ifdef __linux__
#include <alloca.h>
#endif

#include "../MemoryBank/MemoryBank.h"

#include "types/PGTypes.h"
#include "ProcessingGraph.h"
#include "PGGraph.h"


PGGraph_t * new_PGGraph()
{
    if (!ProcessingGraphInitialised())
    {
        fprintf(stderr, "ProcessingGraph library not initialised.\n");
        return NULL;
    }

    MemoryBank_t * mb = new_MemoryBank(1);
    PGGraph_t * graph = MBMalloc(mb, sizeof(PGGraph_t));
    graph->memory_bank = mb;

    graph->num_nodes = 0;
    graph->nodes = MBMalloc(mb, sizeof(PGNode_t *));

    graph->num_files = 0;
    graph->current_file_id = 0;
    graph->files = MBMalloc(mb, sizeof(pg_file_t));

    return graph;
}

/* TODO finish */
void delete_PGGraph(PGGraph_t * Graph)
{
    delete_MemoryBank(Graph->memory_bank);
}

int PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * Type)
{
    PGNode_t * node = new_PGNode(Type, Graph);
    PGNodeSetGraph(node, Graph);

    Graph->nodes = MBRealloc( Graph->memory_bank, Graph->nodes,
                              sizeof(PGNode_t *) * (Graph->num_nodes+1) );
    Graph->nodes[Graph->num_nodes] = node;

    /* Increment name number until it is unique. Not too efficient but simple */
    int node_with_same_name_exists = 0, attempts = 0;
    char name[128];
    strcpy(name, PGNodeGetName(node));

    do {
        node_with_same_name_exists = 0;
        for (int n = 0; n < Graph->num_nodes; ++n)
        {
            if (!strcmp(name, PGNodeGetName(Graph->nodes[n]))
                && strlen(name) == strlen(PGNodeGetName(Graph->nodes[n])) )
            {
                node_with_same_name_exists = 1;
                break;
            }
        }
        if (node_with_same_name_exists)
        {
            snprintf(name, 128, "%s.%03i", PGNodeGetName(node), ++attempts);
        }
    } while (node_with_same_name_exists);

    PGNodeSetName(node, name);

    return Graph->num_nodes++;
}

int PGGraphAddNodeByTypeName(PGGraph_t * Graph, char * NodeTypeName)
{
    PGNodeSpec_t * node_type = NULL;
    PGNodeSpec_t ** node_types = ProcessingGraphGetNodeTypes();
    int num_nodes = ProcessingGraphGetNumNodeTypes(Graph);
    for (int i = 0; i < num_nodes; ++i)
    {
        if (!strcmp(node_types[i]->Name, NodeTypeName))
        {
            node_type = node_types[i];
            break;
        }
    }

    if (node_type == NULL)
    {
        printf("PGGraphAddNodeByTypeName: \"%s\" not found\n", NodeTypeName);
        return -1;
    }

    return PGGraphAddNode(Graph, node_type);
}

int PGGraphGetNumNodes(PGGraph_t * Graph)
{
    return Graph->num_nodes;
}

PGNode_t * PGGraphGetNode(PGGraph_t * Graph, int Index)
{
    return Graph->nodes[Index];
}

PGNode_t * PGGraphGetNodeByName(PGGraph_t * Graph, char * Name)
{
    int num_nodes = PGGraphGetNumNodes(Graph);
    for (int n = 0; n < num_nodes; ++n)
    {
        PGNode_t * node = PGGraphGetNode(Graph, n);
        char * name = PGNodeGetName(node);
        if (!strcmp(name, Name) && strlen(name) == strlen(Name))
            return node;
    }
    return NULL;
}

/* Gets the output of a node */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph)
{
    /* Find the output node... */
    PGNode_t * output_node = NULL;
    int num_nodes = PGGraphGetNumNodes(Graph);
    for (int i = 0; i < num_nodes; ++i)
    {
        PGNode_t * node = PGGraphGetNode(Graph,i);
        if (!strcmp(PGNodeGetSpec(node)->Name, "Output"))
        {
            output_node = node;
            break;
        }
    }

    if (output_node == NULL)
    {
        puts("PGGraphGetOutput: No output node found in graph.");
        return NULL;
    }

    return PGNodeGetInput(output_node, 0)->value.image;
}

int PGGraphGetNodeIndex(PGGraph_t * Graph, PGNode_t * Node)
{
    int num_nodes = PGGraphGetNumNodes(Graph);
    int index = 0;
    while (index < num_nodes)
    {
        if (Graph->nodes[index] == Node) return index;
        ++index;
    }
    return -1;
}




/******************************* File managment *******************************/

char * PGGraphGetFilePath(PGGraph_t * Graph, int FileID)
{
    char * path = NULL;

    for (int i = 0; i < Graph->num_files; ++i)
        if (Graph->files[i].id == FileID) path = Graph->files[i].path;

    return path;
}

char * PGGraphGetFilePathByIndex(PGGraph_t * Graph, int Index)
{
    return Graph->files[Index].path;
}

int PGGraphGetNumFiles(PGGraph_t * Graph)
{
    return Graph->num_files;
}

void PGGraphAddFileWithID(PGGraph_t * Graph, char * Path, int FileID)
{
    Graph->files = MBRealloc( Graph->memory_bank, Graph->files,
                              sizeof(pg_file_t) * (++Graph->num_files) );

    pg_file_t * file = &Graph->files[Graph->num_files-1];

    file->id = FileID;

    file->path = MBMalloc(Graph->memory_bank, strlen(Path)+1);
    strcpy(file->path, Path);
}

int PGGraphAddFile(PGGraph_t * Graph, char * Path)
{
    PGGraphAddFileWithID(Graph, Path, Graph->current_file_id);
    return Graph->current_file_id++;
}

int PGGraphGetFileID(PGGraph_t * Graph, int Index)
{
    return Graph->files[Index].id;
}

void PGGraphSetFileID(PGGraph_t * Graph, int Index, int FileID)
{
    Graph->files[Index].id = FileID;
}

void PGGraphSetCurrentFileID(PGGraph_t * Graph, int CurrentFileID)
{
    Graph->current_file_id = CurrentFileID;
}