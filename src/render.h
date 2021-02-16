#pragma once

#include "vector.h"

#include <SDL.h>

typedef struct sprite_sheet_s sprite_sheet_t;

void draw_rect_solid(SDL_Renderer* rend, i32 x, i32 y, i32 w, i32 h, rgba_t color);
void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet, rect_t* bbox,
		       const f64 scale, const f32 angle, const bool flip);
