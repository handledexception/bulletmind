#ifndef _H_IMGFILE
#define _H_IMGFILE

#include "c99defs.h"

#include <SDL.h>

typedef enum {
    IMG_TYPE_TARGA,
    IMG_TYPE_BMP,
    IMG_TYPE_PNG,
    IMG_TYPE_JPEG,
    IMG_TYPE_RAW
} imgtype_t;

typedef enum {
    BGR24,
    RGB24,
    RGBA32,
    BGRA32,
    ARGB32,
} pix_fmt_t;

typedef struct img_file_s {
    uint8_t* data;
    imgtype_t type;
    SDL_Surface* surface;
    SDL_Texture* texture;
} img_file_t;

bool img_file_load(const char* path, img_file_t** out);
void img_file_create(uint8_t* data, uint32_t w, uint32_t h, uint32_t bpp, uint32_t stride, uint32_t format, img_file_t** out);
void img_file_create_texture(SDL_Renderer* ren, img_file_t* img);
void img_file_shutdown(img_file_t* img);

#endif