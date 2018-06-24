/* The processing graph library */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <time.h>

// #include "../MemoryBank/MemoryBank.h"

#include "types/PGImage_types.h"
#include "types/PGTypes.h"

/* PGImage currently does not use memorybank due to its simplicity (right now)*/
PGImage_t * new_PGImage(int Width, int Height)
{
    PGImage_t * image = malloc(sizeof(PGImage_t));
    image->data = malloc(Width * Height * sizeof(float) * 4);
    image->width = Width;
    image->height = Height;
    return image;
}

float * PGImageGetDataPointer(PGImage_t * Image)
{
    return Image->data;
}

int PGImageGetWidth(PGImage_t * Image)
{
    return Image->width;
}

int PGImageGetHeight(PGImage_t * Image)
{
    return Image->height;
}

PGColour_t PGImageGetColour(PGImage_t * Image, int X, int Y)
{
    size_t location = PGImageGetWidth(Image) * Y + X;
    return *(((PGColour_t *)PGImageGetDataPointer(Image)) + location);
}