/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>

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
    graph->files = MBMalloc(mb, sizeof(pg_file_t));

    return graph;
}

int PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * Type)
{
    Graph->nodes = MBRealloc( Graph->memory_bank, Graph->nodes,
                              sizeof(PGNode_t *) * (Graph->num_nodes+1) );

    Graph->nodes[Graph->num_nodes] = new_PGNode(Type, Graph);

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
    pg_file_t * file = NULL;

    for (int i = 0; i < Graph->num_files; ++i)
        if (Graph->files[i].id == FileID) file = &Graph->files[i];

    if (file == NULL) return NULL;
    else return file->path;
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

    file->path = MBMalloc(Graph->memory_bank, strlen(Path)+1);
    strcpy(file->path, Path);

    file->id = FileID;
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