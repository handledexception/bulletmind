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
#include "math/utils.h"
#include <math.h>
#include <stdarg.h>

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

static inline mat4f_t mat4_zero()
{
	mat4f_t m = { 0 };
	m.x = vec4_zero();
	m.y = vec4_zero();
	m.z = vec4_zero();
	m.w = vec4_zero();
	return m;
}

static inline mat4f_t mat4_diag(f32 val)
{
	mat4f_t m = mat4_zero();
	m.elems[0 + 0 * 4] = val;
	m.elems[1 + 1 * 4] = val;
	m.elems[2 + 2 * 4] = val;
	m.elems[3 + 3 * 4] = val;
	return m;
}

static inline mat4f_t mat4_identity()
{
	mat4f_t m = mat4_diag(1.0f);
	return m;
}

static inline mat4f_t mat4_mul(mat4f_t a, mat4f_t b)
{
	mat4f_t m = mat4_zero();
	for (u32 y = 0; y < 4; ++y) {
		for (u32 x = 0; x < 4; ++x) {
			f32 sum = 0.0f;
			for (u32 e = 0; e < 4; ++e) {
				sum += a.elems[x + e * 4] * b.elems[e + y * 4];
			}
			m.elems[x + y * 4] = sum;
		}
	}

	return m;
}

static inline mat4f_t mat4_mul_list(u32 count, ...)
{
    va_list ap;
    mat4f_t m = mat4_identity();
    va_start(ap, count);
    for (u32 i = 0; i < count; ++i) {
        m = mat4_mul(m, va_arg(ap, mat4f_t));
    }
    va_end(ap);
    return m;
}

static inline mat4f_t mat4_transpose(const mat4f_t m)
{
	mat4f_t trx = mat4_identity();
	// First row
	trx.elems[0 * 4 + 0] = m.elems[0 * 4 + 0];
	trx.elems[1 * 4 + 0] = m.elems[0 * 4 + 1];
	trx.elems[2 * 4 + 0] = m.elems[0 * 4 + 2];
	trx.elems[3 * 4 + 0] = m.elems[0 * 4 + 3];

	// Second row
	trx.elems[0 * 4 + 1] = m.elems[1 * 4 + 0];
	trx.elems[1 * 4 + 1] = m.elems[1 * 4 + 1];
	trx.elems[2 * 4 + 1] = m.elems[1 * 4 + 2];
	trx.elems[3 * 4 + 1] = m.elems[1 * 4 + 3];

	// Third row
	trx.elems[0 * 4 + 2] = m.elems[2 * 4 + 0];
	trx.elems[1 * 4 + 2] = m.elems[2 * 4 + 1];
	trx.elems[2 * 4 + 2] = m.elems[2 * 4 + 2];
	trx.elems[3 * 4 + 2] = m.elems[2 * 4 + 3];

	// Fourth row
	trx.elems[0 * 4 + 3] = m.elems[3 * 4 + 0];
	trx.elems[1 * 4 + 3] = m.elems[3 * 4 + 1];
	trx.elems[2 * 4 + 3] = m.elems[3 * 4 + 2];
	trx.elems[3 * 4 + 3] = m.elems[3 * 4 + 3];

	return trx;
}

