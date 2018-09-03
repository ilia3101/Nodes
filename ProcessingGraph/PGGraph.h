#ifndef _PGGraph_h_
#define _PGGraph_h_

#include "types/PGGraph_types.h"
#include "types/PGTypes.h"

/* New graph, needs node directory to function */
PGGraph_t * new_PGGraph();

/* Gets the output of graph */
PGImage_t * PGGraphGetOutput(PGGraph_t * Graph);

/* Return value is index of the node */
int PGGraphAddNode(PGGraph_t * Graph, PGNodeSpec_t * NodeType);
int PGGraphAddNodeByTypeName(PGGraph_t * Graph, char * NodeTypeName);

/* Get number of nodes in graph */
int PGGraphGetNumNodes(PGGraph_t * Graph);

/* Get pointer to node */
PGNode_t * PGGraphGetNode(PGGraph_t * Graph, int Index);

/* Get index of node in graph. Nope should be part of that graph */
int PGGraphGetNodeIndex(PGGraph_t * Graph, PGNode_t * Node);



/******************************* File managment *******************************/
void PGGraphAddFileWithID(PGGraph_t * Graph, char * Path, int FileID);
/* Return value is file's ID */
int PGGraphAddFile(PGGraph_t * Graph, char * Path);
char * PGGraphGetFilePath(PGGraph_t * Graph, int FileID);
char * PGGraphGetFilePathByIndex(PGGraph_t * Graph, int Index);
int PGGraphGetNumFiles(PGGraph_t * Graph);
int PGGraphGetFileID(PGGraph_t * Graph, int Index);
void PGGraphSetFileID(PGGraph_t * Graph, int Index, int FileID);
int PGGraphGetFileIndex(PGGraph_t * Graph, int FileID);
void PGGraphSetCurrentFileID(PGGraph_t * Graph, int CurrentFileID);

#endif