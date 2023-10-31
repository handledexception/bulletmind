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
#include "math/vec4.h"

#include "platform/platform.h"

// https://stackoverflow.com/questions/38334081/howto-draw-circles-arcs-and-vector-graphics-in-sdl
void draw_circle(SDL_Renderer* rend, f32 cx, f32 cy, f32 radius)
{
	const f32 diameter = (radius * 2.f);

	f32 x = (radius - 1.f);
	f32 y = 0.f;
	f32 tx = 1.f;
	f32 ty = 1.f;
	f32 error = (tx - diameter);

	while (x >= y) {
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(rend, (int)(cx + x), (int)(cy - y));
		SDL_RenderDrawPoint(rend, (int)(cx + x), (int)(cy + y));
		SDL_RenderDrawPoint(rend, (int)(cx - x), (int)(cy - y));
		SDL_RenderDrawPoint(rend, (int)(cx - x), (int)(cy + y));
		SDL_RenderDrawPoint(rend, (int)(cx + y), (int)(cy - x));
		SDL_RenderDrawPoint(rend, (int)(cx + y), (int)(cy + x));
		SDL_RenderDrawPoint(rend, (int)(cx - y), (int)(cy - x));
		SDL_RenderDrawPoint(rend, (int)(cx - y), (int)(cy + x));

		if (error <= 0.f) {
			++y;
			error += ty;
			ty += 2.f;
		}

		if (error > 0.f) {
			--x;
			tx += 2.f;
			error += (tx - diameter);
		}
	}
}

void draw_rect_outline(SDL_Renderer* rend, rect_t* rect, rgba_t* color)
{
	SDL_SetRenderDrawColor(rend, color->r, color->g, color->b, color->a);
	SDL_RenderDrawRect(rend, (const SDL_Rect*)rect);
}

void draw_rect_solid(SDL_Renderer* rend, rect_t* rect, rgba_t* color)
{
	SDL_SetRenderDrawColor(rend, color->r, color->g, color->b, color->a);
	SDL_RenderFillRect(rend, (const SDL_Rect*)rect);
}

void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet,
		       vec2f_t* org, const f64 scale, const f32 angle,
		       const bool flip)
{
	//TODO(paulh): move the frame time fields into the sprite_sheet_t structure
	static s32 frame_num = 0;
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
	s32 scaled_width =
		(s32)current_frame->bbox.max.x * backing_sprite->scaling;
	s32 scaled_height =
		(s32)current_frame->bbox.max.y * backing_sprite->scaling;
	SDL_Rect dst = {
		(s32)(org->x) - scaled_width / 2,
		(s32)(org->y) - scaled_height / 2,
		scaled_width,
		scaled_height,
	};

	SDL_RendererFlip sprite_flip = SDL_FLIP_NONE;
	if (flip)
		sprite_flip = SDL_FLIP_HORIZONTAL;

	rect_t frame_rect = {
		.x = (s32)current_frame->bbox.min.x,
		.y = (s32)current_frame->bbox.min.y,
		.w = (s32)current_frame->bbox.max.x,
		.h = (s32)current_frame->bbox.max.y,
	};
	SDL_RenderCopyEx(rend, backing_sprite->texture, (SDL_Rect*)&frame_rect,
			 &dst, angle, NULL, sprite_flip);

	if (frame_delay > 0.0 && os_get_time_sec() >= frame_time) {
		frame_time = os_get_time_sec() + frame_delay;
		frame_num += 1;
	}
	if (frame_num > sprite_sheet->num_frames - 1)
		frame_num = 0;
}
