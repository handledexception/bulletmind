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

#include "render.h"
#include "sprite.h"

#include "core/time_convert.h"

#include "math/vec2.h"

#include "platform/platform.h"

void draw_rect_solid(SDL_Renderer* rend, i32 x, i32 y, i32 w, i32 h,
		     rgba_t rgba)
{
	rect_t rect = {x, y, w, h};
	SDL_SetRenderDrawColor(rend, rgba.r, rgba.g, rgba.b, rgba.a);
	SDL_RenderFillRect(rend, (const SDL_Rect*)&rect);
}

void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet,
		       rect_t* bbox, const f64 scale, const f32 angle,
		       const bool flip)
{
	static i32 frame_num = 0;
	static f64 frame_time = 0.0;

	sprite_t* backing_sprite = sprite_sheet->backing_sprite;

	ss_frame_t* current_frame = &sprite_sheet->frames[frame_num];
	// const f32 frame_delay = current_frame->duration * scale;
	f64 frame_delay = 0.0;
	if (scale > 0.0 && !isnan(scale) && !isinf(scale))
		frame_delay = 1.0 / (scale);
	else
		frame_delay = 0.0;

	// if (frame_delay < current_frame->duration)
	//     frame_delay = 0.0;
	if (frame_delay > 0.750)
		frame_delay = 0.0;
	if (frame_delay > 0.0 && frame_delay < 0.025)
		frame_delay = 0.025;

	// printf("frame_delay %f\n", frame_delay);

	SDL_Rect dst = {
		bbox->x,
		bbox->y,
		current_frame->bounds.w * backing_sprite->scaling,
		current_frame->bounds.h * backing_sprite->scaling,
	};

	SDL_RendererFlip sprite_flip = SDL_FLIP_NONE;
	if (flip)
		sprite_flip = SDL_FLIP_HORIZONTAL;

	SDL_RenderCopyEx(rend, backing_sprite->texture, &current_frame->bounds,
			 &dst, angle, NULL, sprite_flip);

	if (frame_delay > 0.0 && os_get_time_sec() >= frame_time) {
		frame_time = os_get_time_sec() + frame_delay;
		frame_num += 1;
	}
	if (frame_num > sprite_sheet->num_frames - 1)
		frame_num = 0;
}
