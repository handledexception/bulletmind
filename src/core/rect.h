#pragma once

#include "core/types.h"

#include <math.h>

typedef struct {
	s32 x, y;
	s32 w, h;
} rect_t;

static inline void center_in_rect(rect_t* dst, const rect_t* outer, const rect_t* inner)
{
	dst->x = (outer->x + (outer->w / 2)) - (inner->x + (inner->w / 2));
	dst->y = (outer->y + (outer->h / 2)) - (inner->y + (inner->h / 2));
	dst->w = inner->w;
	dst->h = inner->h;
}

static inline f32 radius_of_circle_in_rect(rect_t r)
{
	f32 a = (f32)(r.w - r.x);
	f32 b = (f32)(r.h - r.y);
	return 0.5f * (sqrtf((a * a) + (b * b)));
}
