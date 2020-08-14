#ifndef _H_RENDER
#define _H_RENDER

#include "vector.h"

#include <SDL.h>

typedef struct sprite_sheet_s sprite_sheet_t;

void draw_rect_solid(SDL_Renderer* rend, int32_t x, int32_t y, int32_t w, int32_t h, rgba_t color);
void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet, rect_t* bbox, const double scale, const float angle, const bool flip);

#endif