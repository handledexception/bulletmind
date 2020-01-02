#ifndef _H_IMGFILE
#define _H_IMGFILE

#include "c99defs.h"

typedef enum {
    TARGA,
    RGB,
    BGR
} imgtype_t;

typedef struct img_file {
    int32_t width, height;
    int8_t bpp;
    int32_t stride;
    uint8_t *data;
    size_t filesize;
    imgtype_t type;
} img_file_t;

bool img_file_init(const char* path, img_file_t** out);
void img_file_shutdown(img_file_t* img);

#endif