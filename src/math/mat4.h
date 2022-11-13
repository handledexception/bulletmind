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

#ifndef H_BM_MATH_MAT4
#define H_BM_MATH_MAT4

#include "core/types.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include <math.h>

typedef struct mat4f {
	union {
		struct {
			vec4f_t x, y, z, w;
		};
		vec4f_t rows[4];
		f32 m[4][4];
		f32 elems[16];
	};
} mat4f_t;

static inline void mat4f_zero(struct mat4f* m)
{
	vec4f_zero(&m->x);
	vec4f_zero(&m->y);
	vec4f_zero(&m->z);
	vec4f_zero(&m->w);
}

static inline void mat4f_identity(struct mat4f* m)
{
	mat4f_zero(m);
	m->x.x = 1.f;
	m->y.y = 1.f;
	m->z.z = 1.f;
	m->w.w = 1.f;
}

BM_EXPORT void mat4f_transpose(struct mat4f* dst, const struct mat4f* m);
BM_EXPORT void mat4f_mul(struct mat4f* dst, const struct mat4f* lhs,
			 const struct mat4f* rhs);
BM_EXPORT void mat4f_translate_v3(struct mat4f* dst, const vec3f_t* v);
BM_EXPORT void mat4f_translate(struct mat4f* dst, const vec4f_t* v);
BM_EXPORT void mat4f_scale_v3(struct mat4f* dst, const vec3f_t* v);
BM_EXPORT void mat4f_scale(struct mat4f* dst, const vec4f_t* v);
BM_EXPORT void mat4f_ortho_lh(struct mat4f* dst, f32 width, f32 height,
			      f32 z_near, f32 z_far);
BM_EXPORT void mat4f_perspective_fov_lh(struct mat4f* dst, f32 fov, f32 aspect,
					f32 z_near, f32 z_far);
BM_EXPORT void mat4f_look_at_lh(struct mat4f* dst, const struct vec3f* eye,
				const struct vec3f* dir,
				const struct vec3f* up);
BM_EXPORT void mat4f_look_at_rh(struct mat4f* dst, const struct vec3f* eye,
				const struct vec3f* dir,
				const struct vec3f* up);

#endif
