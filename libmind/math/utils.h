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

#include "core/types.h"

#include <math.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

#ifndef M_2PI
#define M_2PI (2.f * M_PI)
#endif

#define RAD_TO_DEG(rad) (180.f / M_PI) * rad
#define DEG_TO_RAD(deg) (M_PI / 180.f) * deg

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LARGE_EPSILON 1e-2f
#define MED_EPSILON 1e-6f
#define EPSILON 1e-4f

#define TINY_EPSILON 1e-5f
#define M_INFINITE 3.4e38f

static int random_initialized = false;
static inline double random64(double a, double b)
{
	if (!random_initialized) {
		srand((unsigned int)time(0));
		random_initialized = true;
	}
	double random = (double)rand() / (double)RAND_MAX;
	double diff = b - a;
	double r = random * diff;
	return a + r;
}

static inline bool f32_compare(float f1, float f2, float epsilon)
{
	return fabsf(f1 - f2) <= epsilon;
}

/*
 *	f32 pos = 2.f;
 *	f32 t;
 *	int steps = 10;
 *	for (int i = 0; i < (steps+1); i++) {
 *		t = (f32)i / (float)steps;
 *		printf("t = %f, lerp = %f\n", t, lerp(1.f, 100.f, t));
 *	}
 * > t = 0.000000, lerp = 1.000000
 * > t = 0.100000, lerp = 10.900001
 * > t = 0.200000, lerp = 20.800001
 * > t = 0.300000, lerp = 30.700001
 * > t = 0.400000, lerp = 40.600002
 * > t = 0.500000, lerp = 50.500000
 * > t = 0.600000, lerp = 60.400002
 * > t = 0.700000, lerp = 70.299995
 * > t = 0.800000, lerp = 80.200005
 * > t = 0.900000, lerp = 90.099998
 * > t = 1.000000, lerp = 100.000000
 */
static inline f32 lerp(f32 a, f32 b, f32 t)
{
	return a + (b - a) * t;
}

static inline f64 lerp64(f64 a, f64 b, f64 t)
{
	return a + (b - a) * t;
}

static inline f32 clampf(f32 val, f32 min_val, f32 max_val)
{
	return val > max_val ? max_val : (val < min_val ? min_val : val);
}

static inline f32 map_range(f32 input_start, f32 input_end, f32 output_start,
			    f32 output_end, f32 val)
{
	f32 slope = (output_end - output_start) / (input_end - input_start);
	return (output_start + (slope * (val - input_start)));
}

#ifdef __cplusplus
}
#endif
