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
	f32 x;
	f32 y;
	f32 z;
	f32 w;
} vec4f_t;

typedef struct rgba {
	u8 r;
	u8 g;
	u8 b;
	u8 a;
} rgba_t;

static inline void vec4f_set(vec4f_t* dst, f32 x, f32 y, f32 z, f32 w)
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
	dst->w = w;
}

static inline void vec4f_from_vec3(vec4f_t* dst, const vec3f_t* v)
{
	dst->x = v->x;
	dst->y = v->y;
	dst->z = v->z;
	dst->w = 1.f;
}

static inline void vec4f_zero(vec4f_t* dst)
{
	vec4f_set(dst, 0.f, 0.f, 0.f, 0.f);
}

static inline void vec4f_copy(vec4f_t* dst, const vec4f_t* rhs)
{
	vec4f_set(dst, rhs->x, rhs->y, rhs->z, rhs->w);
}

static inline void vec4f_add(vec4f_t* dst, const vec4f_t* lhs,
			     const vec4f_t* rhs)
{
	vec4f_set(dst, lhs->x + rhs->x, lhs->y + rhs->y, lhs->z + rhs->z,
		  lhs->w + rhs->w);
}

static inline void vec4f_sub(vec4f_t* dst, const vec4f_t* lhs,
			     const vec4f_t* rhs)
{
	vec4f_set(dst, lhs->x - rhs->x, lhs->y - rhs->y, lhs->z - rhs->z,
		  lhs->w - rhs->w);
}

static inline void vec4f_mul(vec4f_t* dst, const vec4f_t* lhs,
			     const vec4f_t* rhs)
{
	vec4f_set(dst, lhs->x * rhs->x, lhs->y * rhs->y, lhs->z * rhs->z,
		  lhs->w * rhs->w);
}

static inline void vec4f_div(vec4f_t* dst, const vec4f_t* lhs,
			     const vec4f_t* rhs)
{
	vec4f_set(dst, lhs->x / rhs->x, lhs->y / rhs->y, lhs->z / rhs->z,
		  lhs->w / rhs->w);
}

static inline f32 vec4f_dot(const vec4f_t* v1, const vec4f_t* v2)
{
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z + v1->w * v2->w);
}

static inline f32 vec4f_len(const vec4f_t* v)
{
	f32 v_dot = vec4f_dot(v, v);
	return v_dot > 0.f ? sqrtf(v_dot) : 0.f;
}

static inline void vec4_mulf(vec4f_t* dst, const vec4f_t* v, f32 s)
{
	vec4f_set(dst, v->x * s, v->y * s, v->z * s, v->w * s);
}

static inline void vec4f_norm(vec4f_t* dst, const vec4f_t* v)
{
	f32 len = vec4f_dot(v, v);
	if (len > 0.f) {
		len = 1.f / sqrtf(len);
		dst->x = v->x * len;
		dst->y = v->y * len;
		dst->z = v->z * len;
		dst->w = v->w * len;
	} else {
		vec4f_zero(dst);
	}
}

#endif // H_BM_MATH_VEC4
