/*
 * Portions of this code adapted or borrowed from Open Broadcaster (OBS).
 *
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
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

#ifndef H_BM_MATH_BOUNDS
#define H_BM_MATH_BOUNDS

#include "math/vec3.h"

struct bounds {
	struct vec3f min;
	struct vec3f max;
};

typedef struct bounds bounds_t;

static inline void bounds_zero(bounds_t* b)
{
	vec3f_zero(&b->min);
	vec3f_zero(&b->max);
}

static inline void bounds_copy(bounds_t* dst, const bounds_t* src)
{
	vec3f_copy(&dst->min, &src->min);
	vec3f_copy(&dst->max, &src->max);
}

static inline void bounds_get_center(vec3f_t* center, const bounds_t* b)
{
	vec3f_sub(center, &b->max, &b->min);
	vec3f_mulf(center, center, 0.5f);
	vec3f_add(center, center, &b->min);
}

static inline bool bounds_intersects(const struct bounds* b1,
				     const struct bounds* b2, f32 epsilon)
{
	return ((b1->min.x - b2->max.x) <= epsilon) &&
	       ((b2->min.x - b1->max.x) <= epsilon) &&
	       ((b1->min.y - b2->max.y) <= epsilon) &&
	       ((b2->min.y - b1->max.y) <= epsilon) &&
	       ((b1->min.z - b2->max.z) <= epsilon) &&
	       ((b2->min.z - b1->max.z) <= epsilon);
}

static inline void bounds_around_centerpoint(bounds_t* dst,
					     const vec3f_t* centerpoint,
					     const vec3f_t* size)
{
}

#endif
