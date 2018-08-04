/* Processing graph library */

#ifndef _ProcessingGraph_h_
#define _ProcessingGraph_h_

#include "types/PGTypes.h"

/* Call before using the library */
void InitialiseProcessingGraphLibrary(char * NodeDirectory);

/* All the important functions are in the following headers */
#include "PGGraph.h"
#include "PGImage.h"
#include "PGNode.h"

#endif