static inline mat4f_t mat4_inverse(mat4f_t m)
{
	mat4f_t res = mat4_identity();

	f32 temp[16];

	temp[0] = m.elems[5] * m.elems[10] * m.elems[15] -
		m.elems[5] * m.elems[11] * m.elems[14] -
		m.elems[9] * m.elems[6] * m.elems[15] +
		m.elems[9] * m.elems[7] * m.elems[14] +
		m.elems[13] * m.elems[6] * m.elems[11] -
		m.elems[13] * m.elems[7] * m.elems[10];

	temp[4] = -m.elems[4] * m.elems[10] * m.elems[15] +
		m.elems[4] * m.elems[11] * m.elems[14] +
		m.elems[8] * m.elems[6] * m.elems[15] -
		m.elems[8] * m.elems[7] * m.elems[14] -
		m.elems[12] * m.elems[6] * m.elems[11] +
		m.elems[12] * m.elems[7] * m.elems[10];

	temp[8] = m.elems[4] * m.elems[9] * m.elems[15] -
		m.elems[4] * m.elems[11] * m.elems[13] -
		m.elems[8] * m.elems[5] * m.elems[15] +
		m.elems[8] * m.elems[7] * m.elems[13] +
		m.elems[12] * m.elems[5] * m.elems[11] -
		m.elems[12] * m.elems[7] * m.elems[9];

	temp[12] = -m.elems[4] * m.elems[9] * m.elems[14] +
		m.elems[4] * m.elems[10] * m.elems[13] +
		m.elems[8] * m.elems[5] * m.elems[14] -
		m.elems[8] * m.elems[6] * m.elems[13] -
		m.elems[12] * m.elems[5] * m.elems[10] +
		m.elems[12] * m.elems[6] * m.elems[9];

	temp[1] = -m.elems[1] * m.elems[10] * m.elems[15] +
		m.elems[1] * m.elems[11] * m.elems[14] +
		m.elems[9] * m.elems[2] * m.elems[15] -
		m.elems[9] * m.elems[3] * m.elems[14] -
		m.elems[13] * m.elems[2] * m.elems[11] +
		m.elems[13] * m.elems[3] * m.elems[10];

	temp[5] = m.elems[0] * m.elems[10] * m.elems[15] -
		m.elems[0] * m.elems[11] * m.elems[14] -
		m.elems[8] * m.elems[2] * m.elems[15] +
		m.elems[8] * m.elems[3] * m.elems[14] +
		m.elems[12] * m.elems[2] * m.elems[11] -
		m.elems[12] * m.elems[3] * m.elems[10];

	temp[9] = -m.elems[0] * m.elems[9] * m.elems[15] +
		m.elems[0] * m.elems[11] * m.elems[13] +
		m.elems[8] * m.elems[1] * m.elems[15] -
		m.elems[8] * m.elems[3] * m.elems[13] -
		m.elems[12] * m.elems[1] * m.elems[11] +
		m.elems[12] * m.elems[3] * m.elems[9];

	temp[13] = m.elems[0] * m.elems[9] * m.elems[14] -
		m.elems[0] * m.elems[10] * m.elems[13] -
		m.elems[8] * m.elems[1] * m.elems[14] +
		m.elems[8] * m.elems[2] * m.elems[13] +
		m.elems[12] * m.elems[1] * m.elems[10] -
		m.elems[12] * m.elems[2] * m.elems[9];

	temp[2] = m.elems[1] * m.elems[6] * m.elems[15] -
		m.elems[1] * m.elems[7] * m.elems[14] -
		m.elems[5] * m.elems[2] * m.elems[15] +
		m.elems[5] * m.elems[3] * m.elems[14] +
		m.elems[13] * m.elems[2] * m.elems[7] -
		m.elems[13] * m.elems[3] * m.elems[6];

	temp[6] = -m.elems[0] * m.elems[6] * m.elems[15] +
		m.elems[0] * m.elems[7] * m.elems[14] +
		m.elems[4] * m.elems[2] * m.elems[15] -
		m.elems[4] * m.elems[3] * m.elems[14] -
		m.elems[12] * m.elems[2] * m.elems[7] +
		m.elems[12] * m.elems[3] * m.elems[6];

	temp[10] = m.elems[0] * m.elems[5] * m.elems[15] -
		m.elems[0] * m.elems[7] * m.elems[13] -
		m.elems[4] * m.elems[1] * m.elems[15] +
		m.elems[4] * m.elems[3] * m.elems[13] +
		m.elems[12] * m.elems[1] * m.elems[7] -
		m.elems[12] * m.elems[3] * m.elems[5];

	temp[14] = -m.elems[0] * m.elems[5] * m.elems[14] +
		m.elems[0] * m.elems[6] * m.elems[13] +
		m.elems[4] * m.elems[1] * m.elems[14] -
		m.elems[4] * m.elems[2] * m.elems[13] -
		m.elems[12] * m.elems[1] * m.elems[6] +
		m.elems[12] * m.elems[2] * m.elems[5];

	temp[3] = -m.elems[1] * m.elems[6] * m.elems[11] +
		m.elems[1] * m.elems[7] * m.elems[10] +
		m.elems[5] * m.elems[2] * m.elems[11] -
		m.elems[5] * m.elems[3] * m.elems[10] -
		m.elems[9] * m.elems[2] * m.elems[7] +
		m.elems[9] * m.elems[3] * m.elems[6];

	temp[7] = m.elems[0] * m.elems[6] * m.elems[11] -
		m.elems[0] * m.elems[7] * m.elems[10] -
		m.elems[4] * m.elems[2] * m.elems[11] +
		m.elems[4] * m.elems[3] * m.elems[10] +
		m.elems[8] * m.elems[2] * m.elems[7] -
		m.elems[8] * m.elems[3] * m.elems[6];

	temp[11] = -m.elems[0] * m.elems[5] * m.elems[11] +
		m.elems[0] * m.elems[7] * m.elems[9] +
		m.elems[4] * m.elems[1] * m.elems[11] -
		m.elems[4] * m.elems[3] * m.elems[9] -
		m.elems[8] * m.elems[1] * m.elems[7] +
		m.elems[8] * m.elems[3] * m.elems[5];

	temp[15] = m.elems[0] * m.elems[5] * m.elems[10] -
		m.elems[0] * m.elems[6] * m.elems[9] -
		m.elems[4] * m.elems[1] * m.elems[10] +
		m.elems[4] * m.elems[2] * m.elems[9] +
		m.elems[8] * m.elems[1] * m.elems[6] -
		m.elems[8] * m.elems[2] * m.elems[5];

	float determinant = m.elems[0] * temp[0] + m.elems[1] * temp[4] + m.elems[2] * temp[8] + m.elems[3] * temp[12];
	determinant = 1.0f / determinant;

	for (int i = 0; i < 4 * 4; i++)
		res.elems[i] = (float)(temp[i] * (float)determinant);

	return res;
}

