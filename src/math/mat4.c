#include "math/mat4.h"
#include "math/vec3.h"
#include "math/utils.h"

void mat4f_transpose(struct mat4f* dst, const struct mat4f* m)
{
	dst->v.x.x = m->v.x.x;
	dst->v.x.y = m->v.y.x;
	dst->v.x.z = m->v.z.x;
	dst->v.x.w = m->v.w.x;

	dst->v.y.x = m->v.x.y;
	dst->v.y.y = m->v.y.y;
	dst->v.y.z = m->v.z.y;
	dst->v.y.w = m->v.w.y;

	dst->v.z.x = m->v.x.z;
	dst->v.z.y = m->v.y.z;
	dst->v.z.z = m->v.z.z;
	dst->v.z.w = m->v.w.z;

	dst->v.w.x = m->v.x.w;
	dst->v.w.y = m->v.y.w;
	dst->v.w.z = m->v.z.w;
	dst->v.w.w = m->v.w.w;
}

void mat4f_mul(struct mat4f* dst, const struct mat4f* lhs,
	       const struct mat4f* rhs)
{
	// mat4f_t tmp;
	// mat4f_zero(&tmp);
	// mat4f_transpose(&tmp, rhs);
	// dst->v.x.x = vec4f_dot(&lhs->v.x, &rhs->v.x);
	// dst->v.x.y = vec4f_dot(&lhs->v.x, &rhs->v.y);
	// dst->v.x.z = vec4f_dot(&lhs->v.x, &rhs->v.z);
	// dst->v.x.w = vec4f_dot(&lhs->v.x, &rhs->v.w);
	// dst->v.y.x = vec4f_dot(&lhs->v.y, &rhs->v.x);
	// dst->v.y.y = vec4f_dot(&lhs->v.y, &rhs->v.y);
	// dst->v.y.z = vec4f_dot(&lhs->v.y, &rhs->v.z);
	// dst->v.y.w = vec4f_dot(&lhs->v.y, &rhs->v.w);
	// dst->v.z.x = vec4f_dot(&lhs->v.z, &rhs->v.x);
	// dst->v.z.y = vec4f_dot(&lhs->v.z, &rhs->v.y);
	// dst->v.z.z = vec4f_dot(&lhs->v.z, &rhs->v.z);
	// dst->v.z.w = vec4f_dot(&lhs->v.z, &rhs->v.w);
	// dst->v.w.x = vec4f_dot(&lhs->v.w, &rhs->v.x);
	// dst->v.w.y = vec4f_dot(&lhs->v.w, &rhs->v.y);
	// dst->v.w.z = vec4f_dot(&lhs->v.w, &rhs->v.z);
	// dst->v.w.w = vec4f_dot(&lhs->v.w, &rhs->v.w);
	dst->v.x.x = (lhs->v.x.x * rhs->v.x.x) + (lhs->v.x.y * rhs->v.y.x) +
		     (lhs->v.x.z * rhs->v.z.x) + (lhs->v.x.w * rhs->v.w.x);
	dst->v.x.y = (lhs->v.x.x * rhs->v.x.y) + (lhs->v.x.y * rhs->v.y.y) +
		     (lhs->v.x.z * rhs->v.z.y) + (lhs->v.x.w * rhs->v.w.y);
	dst->v.x.z = (lhs->v.x.x * rhs->v.x.z) + (lhs->v.x.y * rhs->v.y.z) +
		     (lhs->v.x.z * rhs->v.z.z) + (lhs->v.x.w * rhs->v.w.z);
	dst->v.x.w = (lhs->v.x.x * rhs->v.x.w) + (lhs->v.x.y * rhs->v.y.w) +
		     (lhs->v.x.z * rhs->v.z.w) + (lhs->v.x.w * rhs->v.w.w);

	dst->v.y.x = (lhs->v.y.x * rhs->v.x.x) + (lhs->v.y.y * rhs->v.y.x) +
		     (lhs->v.y.z * rhs->v.z.x) + (lhs->v.y.w * rhs->v.w.x);
	dst->v.y.y = (lhs->v.y.x * rhs->v.x.y) + (lhs->v.y.y * rhs->v.y.y) +
		     (lhs->v.y.z * rhs->v.z.y) + (lhs->v.y.w * rhs->v.w.y);
	dst->v.y.z = (lhs->v.y.x * rhs->v.x.z) + (lhs->v.y.y * rhs->v.y.z) +
		     (lhs->v.y.z * rhs->v.z.z) + (lhs->v.y.w * rhs->v.w.z);
	dst->v.y.w = (lhs->v.y.x * rhs->v.x.w) + (lhs->v.y.y * rhs->v.y.w) +
		     (lhs->v.y.z * rhs->v.z.w) + (lhs->v.y.w * rhs->v.w.w);

	dst->v.z.x = (lhs->v.z.x * rhs->v.x.x) + (lhs->v.z.y * rhs->v.y.x) +
		     (lhs->v.z.z * rhs->v.z.x) + (lhs->v.z.w * rhs->v.w.x);
	dst->v.z.y = (lhs->v.z.x * rhs->v.x.y) + (lhs->v.z.y * rhs->v.y.y) +
		     (lhs->v.z.z * rhs->v.z.y) + (lhs->v.z.w * rhs->v.w.y);
	dst->v.z.z = (lhs->v.z.x * rhs->v.x.z) + (lhs->v.z.y * rhs->v.y.z) +
		     (lhs->v.z.z * rhs->v.z.z) + (lhs->v.z.w * rhs->v.w.z);
	dst->v.z.w = (lhs->v.z.x * rhs->v.x.w) + (lhs->v.z.y * rhs->v.y.w) +
		     (lhs->v.z.z * rhs->v.z.w) + (lhs->v.z.w * rhs->v.w.w);

	dst->v.w.x = (lhs->v.w.x * rhs->v.x.x) + (lhs->v.w.y * rhs->v.y.x) +
		     (lhs->v.w.z * rhs->v.z.x) + (lhs->v.w.w * rhs->v.w.x);
	dst->v.w.y = (lhs->v.w.x * rhs->v.x.y) + (lhs->v.w.y * rhs->v.y.y) +
		     (lhs->v.w.z * rhs->v.z.y) + (lhs->v.w.w * rhs->v.w.y);
	dst->v.w.z = (lhs->v.w.x * rhs->v.x.z) + (lhs->v.w.y * rhs->v.y.z) +
		     (lhs->v.w.z * rhs->v.z.z) + (lhs->v.w.w * rhs->v.w.z);
	dst->v.w.w = (lhs->v.w.x * rhs->v.x.w) + (lhs->v.w.y * rhs->v.y.w) +
		     (lhs->v.w.z * rhs->v.z.w) + (lhs->v.w.w * rhs->v.w.w);
}

