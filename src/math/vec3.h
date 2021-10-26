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

#ifndef H_BM_MATH_VEC3
#define H_BM_MATH_VEC3

#include "core/types.h"
#include "core/export.h"

#ifdef __cplusplus
extern "C" {
}
#endif

typedef struct vec3f {
	f32 x;
	f32 y;
	f32 z;
} vec3f_t;

static inline void vec3f_set(vec3f_t* dst, f32 x, f32 y, f32 z)
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
}

static inline void vec3f_zero(vec3f_t* dst)
{
	vec3f_set(dst, 0.f, 0.f, 0.f);
}

static inline void vec3f_copy(vec3f_t* dst, const vec3f_t* src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
}

static inline void vec3f_add(vec3f_t* dst, const vec3f_t* lhs,
			     const vec3f_t* rhs)
{
	vec3f_set(dst, lhs->x + rhs->x, lhs->y + rhs->y, lhs->z + rhs->z);
}

static inline void vec3f_sub(vec3f_t* dst, const vec3f_t* lhs,
			     const vec3f_t* rhs)
{
	vec3f_set(dst, lhs->x - rhs->x, lhs->y - rhs->y, lhs->z - rhs->z);
}

static inline void vec3f_mul(vec3f_t* dst, const vec3f_t* lhs,
			     const vec3f_t* rhs)
{
	vec3f_set(dst, lhs->x * rhs->x, lhs->y * rhs->y, lhs->z * rhs->z);
}

static inline void vec3f_div(vec3f_t* dst, const vec3f_t* lhs,
			     const vec3f_t* rhs)
{
	vec3f_set(dst, lhs->x / rhs->x, lhs->y / rhs->y, lhs->z / rhs->z);
}

static inline void vec3f_addf(vec3f_t* dst, const vec3f_t* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x + rhs, lhs->y + rhs, lhs->z + rhs);
}

static inline void vec3f_subf(vec3f_t* dst, const vec3f_t* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x - rhs, lhs->y - rhs, lhs->z - rhs);
}

static inline void vec3f_mulf(vec3f_t* dst, vec3f_t* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x * rhs, lhs->y * rhs, lhs->z * rhs);
}

static inline void vec3f_divf(vec3f_t* dst, vec3f_t* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x / rhs, lhs->y / rhs, lhs->z / rhs);
}

static inline void vec3f_negate(vec3f_t* dst, const vec3f_t* v)
{
	vec3f_set(dst, -v->x, -v->y, -v->z);
}

static inline f32 vec3f_dot(const vec3f_t* v1, const vec3f_t* v2)
{
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

static inline f32 vec3f_len(const vec3f_t* v)
{
	return sqrtf(vec3f_dot(v, v));
}

static inline f32 vec3f_dist(const vec3f_t* v1, const vec3f_t* v2)
{
	vec3f_t tmp;
	vec3f_sub(&tmp, v1, v2);
	return vec3f_len(&tmp);
}

static inline void vec3f_norm(vec3f_t* dst, const vec3f_t* v)
{
	f32 len = vec3f_dot(v, v);
	if (len > 0.f) {
		len = 1.f / sqrtf(len);
		dst->x = v->x * len;
		dst->y = v->y * len;
		dst->z = v->z * len;
	} else {
		vec3f_zero(dst);
	}
}

static inline void vec3f_cross(vec3f_t* dst, const vec3f_t* v1,
			       const vec3f_t* v2)
{
	dst->x = (v1->y * v2->z) - (v1->z * v2->y);
	dst->y = (v1->z * v2->x) - (v1->x * v2->z);
	dst->z = (v1->x * v2->y) - (v1->y * v2->x);
}

#ifdef __cplusplus
}
#endif

#endif