static inline mat4f_t mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
	mat4f_t m = mat4_identity();

	// Main diagonal
	m.elems[0 + 0 * 4] = 2.0f / (right - left);
	m.elems[1 + 1 * 4] = 2.0f / (top - bottom);
	m.elems[2 + 2 * 4] = -2.0f / (far - near);

	// Last column
	m.elems[0 + 3 * 4] = -(right + left) / (right - left);
	m.elems[1 + 3 * 4] = -(top + bottom) / (top - bottom);
	m.elems[2 + 3 * 4] = -(far + near) / (far - near);

	return m;
}

static inline mat4f_t mat4_ortho_norm(const mat4f_t* m)
{
    mat4f_t r = *m;
    r.x = vec4_norm(r.x);
    r.y = vec4_norm(r.y);
    r.z = vec4_norm(r.z);
    return r;
}

static inline mat4f_t mat4_persp(f32 fov, f32 aspect, f32 near, f32 far)
{
	mat4f_t m = mat4_zero();

	f32 q = 1.0f / (f32)tan(DEG_TO_RAD(0.5f * fov));
	f32 a = q / aspect;
	f32 b = (near + far) / (near - far);
	f32 c = (2.0f * near * far) / (near - far);

	m.elems[0 + 0 * 4] = a;
	m.elems[1 + 1 * 4] = q;
	m.elems[2 + 2 * 4] = b;
	m.elems[2 + 3 * 4] = c;
	m.elems[3 + 2 * 4] = -1.0f;

	return m;
}

static inline mat4f_t mat4_translate(vec3f_t v)
{
	mat4f_t m = mat4_identity();
	m.elems[0 + 4 * 3] = v.x;
	m.elems[1 + 4 * 3] = v.y;
	m.elems[2 + 4 * 3] = v.z;
	return m;
}

static inline mat4f_t mat4_scale(vec3f_t v)
{
	mat4f_t m = mat4_identity();
	m.elems[0 + 0 * 4] = v.x;
	m.elems[1 + 1 * 4] = v.y;
	m.elems[2 + 2 * 4] = v.z;
	return m;
}

static inline mat4f_t mat4_rotate(f32 angle, vec3f_t axis)
{
	mat4f_t m = mat4_identity();
	f32 a = angle;
	f32 c = (f32)cos(a);
	f32 s = (f32)sin(a);

	vec3f_t naxis = vec3_norm(axis);
	f32 x = naxis.x;
	f32 y = naxis.y;
	f32 z = naxis.z;

	//First column
	m.elems[0 + 0 * 4] = x * x * (1 - c) + c;
	m.elems[1 + 0 * 4] = x * y * (1 - c) + z * s;
	m.elems[2 + 0 * 4] = x * z * (1 - c) - y * s;

	//Second column
	m.elems[0 + 1 * 4] = x * y * (1 - c) - z * s;
	m.elems[1 + 1 * 4] = y * y * (1 - c) + c;
	m.elems[2 + 1 * 4] = y * z * (1 - c) + x * s;

	//Third column
	m.elems[0 + 2 * 4] = x * z * (1 - c) + y * s;
	m.elems[1 + 2 * 4] = y * z * (1 - c) - x * s;
	m.elems[2 + 2 * 4] = z * z * (1 - c) + c;

	return m;
}

