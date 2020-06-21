#ifndef _H_IMGFILE
#define _H_IMGFILE

#include "c99defs.h"

#include <SDL.h>

typedef enum {
    IMG_TYPE_TARGA,
    IMG_TYPE_BMP,
    IMG_TYPE_PNG,
    IMG_TYPE_JPEG,
    IMG_TYPE_RAW,
} imgtype_t;

typedef enum {
    BGR24,
    RGB24,
    RGBA32,
    BGRA32,
    ARGB32,
} pix_fmt_t;

typedef struct sprite_s {
    uint8_t* data;
    imgtype_t type;
    SDL_Surface* surface;
    SDL_Texture* texture;
    bool has_alpha;
} sprite_t;

bool sprite_load(const char* path, sprite_t** out);
void sprite_create(uint8_t* data, uint32_t w, uint32_t h, uint32_t bpp, uint32_t stride, uint32_t format, sprite_t** out);
void sprite_create_texture(SDL_Renderer* ren, sprite_t* img);
void sprite_shutdown(sprite_t* img);

#endif