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
#define EPSILON 1e-4f
#define TINY_EPSILON 1e-5f
static inline bool close_float(float f1, float f2, float precision)
{
	return fabsf(f1 - f2) <= precision;
}

#ifdef __cplusplus
}
#endif
