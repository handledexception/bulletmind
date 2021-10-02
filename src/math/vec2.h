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

#include "core/c99defs.h"
#include "core/types.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
}
#endif

typedef struct vec2i {
	s32 x;
	s32 y;
} vec2i_t;

typedef struct vec2f {
	f32 x;
	f32 y;
} vec2f_t;

static inline void vec2f_set(vec2f_t* dst, f32 x, f32 y)
{
	dst->x = x;
	dst->y = y;
}

static inline void vec2i_set(vec2i_t* dst, s32 x, s32 y)
{
	dst->x = x;
	dst->y = y;
}

static inline void vec2f_zero(vec2f_t* dst)
{
	vec2f_set(dst, 0.f, 0.f);
}

static inline void vec2f_copy(vec2f_t* dst, const vec2f_t other)
{
	dst->x = other.x;
	dst->y = other.y;
}

static inline void vec2f_add(vec2f_t* dst, const vec2f_t lhs, const vec2f_t rhs)
{
	vec2f_set(dst, lhs.x + rhs.x, lhs.y + rhs.y);
}

static inline void vec2f_sub(vec2f_t* dst, const vec2f_t lhs, const vec2f_t rhs)
{
	vec2f_set(dst, lhs.x - rhs.x, lhs.y - rhs.y);
}

static inline void vec2f_mul(vec2f_t* dst, const vec2f_t lhs, const vec2f_t rhs)
{
	vec2f_set(dst, lhs.x * rhs.x, lhs.y * rhs.y);
}

static inline void vec2f_div(vec2f_t* dst, const vec2f_t lhs, const vec2f_t rhs)
{
	vec2f_set(dst, lhs.x / rhs.x, lhs.y / rhs.y);
}

static inline void vec2f_addf(vec2f_t* dst, const vec2f_t v, f32 f)
{
	vec2f_set(dst, v.x + f, v.y + f);
}

static inline void vec2f_subf(vec2f_t* dst, const vec2f_t v, f32 f)
{
	vec2f_set(dst, v.x - f, v.y - f);
}

static inline void vec2f_mulf(vec2f_t* dst, const vec2f_t v, f32 f)
{
	vec2f_set(dst, v.x * f, v.y * f);
}

static inline void vec2f_divf(vec2f_t* dst, const vec2f_t v, f32 f)
{
	vec2f_set(dst, v.x / f, v.y / f);
}

static inline void vec2i_div(vec2i_t* dst, const vec2i_t v, s32 i)
{
	vec2i_set(dst, v.x / i, v.y / i);
}

static inline void vec2f_negate(vec2f_t* dst, const vec2f_t v)
{
	vec2f_set(dst, -v.x, -v.y);
}

static inline f32 vec2f_dot(const vec2f_t v1, const vec2f_t v2)
{
	return v1.x * v2.x + v1.y * v2.x;
}

static inline f32 vec2f_len(const vec2f_t v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

static inline f32 vec2f_norm(vec2f_t* dst, const vec2f_t v)
{
	const f32 length = 1.f / vec2f_len(v);
	vec2f_mulf(dst, v, length);
	return length;
}

static inline f32 vec2f_dist(const vec2f_t v1, const vec2f_t v2)
{
	vec2f_t temp;
	vec2f_sub(&temp, v1, v2);
	return vec2f_len(temp);
}

static inline void vec2f_fabsf(vec2f_t* dst, const vec2f_t v)
{
	vec2f_set(dst, fabsf(v.x), fabsf(v.y));
}

static inline void vec2f_friction(vec2f_t* dst, const vec2f_t a, f32 friction)
{
	f32 speed = vec2f_len(a);
	f32 new_speed = speed - (speed * friction);

	if (new_speed > 0)
		new_speed /= speed;
	else
		new_speed = 0;

	vec2f_mulf(dst, a, new_speed);
}

typedef struct rgba {
	f32 r;
	f32 g;
	f32 b;
	f32 a;
} rgba_t;

#ifdef __cplusplus
}
#endif
