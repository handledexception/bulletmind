#pragma once

#include "core/types.h"

#include <math.h>

typedef struct {
	i32 x, y;
	i32 w, h;
} rect_t;

inline f32 get_rect_radius(rect_t r)
{
	f32 a = (f32)(r.w - r.x);
	f32 b = (f32)(r.h - r.y);
	return 0.5f * (sqrtf((a * a) + (b * b)));
}
