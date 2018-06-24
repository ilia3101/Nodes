/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <dirent.h>
#include <time.h>

#include "../MemoryBank/MemoryBank.h"

#include "types/PGTypes.h"
#include "ProcessingGraph.h"
#include "PGGraph.h"

/* All node types in the library. GLoabal, yes */
static PGNodeSpec_t * pg_node_types = NULL;

void pg_graph_load_node(PGGraph_t * Graph, char * NodeFilePath)
{
    return;
}

/* Must be called before using the library. Loads all node plugins in to the
 * pg_node_types array (first time I ever found a global variable useful) */
void InitialiseProcessingGraphLibrary(char * NodeDirectory)
{
    /* Add all node types in the node type category */
    DIR * dir = opendir(NodeDirectory);
    if (dir != NULL)
    {
        /* print all the files and directories within directory */
        struct dirent * ent;
        while ((ent = readdir(dir)) != NULL)
            printf ("%s\n", ent->d_name);
        closedir(dir);
    }
    else {
        /* could not open directory */
        perror ("");
    }
}


PGGraph_t * new_PGGraph(char * NodeDirectory)
{
    if (pg_node_types == NULL)
    {
        puts("Error - ProcessingGraph library has not been initialised or "
             "NodeDirectory did not contain any nodes.");
        return NULL;
    }

    MemoryBank_t * mb = new_MemoryBank(1);
    PGGraph_t * graph = MBMalloc(mb, sizeof(PGGraph_t));
    graph->memory_bank = mb;

    
    return graph;
}

/* Gets the output of a node */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph)
{
    // PGNode_Output_t output = Node->output_functions[OutputIndex](Node);
    // return output;
    return NULL;
}

int PGGraphGetNumNodeTypes(PGGraph_t * Graph);

/* Gets all node type descrpitions */
PGNodeSpec_t * PGGraphGetNodeTypes(PGGraph_t * Graph);

void PGGraphAddNodeType(PGGraph_t * Graph, PGNodeSpec_t * NodeType);

int PGGraphAddNode(PGNodeSpec_t * NodeType);