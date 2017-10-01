#ifndef _H_IMGFILE
#define _H_IMGFILE

#include "c99defs.h"

typedef enum {
    TARGA,
    RGB
} imgtype_t;

typedef struct imgfile_s {
    int32_t width, height;
    uint8_t *data;
    size_t filesize;
    imgtype_t type;
} imgfile_t;

bool imgfile_init(const char *path, imgfile_t *out);
void imgfile_shutdown();

#endif