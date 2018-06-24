#include <stdio.h>
void writebmp(unsigned char * data, int width, int height, char * filename) {
    int rowbytes = width*3+(4-(width*3%4))%4, imagesize = rowbytes*height, y;
    unsigned short header[] = {0x4D42,0,0,0,0,26,0,12,0,width,height,1,24};
    *((unsigned int *)(header+1)) = 26 + imagesize-((4-(width*3%4))%4);
    FILE * file = fopen(filename, "wb");
    fwrite(header, 1, 26, file);
    for (y = height-1; y > 0; --y) fwrite(data+(y*width*3), rowbytes, 1, file);
    fwrite(data, width*3, 1, file);
    fclose(file);
}