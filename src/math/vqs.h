#ifndef H_BM_VQS
#define H_BM_VQS

#include "math/quat.h"
#include "math/mat4.h"

// https://www.eurosis.org/cms/files/conf/gameon-asia/gameon-asia2007/R-SESSION/G1.pdf
// https://www.cs.jhu.edu/~cohen/VW2000/Misc/Robinett_Holloway_94-031.pdf

typedef struct vqs
{
    union {
        vec3f_t position;
    };
    union {
        quat_t rotation;
    };
    union {
        vec3f_t scale;
    };
} vqs_t;

static inline vqs_t vqs_set(vec3f_t v, quat_t q, vec3f_t s)
{
    vqs_t t = { 0 };
    t.position = v;
    t.rotation = q;
    t.scale = s;
    return t;
}

static inline vqs_t vqs_default()
{
    vqs_t t = vqs_set(
        vec3_set(0.0f, 0.0f, 0.0f),
        quat_set(0.0f, 0.0f, 0.0f, 1.0f),
        vec3_set(1.0f, 1.0f, 1.0f)
    );
    return t;
}

// AbsScale = ParentScale * LocalScale
// AbsRot   = LocalRot * ParentRot
// AbsTrans = ParentPos + [ParentRot * (ParentScale * LocalPos)]
static inline vqs_t vqs_absolute_transform(const vqs_t* local, const vqs_t* parent)
{
    if (!local || !parent) {
        return vqs_default();
    }

    // Normalized rotations
    quat_t p_rot_norm = quat_norm(parent->rotation);
    quat_t l_rot_norm = quat_norm(local->rotation);

    // Scale
    vec3f_t scl = vec3_mul(local->scale, parent->scale);
    // Rotation
    quat_t rot = quat_norm(quat_mul(p_rot_norm, l_rot_norm));
    // position
    vec3f_t tns = vec3_add(parent->position, quat_rotate(p_rot_norm, vec3_mul(parent->scale, local->position)));

    return vqs_set(tns, rot, scl);
}

// RelScale = AbsScale / ParentScale
// RelRot   = Inverse(ParentRot) * AbsRot
// RelTrans = [Inverse(ParentRot) * (AbsPos - ParentPosition)] / ParentScale;
static inline vqs_t vqs_relative_transform(const vqs_t* absolute, const vqs_t* parent)
{
    if (!absolute || !parent) {
        return vqs_default();
    }

    // Get inverse rotation normalized
    quat_t p_rot_inv = quat_norm(quat_inverse(parent->rotation));
    // Normalized abs rotation
    quat_t a_rot_norm = quat_norm(absolute->rotation);

    // Scale
    vec3f_t scl = vec3_div(absolute->scale, parent->scale);
    // Rotation
    quat_t rot = quat_norm(quat_mul(p_rot_inv, a_rot_norm));
    // position
    vec3f_t tns = vec3_div(quat_rotate(p_rot_inv, vec3_sub(absolute->position, parent->position)), parent->scale);

    return vqs_set(tns, rot, scl);
}

static inline mat4f_t vqs_to_mat4(const vqs_t* transform)
{
    mat4f_t mat = mat4_identity();
    mat4f_t trans = mat4_translate(transform->position);
    mat4f_t rot = quat_to_mat4(transform->rotation);
    mat4f_t scl = mat4_scale(transform->scale);
    mat = mat4_mul(mat, trans);
    mat = mat4_mul(mat, rot);
    mat = mat4_mul(mat, scl);
    return mat;
}

static inline vqs_t vqs_from_mat4(const mat4f_t* m)
{
    vec3f_t translation = vec3_default();
    vec3f_t rotation = vec3_default();
    vec3f_t scale = vec3_set(1.0f, 1.0f, 1.0f);
    mat4_decompose(m, (float*)&translation, (float*)&rotation, (float*)&scale);
    return vqs_set(
        translation,
        quat_from_euler(rotation.x, rotation.y, rotation.z),
        scale
    );
}

static inline vec3f_t
vqs_forward(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(0.0f, 0.0f, -1.0f)));
}

static inline vec3f_t
vqs_backward(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(0.0f, 0.0f, 1.0f)));
}

static inline vec3f_t
vqs_left(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(-1.0f, 0.0f, 0.0f)));
}

static inline vec3f_t
vqs_right(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(1.0f, 0.0f, 0.0f)));
}

static inline vec3f_t
vqs_up(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(0.0f, 1.0f, 0.0f)));
}

static inline vec3f_t
vqs_down(const vqs_t* transform)
{
    return (quat_rotate(transform->rotation, vec3_set(0.0f, -1.0f, 0.0f)));
}

#endif
