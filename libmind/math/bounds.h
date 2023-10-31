#pragma once

#include "math/vec3.h"

typedef struct bounds {
	vec3f_t min;
	vec3f_t max;
} bounds_t;

static inline void bounds_zero(bounds_t* b)
{
	b->min = kVec3Zero;
	b->max = kVec3Zero;
}

static inline void bounds_copy(bounds_t* dst, const bounds_t* src)
{
	dst->min = vec3_copy(src->min);
	dst->max = vec3_copy(src->max);
}

static inline vec3f_t bounds_get_center(const bounds_t* b)
{
	vec3f_t center = { 0 };
	center = vec3_sub(b->max, b->min);
	center = vec3_mulf(center, 0.5f);
	center = vec3_add(center, b->min);
	return center;
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
