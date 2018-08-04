#ifndef _PGImage_h_
#define _PGImage_h_


#include "types/PGImage_types.h"
#include "types/PGTypes.h"

PGImage_t * new_PGImage(int Width, int Height);
void delete_PGImage(PGImage_t * Image);

/* Usefl setters */
void PGImageSetDimensions(PGImage_t * Image, int NewWidth, int NewHeight);

/* Usefull getters */
float * PGImageGetDataPointer(PGImage_t * Image);
int PGImageGetWidth(PGImage_t * Image);
int PGImageGetHeight(PGImage_t * Image);
PGColour_t PGImageGetColour(PGImage_t * Image, int X, int Y);

#endif