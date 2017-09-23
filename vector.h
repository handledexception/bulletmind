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

typedef struct vec2f {
	float x, y;
} vec2f_t;

typedef struct vec3f {
	float x, y, z;
} vec3f_t;

/*	________________
	
	vec2f operations
	________________
*/

inline void vec2f_set(vec2f_t *dst, float x, float y)
{
	dst->x = x;
	dst->y = y;
}

inline void vec2f_add(vec2f_t *dst, vec2f_t *a, vec2f_t *b)
{
	dst->x = a->x + b->x;
	dst->y = a->y + b->y;
}

inline void vec2f_sub(vec2f_t *dst, vec2f_t *a, vec2f_t *b)
{
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
}

inline void vec2f_equ(vec2f_t *a, vec2f_t *b)
{
	a->x = b->x;
	a->y = b->y;
}

inline void vec2f_addequ(vec2f_t *a, vec2f_t *b)
{
	a->x += b->x;
	a->y += b->y;
}

inline void vec2f_subequ(vec2f_t *a, vec2f_t *b)
{
	a->x -= b->x;
	a->y -= b->y;
}

inline void vec2f_mul(vec2f_t *a, vec2f_t *b)
{
	a->x *= b->x;
	a->y *= b->y;
}

inline void vec2f_sqr(vec2f_t *a)
{
	a->x *= a->x;
	a->y *= a->y;
}

/*	________________
	
	float operations
	________________
*/

// add float to vec2f
inline void vec2f_addf(vec2f_t *a, float s)
{
	a->x += s;
	a->y += s;
}

// subtract float from vec2f
inline void vec2f_subf(vec2f_t *a, float s)
{
	a->x -= s;
	a->y -= s;
}

// scales a vec2f by a float
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