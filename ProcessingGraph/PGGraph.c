/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>
#include <dirent.h>
/* TODO: FIX 4 WINDOWS */
#ifndef WIN32
#include <dlfcn.h>
#endif

#include "../MemoryBank/MemoryBank.h"

#include "types/PGTypes.h"
#include "ProcessingGraph.h"
#include "PGGraph.h"

int pg_library_initialised = 0;

/* All node types in the library. GLoabal, yes */
int pg_num_node_types = 0;
PGNodeSpec_t ** pg_node_types = NULL;
void ** pg_node_dlls = NULL; /* Pointers to dlopen/LoadLibrary returns */

void ProcessingGraphLoadNodeFromFile(char * NodeFilePath)
{
    /* This works everywhere but Windows (TODO: add LoadLibrary) */
    void * node = dlopen(NodeFilePath, RTLD_LAZY);
    if (node == NULL) {
        fprintf(stderr, "Couldn't load node plugin...\n%s\n", dlerror());
        return;
    }

    ++pg_num_node_types;

    pg_node_dlls = realloc(pg_node_dlls, pg_num_node_types*sizeof(void *));
    pg_node_types = realloc( pg_node_types,
                             pg_num_node_types * sizeof(PGNodeSpec_t *) );

    PGNodeSpec_t * (* node_get_spec)() = dlsym(node, "PGNodeGetSpec");

    if (node_get_spec == NULL) {
        fprintf(stderr, "Didn't find PGNodeGetSpec...\n%s\n", dlerror());
        return;
    }

    pg_node_types[pg_num_node_types-1] = node_get_spec();
    pg_node_dlls[pg_num_node_types-1] = node;
}

/* Must be called before using the library. Loads all node plugins in to the
 * pg_node_types array (first time I ever found a global variable useful) */
void InitialiseProcessingGraphLibrary(char * NodeDirectory)
{
    if (pg_library_initialised) return;

    pg_node_types == malloc(sizeof(PGNodeSpec_t *));
    pg_node_dlls == malloc(sizeof(void *));

    /* Add all node types in the node type category */
    DIR * dir = opendir(NodeDirectory);
    if (dir != NULL)
    {
        /* print all the files and directories within directory */
        struct dirent * ent;
        while ((ent = readdir(dir)) != NULL)
        {
            size_t pathlen = strlen(NodeDirectory)+strlen(ent->d_name)+2;
            char * path = alloca(pathlen);
            snprintf(path, pathlen, "%s/%s", NodeDirectory, ent->d_name);
            ProcessingGraphLoadNodeFromFile(path);
        }
        closedir(dir);

        pg_library_initialised = 1;
    }
    else
    {
        perror("Couldn't load node plugins");
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