void mat4f_translate_v3(struct mat4f* dst, const struct vec3f* v)
{
	vec4f_t v4;
	vec4f_from_vec3(&v4, v);
	mat4f_translate(dst, &v4);
}

void mat4f_translate(struct mat4f* dst, const struct vec4f* v)
{
	dst->v.x.x = dst->v.y.y = dst->v.z.z = dst->v.w.w = 1.f;
	dst->v.x.w = v->x;
	dst->v.y.w = v->y;
	dst->v.z.w = v->z;
	dst->v.x.y = dst->v.x.z = dst->v.y.x = dst->v.y.z = dst->v.z.x =
		dst->v.z.y = dst->v.w.x = dst->v.w.y = dst->v.w.z = 0.f;
}

void mat4f_scale_v3(struct mat4f* dst, const struct vec3f* v)
{
	struct vec4f v4;
	vec4f_from_vec3(&v4, v);
	mat4f_scale(dst, &v4);
}

void mat4f_scale(struct mat4f* dst, const struct vec4f* v)
{
	dst->v.x.x = v->x;
	dst->v.y.y = v->y;
	dst->v.z.z = v->z;
	dst->v.w.w = 1.f;
	dst->v.x.y = dst->v.x.z = dst->v.x.w = dst->v.y.x = dst->v.y.z =
		dst->v.y.w = dst->v.z.x = dst->v.z.y = dst->v.z.w = dst->v.w.x =
			dst->v.w.y = dst->v.w.z = 0.f;
}

void mat4f_ortho_lh(struct mat4f* dst, f32 width, f32 height, f32 z_near,
		    f32 z_far)
{
	mat4f_zero(dst);
	f32 range = 1.f / (z_far - z_near);
	dst->v.x.x = 2.f / width;
	dst->v.y.y = 2.f / height;
	dst->v.z.z = range;
	dst->v.w.z = -range * z_near;
	dst->v.w.w = 1.f;
}

void mat4f_perspective_fov_lh(struct mat4f* dst, f32 fov, f32 aspect,
			      f32 z_near, f32 z_far)
{
	mat4f_zero(dst);
	f32 h = 1.f / tanf(DEG_TO_RAD(fov) * 0.5f);
	f32 w = h / aspect;
	f32 a = z_far / (z_far - z_near);
	f32 b = (z_far * z_near) / (z_far - z_near);
	dst->v.x.x = w;
	dst->v.y.y = h;
	dst->v.z.z = a;
	dst->v.z.w = 1.f;
	dst->v.w.z = -b;
}

void mat4f_look_at_lh(struct mat4f* dst, const struct vec3f* eye,
		      const struct vec3f* dir, const struct vec3f* up)
{
	struct vec3f look_vec;
	vec3f_sub(&look_vec, dir, eye);
	struct vec3f z_axis; //TODO: FIXME
	vec3f_norm(&z_axis, &look_vec);
	struct vec3f x_axis;
	vec3f_cross(&x_axis, up, &z_axis);
	vec3f_norm(&x_axis, &x_axis);
	struct vec3f y_axis;
	vec3f_cross(&y_axis, &z_axis, &x_axis);
	f32 eye_x = vec3f_dot(&x_axis, eye);
	f32 eye_y = vec3f_dot(&y_axis, eye);
	f32 eye_z = vec3f_dot(&z_axis, eye);
	dst->v.x.x = x_axis.x;
	dst->v.x.y = y_axis.x;
	dst->v.x.z = z_axis.x;
	dst->v.x.w = 0.f;
	dst->v.y.x = x_axis.y;
	dst->v.y.y = y_axis.y;
	dst->v.y.z = z_axis.y;
	dst->v.y.w = 0.f;
	dst->v.z.x = x_axis.z;
	dst->v.z.y = y_axis.z;
	dst->v.z.z = z_axis.z;
	dst->v.z.w = 0.f;
	dst->v.w.x = -eye_x;
	dst->v.w.y = -eye_y;
	dst->v.w.z = -eye_z;
	dst->v.w.w = 1.f;
}
