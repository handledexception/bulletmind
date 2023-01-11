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

#ifndef H_BM_MATH_VEC2
#define H_BM_MATH_VEC2

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

static inline vec2f_t vec2_set(f32 x, f32 y)
{
	vec2f_t v = { 0 };
	v.x = x;
	v.y = y;
	return v;
}

static inline vec2i_t vec2i_set(s32 x, s32 y)
{
	vec2i_t v = { 0 };
	v.x = x;
	v.y = y;
	return v;
}

static inline vec2f_t vec2_zero()
{
	return vec2_set(0.f, 0.f);
}

static inline vec2f_t vec2_copy(const vec2f_t v)
{
	vec2f_t res = { 0 };
	res.x = v.x;
	res.y = v.y;
	return res;
}

static inline vec2f_t vec2_add(vec2f_t a, vec2f_t b)
{
	return vec2_set(a.x + b.x, a.y + b.y);
}

static inline vec2f_t vec2_sub(vec2f_t a, vec2f_t b)
{
	return vec2_set(a.x - b.x, a.y - b.y);
}

static inline vec2f_t vec2_mul(vec2f_t a, vec2f_t b)
{
	return vec2_set(a.x * b.x, a.y * b.y);
}

static inline vec2f_t vec2_div(vec2f_t a, vec2f_t b)
{
	return vec2_set(a.x / b.x, a.y / b.y);
}

static inline vec2f_t vec2_addf(const vec2f_t v, f32 f)
{
	return vec2_set(v.x + f, v.y + f);
}

static inline vec2f_t vec2_subf(const vec2f_t v, f32 f)
{
	return vec2_set(v.x - f, v.y - f);
}

static inline vec2f_t vec2_mulf(const vec2f_t v, f32 f)
{
	return vec2_set(v.x * f, v.y * f);
}

static inline vec2f_t vec2_divf(const vec2f_t v, f32 f)
{
	return vec2_set(v.x / f, v.y / f);
}

static inline vec2i_t vec2i_divi(const vec2i_t v, s32 i)
{
	return vec2i_set(v.x / i, v.y / i);
}

static inline vec2f_t vec2_negate(const vec2f_t v)
{
	return vec2_set(-v.x, -v.y);
}

static inline f32 vec2_dot(const vec2f_t v1, const vec2f_t v2)
{
	return v1.x * v2.x + v1.y * v2.x;
}

static inline f32 vec2_len(const vec2f_t v)
{
	return (f32)sqrt(vec2_dot(v, v));
}

static inline vec2f_t vec2_norm(const vec2f_t v)
{
	const f32 len = vec2_len(v);
	return len == 0.0f ? v : vec2_mulf(v, 1.0f / len);
}

static inline f32 vec2_dist(const vec2f_t v1, const vec2f_t v2)
{
	return vec2f_len(vec2_sub(v1, v2));
}

static inline vec2f_t vec2_fabsf(const vec2f_t v)
{
	return vec2_set(fabsf(v.x), fabsf(v.y));
}

static inline vec2f_t vec2_friction(const vec2f_t a, f32 friction)
{
	f32 speed = vec2_len(a);
	f32 new_speed = speed - (speed * friction);
	if (new_speed > 0)
		new_speed /= speed;
	else
		new_speed = 0;
	return vec2_mulf(a, new_speed);
}

#ifdef __cplusplus
}
#endif

#endif // H_BM_MATH_VEC2
