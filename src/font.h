#ifndef _H_FONT
#define _H_FONT

#include "c99defs.h"

typedef struct SDL_Renderer SDL_Renderer;

// #define DEFAULT_FONT_7PX "font_7px.tga"
#define DEFAULT_FONT_7PX "font_7px_rgba.tga"

bool font_init(SDL_Renderer* ren, const char* path);
void font_print(SDL_Renderer* ren, int32_t x, int32_t y, float scale, const char* str, ...);
void font_shutdown();

#endif