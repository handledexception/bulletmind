#ifndef _H_FONT
#define _H_FONT

#include "c99defs.h"
#include "system.h"

typedef struct SDL_Renderer SDL_Renderer;

void font_print(engine_t* eng, int32_t x, int32_t y, float scale, const char* str, ...);

#endif
