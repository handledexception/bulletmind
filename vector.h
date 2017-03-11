#ifndef H_VECTOR
#define H_VECTOR

#include <math.h>

typedef union {
	float x, y;
} vec2f;

typedef union {
	float x, y, z;
} vec3f;

void vec2f_equ(vec2f *a, vec2f *b)
{
	a->x = b->x;
	a->y = b->y;
}

void vec2f_add(vec2f *a, vec2f *b)
{
	a->x += b->x;
	a->y += b->y;
}

void vec2f_sub(vec2f *a, vec2f *b)
{
	a->x -= b->x;
	a->y -= b->y;
}

void vec2f_scale(vec2f *a, float s)
{
	a->x *= s;
	a->y *= s;
}

float vec2f_sqrlen(vec2f *a)
{
	return (a->x * a->x + a->y * a->y);
}

// return the length of vector
float vec2f_length(vec2f *a)
{
	return sqrtf(vec2f_sqrlen(a));
}

// produce a unit vector
float vec2f_norm(vec2f *a)
{
	float length = 1.0f / vec2f_sqrlen(a);
	a->x *= length;
	a->y *= length;
	return length;
}

// return the dot product of vectors a and b
float vec2f_dot(vec2f *a, vec2f *b)
{
	return (a->x * b->x + a->y * b->y);
}

#endif