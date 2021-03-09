#pragma once

#include "c99defs.h"
#include "types.h"
#include "engine.h"

typedef struct SDL_Renderer SDL_Renderer;

void font_print(engine_t *eng, i32 x, i32 y, f32 scale, const char *str, ...);
