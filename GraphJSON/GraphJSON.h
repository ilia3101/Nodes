/* Functions for saving a Graph to JSON and reading a JSON to create a graph */
#ifndef _GraphJSON_h_
#define _GraphJSON_h_

#include "../JSONParser/JSONParser.h"
#include "../ProcessingGraph/ProcessingGraph.h"

JSONBlock_t * PGGraphToJSON(PGGraph_t * Graph);
PGGraph_t * JSONToPGGraph(JSONBlock_t * JSON);

#endif