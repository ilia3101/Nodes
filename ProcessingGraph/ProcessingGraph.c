#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>
/* TODO: Make this code file work on WINDOWS (windows.h/LoadLibrary) */
#ifndef WIN32
    #include <dirent.h>
    #include <dlfcn.h>
#else
    #include <windows.h>
#endif

#include "../MemoryBank/MemoryBank.h"

#include "types/PGTypes.h"
#include "ProcessingGraph.h"
#include "PGGraph.h"


static int pg_library_initialised = 0;

/* All node types in the library. GLoabal, yes */
static int pg_num_node_types = 0;
static PGNodeSpec_t ** pg_node_types = NULL;
static void ** pg_node_dlls = NULL; /* Pointers to dlopen/LoadLibrary returns */


void ProcessingGraphLoadNodeFromFile(char * NodeFilePath)
{
    /* This works everywhere but Windows (TODO: add LoadLibrary) */
    void * node = dlopen(NodeFilePath, RTLD_NOW);
    if (node == NULL)
    {
        fprintf(stderr, "Couldn't load node plugin...\n%s\n", dlerror());
        return;
    }

    PGNodeSpec_t * (* node_get_spec)() = dlsym(node, "GetNodeSpec");
    if (node_get_spec == NULL)
    {
        fprintf(stderr, "Didn't find GetNodeSpec...\n%s\n", dlerror());
        return;
    }

    ++pg_num_node_types;

    pg_node_dlls = realloc(pg_node_dlls, pg_num_node_types*sizeof(void *));
    pg_node_types = realloc( pg_node_types,
                             pg_num_node_types * sizeof(PGNodeSpec_t *) );

    pg_node_types[pg_num_node_types-1] = node_get_spec();
    pg_node_dlls[pg_num_node_types-1] = node;

    printf("🐸  Successfully loaded %s\n", NodeFilePath);
}


/* Must be called before using the library. Loads all node plugins in to the
 * pg_node_types array (first time I ever found a global variable useful) */
void ProcessingGraphInitialiseLibrary(char * NodeDirectory)
{
    if (ProcessingGraphInitialised()) return;

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

        printf("Loaded %d plugins.\n", pg_num_node_types);

        pg_library_initialised = 1;
    }
    else
    {
        perror("Couldn't load node plugins");
    }
}

int ProcessingGraphInitialised()
{
    return pg_library_initialised;
}

int ProcessingGraphGetNumNodeTypes()
{
    return pg_num_node_types;
}

PGNodeSpec_t ** ProcessingGraphGetNodeTypes()
{
    return pg_node_types;
}

void * ProcessingGraphGetNodeDLLHandle(int NodeIndex)
{
    if (NodeIndex < pg_num_node_types && NodeIndex >= 0)
        return pg_node_dlls[NodeIndex];
    else
        return NULL;
}