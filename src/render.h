#ifndef H_RENDER
#define H_RENDER

#include "vector.h"

#include <SDL.h>

void draw_rect_solid(SDL_Renderer* rend, int32_t x, int32_t y, int32_t w, int32_t h, rgba_t color);

#endif