#ifndef H_BM_MATH_QUAT
#define H_BM_MATH_QUAT

#include "math/vec4.h"

// typedef struct quat {
//     union {
//         struct {
//             f32 x, y, z, w;
//         };
//         f32 elems[4];
//     };
// } quat_t;

typedef vec4f_t quat_t;

static inline void quat_identity(quat_t* q)
{
    vec4f_zero((struct vec4f*)q);
    q->w = 1.0f;
}

static inline void quat_set(quat_t* dst, f32 x, f32 y, f32 z, f32 w)
{
    dst->x = x;
    dst->y = y;
    dst->z = z;
    dst->w = w;
}

static inline void vec3f_from_quat(vec3f_t* dst, const quat_t* q)
{
    dst->x = q->x;
    dst->y = q->y;
    dst->z = q->z;
}

static inline void quat_set_vec4f(quat_t* dst, const vec4f_t* v)
{
    quat_set(dst, v->x, v->y, v->z, v->w);
}

static inline void quat_copy(quat_t* dst, const quat_t* q)
{
    dst->x = q->x;
    dst->y = q->y;
    dst->z = q->z;
    dst->w = q->w;
}

static inline void quat_add(quat_t* dst, const quat_t* q1, const quat_t* q2)
{
    dst->x = q1->x + q2->x;
    dst->y = q1->y + q2->y;
    dst->z = q1->z + q2->z;
    dst->w = q1->w + q2->w;
}

static inline void quat_sub(quat_t* dst, const quat_t* q1, const quat_t* q2)
{
    dst->x = q1->x - q2->x;
    dst->y = q1->y - q2->y;
    dst->z = q1->z - q2->z;
    dst->w = q1->w - q2->w;
}

static inline void quat_mul(quat_t* dst, const quat_t* q1, const quat_t* q2)
{
	vec3f_t q1axis, q2axis;
	vec3f_t temp1, temp2;

	vec3f_from_quat(&q1axis, q1);
	vec3f_from_quat(&q2axis, q2);

	vec3f_mulf(&temp1, &q2axis, q1->w);
	vec3f_mulf(&temp2, &q1axis, q2->w);
	vec3f_add(&temp1, &temp1, &temp2);
	vec3f_cross(&temp2, &q1axis, &q2axis);
	vec3f_add((struct vec3f_t *)dst, &temp1, &temp2);

	dst->w = (q1->w * q2->w) - vec3f_dot(&q1axis, &q2axis);
}

static inline f32 quat_dot(const quat_t* q1, const quat_t* q2)
{
    return vec4f_dot((vec4f_t*)q1, (vec4f_t*)q2);
}

static inline void quat_inv(quat_t* dst, const quat_t* q)
{
    dst->x = -q->x;
    dst->y = -q->y;
    dst->z = -q->z;
}

static inline void quat_neg(quat_t* dst, const quat_t* q)
{
    quat_inv(dst, q);
    dst->w = -q->w;
}

static inline f32 quat_len(const quat_t* q)
{
    f32 dot = quat_dot(q, q);
    return (dot > 0.0f) ? sqrtf(dot) : 0.0f;
}

static inline f32 quat_dist(const quat_t* q1, const quat_t* q2)
{
    quat_t temp;
    f32 dot;
    quat_sub(&temp, q1, q2);
    return quat_len(&temp);
}

static inline void quat_norm(quat_t* dst, const quat_t* q)
{
    f32 dot = quat_dot(q, q);
    if (dot > 0.0f) {
        f32 dot_inv_sqrt = 1.0f / sqrtf(dot);
        dst->x = q->x * dot_inv_sqrt;
        dst->y = q->y * dot_inv_sqrt;
        dst->z = q->z * dot_inv_sqrt;
        dst->w = q->w * dot_inv_sqrt;
    } else {
        vec4f_zero((vec4f_t*)dst);
    }
}

static inline void quat_addf(quat_t* dst, const quat_t* q, f32 f)
{
    dst->x = q->x + f;
    dst->y = q->y + f;
    dst->z = q->z + f;
    dst->w = q->w + f;
}

static inline void quat_subf(quat_t* dst, const quat_t* q, f32 f)
{
    dst->x = q->x - f;
    dst->y = q->y - f;
    dst->z = q->z - f;
    dst->w = q->w - f;
}

static inline void quat_mulf(quat_t* dst, const quat_t* q, f32 f)
{
    dst->x = q->x * f;
    dst->y = q->y * f;
    dst->z = q->z * f;
    dst->w = q->w * f;
}

static inline void quat_divf(quat_t* dst, const quat_t* q, f32 f)
{
    dst->x = q->x / f;
    dst->y = q->y / f;
    dst->z = q->z / f;
    dst->w = q->w / f;
}

#endif // H_BM_MATH_QUAT
