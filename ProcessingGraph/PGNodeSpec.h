#ifndef _PGNodeSpec_h_
#define _PGNodeSpec_h_

#include "types/PGNode_types.h"

/* Info */
char * PGNodeSpecGetName(PGNodeSpec_t * NodeSpec);
char * PGNodeSpecGetDescription(PGNodeSpec_t * NodeSpec);
char * PGNodeSpecGetCategory(PGNodeSpec_t * NodeSpec);
// char * PGNodeSpecGetCanUseGPU(PGNodeSpec_t * NodeSpec); /* Maybe */

#endif