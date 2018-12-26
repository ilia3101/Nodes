#ifndef _GraphJSON_h_
#define _GraphJSON_h_

/* For saving graph object to JSON and back, in a very user readable way */

#include "../JSONParser/JSONParser.h"
#include "../ProcessingGraph/ProcessingGraph.h"

/* Saves Graph to JSON at path JSONFilePath */
void SavePGGraphToJSONFile(PGGraph_t * Graph, char * JSONFilePath);

/* Creates a JSON representing Graph. JSONFilePath is where you intend to save
 * the file so that relative paths can be generated */
JSONBlock_t * PGGraphToJSON(PGGraph_t * Graph, char * JSONFilePath);

/* Convert a JSON to a PGGraph. JSONFilePath is where the JSON was saved so
 * relative file paths can be used (they are useful if everything was moved) */
PGGraph_t * JSONToPGGraph(JSONBlock_t * JSON, char * JSONFilePath);

#endif