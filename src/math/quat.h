#pragma once

#include "math/vec4.h"
#include "math/utils.h"

typedef struct quaternion {
    union {
        struct {
            union {
                vec3f_t xyz;
                vec3f_t axis;
            } axis;
            float a;
        } aa;
        f32 elems[4];
        vec4f_t v;
        struct {
            f32 x, y, z, w;
        };
    };
} quat_t;

static inline quat_t quat_default()
{
    quat_t q = { 0 };
    q.x = 0.0f;
    q.y = 0.0f;
    q.z = 0.0f;
    q.w = 1.0f;
    return q;
}

static inline quat_t quat_set(f32 x, f32 y, f32 z, f32 w)
{
    quat_t q = { 0 };
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}

static inline vec3f_t vec3f_from_quat(const quat_t q)
{
    vec3f_t v = { 0 };
    v.x = q.x;
    v.y = q.y;
    v.z = q.z;
    return v;
}

static inline quat_t quat_from_vec4f(const vec4f_t v)
{
    quat_t q = quat_set(v.x, v.y, v.z, v.w);
    return q;
}

static inline quat_t quat_copy(const quat_t q)
{
    quat_t qc = { 0 };
    qc.x = q.x;
    qc.y = q.y;
    qc.z = q.z;
    qc.w = q.w;
    return qc;
}

static inline quat_t quat_add(quat_t a, quat_t b)
{
    quat_t c = { 0 };
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    c.w = a.w + b.w;
    return c;
}

static inline quat_t quat_sub(quat_t a, quat_t b)
{
    quat_t c = { 0 };
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    c.w = a.w - b.w;
    return c;
}