static inline mat4f_t mat4_lookat(vec3f_t eye, vec3f_t dir, vec3f_t up)
{
    vec3f_t f = vec3_norm(vec3_sub(dir, eye));
    vec3f_t s = vec3_norm(vec3_cross(f, up));
    vec3f_t u = vec3_cross(s, f);

    mat4f_t m = mat4_identity();
    m.elems[0 * 4 + 0] = s.x;
    m.elems[1 * 4 + 0] = s.y;
    m.elems[2 * 4 + 0] = s.z;

    m.elems[0 * 4 + 1] = u.x;
    m.elems[1 * 4 + 1] = u.y;
    m.elems[2 * 4 + 1] = u.z;

    m.elems[0 * 4 + 2] = -f.x;
    m.elems[1 * 4 + 2] = -f.y;
    m.elems[2 * 4 + 2] = -f.z;

    m.elems[3 * 4 + 0] = -vec3_dot(s, eye);;
    m.elems[3 * 4 + 1] = -vec3_dot(u, eye);
    m.elems[3 * 4 + 2] = vec3_dot(f, eye);

    return m;
}

// Modified from https://github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp

static inline void mat4_decompose(const mat4f_t* m, f32* translation, f32* rotation, f32* scale)
{
    mat4f_t mat = *m;

    scale[0] = vec4_len(mat.x);
    scale[1] = vec4_len(mat.y);
    scale[2] = vec4_len(mat.z);

    mat = mat4_ortho_norm(&mat);

    rotation[0] = DEG_TO_RAD(atan2f(mat.m[1][2], mat.m[2][2]));
    rotation[1] = DEG_TO_RAD(atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[1][2] +
                mat.m[2][2] * mat.m[2][2])));
    rotation[2] = DEG_TO_RAD(atan2f(mat.m[0][1], mat.m[0][0]));

    translation[0] = mat.w.x;
    translation[1] = mat.w.y;
    translation[2] = mat.w.z;
}

// Modified from github.com/CedricGuillemet/ImGuizmo/blob/master/ImGuizmo.cpp

static inline mat4f_t mat4_recompose(const f32* translation, const f32* rotation, const f32* scale)
{
    mat4f_t mat = mat4_identity();

    vec3f_t direction_unary[3] = {
        vec3_set(1.0f, 0.0f, 0.0f),
        vec3_set(0.0f, 1.0f, 0.0f),
        vec3_set(0.0f, 0.0f, 1.0f),
    };

    mat4f_t rot[3] = {mat4_identity(), mat4_identity(), mat4_identity()};
    for (uint32_t i = 0; i < 3; ++i) {
        rot[i] = mat4_rotate(DEG_TO_RAD(rotation[i]), direction_unary[i]);
    }

    mat = mat4_mul_list(3, rot[2], rot[1], rot[0]);

    float valid_scale[3] = { 0 };
    for (uint32_t i = 0; i < 3; ++i) {
        valid_scale[i] = fabsf(scale[i]) < MED_EPSILON ? 0.001f : scale[i];
    }

    mat.x = vec4_mulf(mat.x, valid_scale[0]);
    mat.y = vec4_mulf(mat.y, valid_scale[1]);
    mat.z = vec4_mulf(mat.z, valid_scale[2]);
    mat.w = vec4_set(translation[0], translation[1], translation[2], 1.f);

    return mat;
}

static inline vec4f_t mat4_mul_vec4(mat4f_t m, vec4f_t v)
{
    return vec4_set(
        m.elems[0 + 4 * 0] * v.x + m.elems[0 + 4 * 1] * v.y + m.elems[0 + 4 * 2] * v.z + m.elems[0 + 4 * 3] * v.w,
        m.elems[1 + 4 * 0] * v.x + m.elems[1 + 4 * 1] * v.y + m.elems[1 + 4 * 2] * v.z + m.elems[1 + 4 * 3] * v.w,
        m.elems[2 + 4 * 0] * v.x + m.elems[2 + 4 * 1] * v.y + m.elems[2 + 4 * 2] * v.z + m.elems[2 + 4 * 3] * v.w,
        m.elems[3 + 4 * 0] * v.x + m.elems[3 + 4 * 1] * v.y + m.elems[3 + 4 * 2] * v.z + m.elems[3 + 4 * 3] * v.w
    );
}

#endif
