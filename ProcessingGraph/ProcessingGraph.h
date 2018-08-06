/* Processing graph library */

#ifndef _ProcessingGraph_h_
#define _ProcessingGraph_h_

#include "types/PGTypes.h"

#include "PGGraph.h"
#include "PGImage.h"
#include "PGNode.h"

/* Call before using the library */
void ProcessingGraphInitialiseLibrary(char * NodeDirectory);
int ProcessingGraphInitialised(); /* To check if previous function called */


/* Nodes and stuff */
void ProcessingGraphLoadNodeFromFile(char * NodeFilePath);
int ProcessingGraphGetNumNodeTypes();
PGNodeSpec_t ** ProcessingGraphGetNodeTypes();
void * ProcessingGraphGetNodeDLLHandle(int NodeIndex);


#endif