static inline quat_t quat_mul(quat_t a, quat_t b)
{
    return quat_set(
        a.w * b.x + b.w * a.x + a.y * b.z - b.y * a.z,
        a.w * b.y + b.w * a.y + a.z * b.x - b.z * a.x,
        a.w * b.z + b.w * a.z + a.x * b.y - b.x * a.y,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

static inline quat_t quat_mulf(const quat_t q, f32 f)
{
    return quat_set(q.x * f, q.y * f, q.z * f, q.w * f);
}

static inline f32 quat_dot(const quat_t q1, const quat_t q2)
{
    return (q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w);
}

static inline quat_t quat_conjugate(quat_t q)
{
    return (quat_set(-q.x, -q.y, -q.z, q.w));
}

static inline f32 quat_len(quat_t q)
{
    return (f32)sqrt(quat_dot(q, q));
}

static inline quat_t quat_norm(quat_t q)
{
    return quat_mulf(q, 1.0f / quat_len(q));
}

static inline quat_t quat_cross(quat_t a, quat_t b)
{
    return quat_set(
        a.x * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
        a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

static inline quat_t quat_inverse(quat_t q)
{
    return (quat_mulf(quat_conjugate(q), 1.0f / quat_dot(q, q)));
}

static inline quat_t quat_angle_axis(f32 rad, vec3f_t axis)
{
    // Normalize axis
    vec3f_t a = vec3_norm(axis);

    // Get scalar
    f32 half_angle = 0.5f * rad;
    f32 s = (float)sin(half_angle);

    return quat_set(a.x * s, a.y * s, a.z * s, (float)cos(half_angle));
}

static inline vec3f_t quat_rotate(quat_t q, vec3f_t v)
{
    // nVidia SDK implementation
    vec3f_t qvec = vec3_set(q.x, q.y, q.z);
    vec3f_t uv = vec3_cross(qvec, v);
    vec3f_t uuv = vec3_cross(qvec, uv);
    uv = vec3_mulf(uv, 2.f * q.w);
    uuv = vec3_mulf(uuv, 2.f);
    return (vec3_add(v, vec3_add(uv, uuv)));
}

static inline vec3f_t quat_forward(quat_t q)
{
    return quat_rotate(q, vec3_set(0.f, 0.f, -1.f));
}

static inline vec3f_t quat_backward(quat_t q)
{
    return quat_rotate(q, vec3_set(0.f, 0.f, 1.f));
}

static inline vec3f_t quat_left(quat_t q)
{
    return quat_rotate(q, vec3_set(-1.f, 0.f, 0.f));
}

static inline vec3f_t quat_right(quat_t q)
{
    return quat_rotate(q, vec3_set(1.f, 0.f, 0.f));
}

static inline vec3f_t quat_up(quat_t q)
{
    return quat_rotate(q, vec3_set(0.f, 1.f, 0.f));
}

static inline vec3f_t quat_down(quat_t q)
{
    return quat_rotate(q, vec3_set(0.f, -1.f, 0.f));
}

static inline quat_t quat_from_to_rotation(vec3f_t src, vec3f_t dst)
{
    src = vec3_norm(src);
    dst = vec3_norm(dst);
    const f32 d = vec3_dot(src, dst);

    if (d  >= 1.f) {
        return quat_default();
    } else if (d <= -1.f) {
        // Orthonormalize, find axis of rotation
        vec3f_t axis = vec3_cross(src, kVec3Right);
        if (vec3_sqrlen(axis) < 1e-6) {
            axis = vec3_cross(src, kVec3Up);
        }
        return quat_angle_axis((f32)M_PI, vec3_norm(axis));
    } else {
        const f32 s = sqrtf(vec3_sqrlen(src) * vec3_sqrlen(dst)) + vec3_dot(src, dst);
        vec3f_t axis = vec3_cross(src, dst);
        return quat_norm(quat_set(axis.x, axis.y, axis.z, s));
    }
}

static inline quat_t quat_look_rotation(vec3f_t position, vec3f_t target, vec3f_t up)
{
    const vec3f_t forward = vec3_norm(vec3_sub(position, target));
    const quat_t q0 = quat_from_to_rotation(kVec3Backward, forward);
    if (vec3_sqrlen(vec3_cross(forward, up)) < 1e-6) {
        return q0;
    }

    const vec3f_t new_up = quat_rotate(q0, up);
    const quat_t q1 = quat_from_to_rotation(new_up, up);

    return quat_mul(q1, q0);
}

static inline quat_t quat_slerp(quat_t a, quat_t b, f32 t)
{
    f32 c = quat_dot(a, b);
    quat_t end = b;

    if (c < 0.0f) {
        // Reverse all signs
        c *= -1.0f;
        end.x *= -1.0f;
        end.y *= -1.0f;
        end.z *= -1.0f;
        end.w *= -1.0f;
    }

    // Calculate coefficients
    f32 sclp, sclq;
    if ((1.0f - c) > 0.0001f) {
        f32 omega = (float)acosf(c);
        f32 s = (float)sinf(omega);
        sclp = (float)sinf((1.0f - t) * omega) / s;
        sclq = (float)sinf(t * omega) / s;
    } else {
        sclp = 1.0f - t;
        sclq = t;
    }

    quat_t q;
    q.x = sclp * a.x + sclq * end.x;
    q.y = sclp * a.y + sclq * end.y;
    q.z = sclp * a.z + sclq * end.z;
    q.w = sclp * a.w + sclq * end.w;

    return q;
}

/*
* @brief Convert given quaternion param into equivalent 4x4 rotation matrix
* @note: From http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
*/
static inline mat4f_t quat_to_mat4(quat_t q)
{
    mat4f_t m = mat4_identity();
    quat_t qn = quat_norm(q);

    f32 xx = qn.x * qn.x;
    f32 yy = qn.y * qn.y;
    f32 zz = qn.z * qn.z;
    f32 xy = qn.x * qn.y;
    f32 xz = qn.x * qn.z;
    f32 yz = qn.y * qn.z;
    f32 wx = qn.w * qn.x;
    f32 wy = qn.w * qn.y;
    f32 wz = qn.w * qn.z;

    m.elems[0 * 4 + 0] = 1.0f - 2.0f * (yy + zz);
    m.elems[1 * 4 + 0] = 2.0f * (xy - wz);
    m.elems[2 * 4 + 0] = 2.0f * (xz + wy);

    m.elems[0 * 4 + 1] = 2.0f * (xy + wz);
    m.elems[1 * 4 + 1] = 1.0f - 2.0f * (xx + zz);
    m.elems[2 * 4 + 1] = 2.0f * (yz - wx);

    m.elems[0 * 4 + 2] = 2.0f * (xz - wy);
    m.elems[1 * 4 + 2] = 2.0f * (yz + wx);
    m.elems[2 * 4 + 2] = 1.0f - 2.0f * (xx + yy);

    return m;
}

static inline quat_t quat_from_euler(f32 yaw_deg, f32 pitch_deg, f32 roll_deg)
{
    f32 yaw = DEG_TO_RAD(yaw_deg);
    f32 pitch = DEG_TO_RAD(pitch_deg);
    f32 roll = DEG_TO_RAD(roll_deg);

    quat_t q;
    f32 cy = (float)cos(yaw * 0.5f);
    f32 sy = (float)sin(yaw * 0.5f);
    f32 cr = (float)cos(roll * 0.5f);
    f32 sr = (float)sin(roll * 0.5f);
    f32 cp = (float)cos(pitch * 0.5f);
    f32 sp = (float)sin(pitch * 0.5f);

    q.x = cy * sr * cp - sy * cr * sp;
    q.y = cy * cr * sp + sy * sr * cp;
    q.z = sy * cr * cp - cy * sr * sp;
    q.w = cy * cr * cp + sy * sr * sp;

    return q;
}
