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

static const vec3f_t kVec3Zero		= {  0.0f,  0.0f,  0.0f };
static const vec3f_t kVec3Left		= { -1.0f,  0.0f,  0.0f };
static const vec3f_t kVec3Right		= {  1.0f,  0.0f,  0.0f };
static const vec3f_t kVec3Up		= {  0.0f,  1.0f,  0.0f };
static const vec3f_t kVec3Down		= {  0.0f, -1.0f,  0.0f };
static const vec3f_t kVec3Forward	= {  0.0f,  0.0f,  1.0f };
static const vec3f_t kVec3Backward	= {  0.0f,  0.0f, -1.0f };

static inline vec3f_t vec3_default()
{
	vec3f_t v = { 0 };
	v.x = 0.0f;
	v.y = 0.0f;
	v.z = 0.0f;
	return v;
}

static inline vec3f_t vec3_set(f32 x, f32 y, f32 z)
{
	vec3f_t v = { 0 };
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

static inline vec3f_t vec3_copy(const vec3f_t v)
{
	vec3f_t res = { 0 };
	res.x = v.x;
	res.y = v.y;
	res.z = v.z;
	return res;
}

static inline bool vec3_eq(vec3f_t a, vec3f_t b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

static inline vec3f_t vec3_add(vec3f_t a, vec3f_t b)
{
	return vec3_set(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline vec3f_t vec3_sub(vec3f_t a, vec3f_t b)
{
	return vec3_set(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline vec3f_t vec3_mul(vec3f_t a, vec3f_t b)
{
	return vec3_set(a.x * b.x, a.y * b.y, a.z * b.z);
}

static inline vec3f_t vec3_div(vec3f_t a, vec3f_t b)
{
	return vec3_set(a.x / b.x, a.y / b.y, a.z / b.z);
}

static inline vec3f_t vec3_mulf(vec3f_t v, f32 s)
{
	return vec3_set(v.x * s, v.y * s, v.z * s);
}

static inline vec3f_t vec3_neg(vec3f_t v)
{
	return vec3_set(-v.x, -v.y, -v.z);
}

static inline f32 vec3_dot(vec3f_t a, vec3f_t b)
{
	return (f32)((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

static inline f32 vec3_len(vec3f_t v)
{
	return (f32)sqrt(vec3_dot(v, v));
}

static inline f32 vec3_sqrlen(vec3f_t v)
{
	return (f32)(vec3_dot(v, v));
}

static inline f32 vec3_sqrdist(vec3f_t a, vec3f_t b)
{
	f32 dx = (a.x - b.x);
	f32 dy = (a.y - b.y);
	f32 dz = (a.z - b.z);
	return (dx * dx + dy * dy + dz * dz);
}

static inline f32 vec3_dist(vec3f_t a, vec3f_t b)
{
	return (f32)sqrt(vec3_sqrdist(a, b));
}

static inline vec3f_t vec3_norm(vec3f_t v)
{
	f32 len = vec3_len(v);
	return len == 0.0f ? v : vec3_mulf(v, 1.0f / len);
}

static inline vec3f_t vec3_cross(vec3f_t a, vec3f_t b)
{
	return vec3_set(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

static inline f32 vec3_angle_between(vec3f_t a, vec3f_t b)
{
	return (f32)acosf(vec3_dot(a, b));
}

static inline f32 vec3_angle_between_signed(vec3f_t a, vec3f_t b)
{
	return (f32)asinf(vec3_len(vec3_cross(a, b)));
}


static inline void vec3f_set(struct vec3f* dst, f32 x, f32 y, f32 z)
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
}

static inline void vec3f_zero(struct vec3f* dst)
{
	vec3f_set(dst, 0.f, 0.f, 0.f);
}

static inline void vec3f_copy(struct vec3f* dst, const struct vec3f* src)
{
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
}

static inline void vec3f_add(struct vec3f* dst, const struct vec3f* lhs,
			     const struct vec3f* rhs)
{
	vec3f_set(dst, lhs->x + rhs->x, lhs->y + rhs->y, lhs->z + rhs->z);
}

static inline void vec3f_sub(struct vec3f* dst, const struct vec3f* lhs,
			     const struct vec3f* rhs)
{
	vec3f_set(dst, lhs->x - rhs->x, lhs->y - rhs->y, lhs->z - rhs->z);
}

static inline void vec3f_mul(struct vec3f* dst, const struct vec3f* lhs,
			     const struct vec3f* rhs)
{
	vec3f_set(dst, lhs->x * rhs->x, lhs->y * rhs->y, lhs->z * rhs->z);
}

static inline void vec3f_div(struct vec3f* dst, const struct vec3f* lhs,
			     const struct vec3f* rhs)
{
	vec3f_set(dst, lhs->x / rhs->x, lhs->y / rhs->y, lhs->z / rhs->z);
}

static inline void vec3f_addf(struct vec3f* dst, const struct vec3f* lhs,
			      f32 rhs)
{
	vec3f_set(dst, lhs->x + rhs, lhs->y + rhs, lhs->z + rhs);
}

static inline void vec3f_subf(struct vec3f* dst, const struct vec3f* lhs,
			      f32 rhs)
{
	vec3f_set(dst, lhs->x - rhs, lhs->y - rhs, lhs->z - rhs);
}

static inline void vec3f_mulf(struct vec3f* dst, struct vec3f* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x * rhs, lhs->y * rhs, lhs->z * rhs);
}

static inline void vec3f_divf(struct vec3f* dst, struct vec3f* lhs, f32 rhs)
{
	vec3f_set(dst, lhs->x / rhs, lhs->y / rhs, lhs->z / rhs);
}

static inline void vec3f_negate(struct vec3f* dst, const struct vec3f* v)
{
	vec3f_set(dst, -v->x, -v->y, -v->z);
	if (dst->x == 0.0f)
		dst->x = 0.0f;
	if (dst->y == 0.0f)
		dst->y = 0.0f;
	if (dst->z == 0.0f)
		dst->z = 0.0f;
}

static inline f32 vec3f_dot(const struct vec3f* v1, const struct vec3f* v2)
{
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

static inline f32 vec3f_len(const struct vec3f* v)
{
	return sqrtf(vec3f_dot(v, v));
}

static inline f32 vec3f_dist(const struct vec3f* v1, const struct vec3f* v2)
{
	struct vec3f tmp;
	vec3f_sub(&tmp, v1, v2);
	return vec3f_len(&tmp);
}

static inline void vec3f_norm(struct vec3f* dst, const struct vec3f* v)
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

static inline void vec3f_cross(struct vec3f* dst, const struct vec3f* v1,
			       const struct vec3f* v2)
{
	dst->x = (v1->y * v2->z) - (v1->z * v2->y);
	dst->y = (v1->z * v2->x) - (v1->x * v2->z);
	dst->z = (v1->x * v2->y) - (v1->y * v2->x);
}

#ifdef __cplusplus
}
#endif

#endif
