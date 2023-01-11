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

#ifndef H_BM_MATH_VEC4
#define H_BM_MATH_VEC4

#include "core/types.h"
#include "core/export.h"
#include "math/vec3.h"

typedef struct vec4f {
	union {
		struct { 
			f32 x, y, z, w;
		};
		f32 elems[4];
	};
} vec4f_t;

typedef struct rgba {
	union {
		struct {
			u8 r, g, b, a;
		};
		f32 elems[4];
	};
} rgba_t;

static inline vec4f_t vec4_set(f32 x, f32 y, f32 z, f32 w)
{
	vec4f_t v = { 0 };
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

static inline vec4f_t vec4_zero()
{
	vec4f_t v = vec4_set(0.0f, 0.0f, 0.0f, 0.0f);
	return v;
}

static inline vec4f_t vec4_add(vec4f_t a, vec4f_t b)
{
	return vec4_set(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static inline vec4f_t vec4_sub(vec4f_t a, vec4f_t b)
{
	return vec4_set(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static inline vec4f_t vec4_mul(vec4f_t a, vec4f_t b)
{
	return vec4_set(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

static inline vec4f_t vec4_div(vec4f_t a, vec4f_t b)
{
	return vec4_set(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

static inline vec4f_t vec4_mulf(vec4f_t v, f32 s)
{
	return vec4_set(v.x * s, v.y * s, v.z * s, v.w * s);
}

static inline vec4f_t vec4_neg(vec4f_t v)
{
	return vec4_set(-v.x, -v.y, -v.z, -v.w);
}

static inline f32 vec4_dot(vec4f_t a, vec4f_t b)
{
	return (f32)((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w + b.w));
}

static inline f32 vec4_len(vec4f_t v)
{
	return (f32)sqrt(vec4_dot(v, v));
}

static inline vec4f_t vec4_norm(vec4f_t v)
{
	return vec4_mulf(v, 1.0f / vec4_len(v));
}

static inline f32 vec4f_dist(vec4f_t a, vec4f_t b)
{
	f32 dx = (a.x - b.x);
	f32 dy = (a.y - b.y);
	f32 dz = (a.z - b.z);
	f32 dw = (a.w - b.w);
	return (f32)(sqrt(dx * dx + dy * dy + dz * dz + dw * dw));
}

static inline vec4f_t vec4_from_vec3(vec3f_t v)
{
	return vec4_set(v.x, v.y, v.z, 0.0f);
}

#endif // H_BM_MATH_VEC4
