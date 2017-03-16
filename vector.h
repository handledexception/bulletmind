#ifndef H_VECTOR
#define H_VECTOR

#include "c99defs.h"
#include <math.h>

typedef struct {
	int32_t x, y;
	int32_t w, h;
} recti32_t;

typedef struct {
	float x, y;
	float w, h;
} rectf_t;

typedef union vec2f {
	float x, y;
} vec2f_t;

typedef union vec3f {
	float x, y, z;
} vec3f_t;

inline void vec2f_equ(vec2f_t *a, vec2f_t *b)
{
	a->x = b->x;
	a->y = b->y;
}

inline void vec2f_add(vec2f_t *a, vec2f_t *b)
{
	a->x += b->x;
	a->y += b->y;
}

inline void vec2f_sub(vec2f_t *a, vec2f_t *b)
{
	a->x -= b->x;
	a->y -= b->y;
}

inline void vec2f_scale(vec2f_t *a, float s)
{
	a->x *= s;
	a->y *= s;
}

inline float vec2f_sqrlen(vec2f_t *a)
{
	return (a->x * a->x + a->y * a->y);
}

// return the length of vector
inline float vec2f_length(vec2f_t *a)
{
	return sqrtf(vec2f_sqrlen(a));
}

// produce a unit vector
inline float vec2f_norm(vec2f_t *a)
{
	float length = 1.0f / vec2f_sqrlen(a);
	a->x *= length;
	a->y *= length;
	return length;
}

// return the dot product of vectors a and b
inline float vec2f_dot(vec2f_t *a, vec2f_t *b)
{
	return (a->x * b->x + a->y * b->y);
}

#endif