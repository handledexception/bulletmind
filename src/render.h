/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "core/rect.h"
#include "math/vec2.h"

#include <SDL.h>

typedef struct sprite_sheet_s sprite_sheet_t;

void draw_circle(SDL_Renderer* rend, f32 cx, f32 cy, f32 radius);
void draw_rect_solid(SDL_Renderer* rend, rect_t rect, rgba_t color);
void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet,
		       rect_t* bbox, const f64 scale, const f32 angle,
		       const bool flip);
