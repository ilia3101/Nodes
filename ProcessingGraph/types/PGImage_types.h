#ifndef _PGImage_types_h_
#define _PGImage_types_h_

/* Images are always RGBA. Will support OpenCL s00n */
typedef struct {
    int height, width;
    float * data; /* This can be directly cast to (PGColour_t *) */
    float loc_x, loc_y; /* Where the image is, (0,0) by default */
} PGImage_t;

#endif