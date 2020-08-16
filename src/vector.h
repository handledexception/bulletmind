#ifndef _H_VECTOR
#define _H_VECTOR

#include "c99defs.h"
#include <math.h>

typedef struct {
    int32_t x, y;
    int32_t w, h;
} rect_t;

typedef struct vec2i {
    int32_t x;
    int32_t y;
} vec2i_t;

typedef struct vec2f {
    float x;
    float y;
} vec2f_t;

typedef struct vec3f {
    float x;
    float y;
    float z;
} vec3f_t;

typedef struct vec4f {
    float x;
    float y;
    float z;
    float w;
} vec4f_t;

typedef struct rgba {
    float r;
    float g;
    float b;
    float a;
} rgba_t;

static inline void vec2f_set(vec2f_t* dst, vec2f_t* value) {
    dst->x = value->x;
    dst->y = value->y;
}

static inline void vec2f_setf(vec2f_t* dst, float x, float y) {
    dst->x = x;
    dst->y = y;
}

static inline void vec2f_add(vec2f_t* dst, vec2f_t* a, vec2f_t* b) {
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
}

static inline void vec2f_sub(vec2f_t* dst, vec2f_t* a, vec2f_t* b) {
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
}

static inline void vec2f_equ(vec2f_t* a, vec2f_t* b) {
    a->x = b->x;
    a->y = b->y;
}

static inline void vec2f_addequ(vec2f_t* a, vec2f_t* b) {
    a->x += b->x;
    a->y += b->y;
}

static inline void vec2f_subequ(vec2f_t* a, vec2f_t* b) {
    a->x -= b->x;
    a->y -= b->y;
}

static inline void vec2f_mul(vec2f_t* a, vec2f_t* b) {
    a->x *= b->x;
    a->y *= b->y;
}

static inline void vec2f_sqr(vec2f_t* a) {
    a->x *= a->x;
    a->y *= a->y;
}

static inline void vec2f_fabsf(vec2f_t* a) {
    a->x = fabsf(a->x);
    a->y = fabsf(a->y);
}

// add float to vec2f
static inline void vec2f_addf(vec2f_t* a, float s) {
    a->x += s;
    a->y += s;
}

// subtract float from vec2f
static inline void vec2f_subf(vec2f_t* a, float s) {
    a->x -= s;
    a->y -= s;
}

// scales a vec2f by a float
static inline void vec2f_scale(vec2f_t* a, float s) {
    a->x *= s;
    a->y *= s;
}

static inline float vec2f_sqrlen(vec2f_t* a) {
    return (a->x * a->x + a->y * a->y);
}

// return the length of vector
static inline float vec2f_length(vec2f_t* a) {
    return sqrtf(vec2f_sqrlen(a));
}

// produce a unit vector
static inline float vec2f_norm(vec2f_t* a) {
    float length = 1.0f / vec2f_length(a);
    a->x *= length;
    a->y *= length;
    return length;
}

// return the dot product of vectors a and b
static inline float vec2f_dot(vec2f_t* a, vec2f_t* b) {
    return (a->x * b->x) + (a->y * b->y);
}

// vector friction
static inline void vec2f_friction(vec2f_t* a, float friction) {
    float speed = vec2f_length(a);
    float newspeed = speed - (speed * friction);

    if(newspeed > 0)
        newspeed /= speed;
    else
        newspeed = 0;

    vec2f_scale(a, newspeed);
}

#endif
