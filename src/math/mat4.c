#include "math/mat4.h"
#include "math/vec3.h"
#include "math/utils.h"

void mat4f_transpose(struct mat4f* dst, const struct mat4f* m)
{
	dst->x.x = m->x.x;
	dst->x.y = m->y.x;
	dst->x.z = m->z.x;
	dst->x.w = m->w.x;

	dst->y.x = m->x.y;
	dst->y.y = m->y.y;
	dst->y.z = m->z.y;
	dst->y.w = m->w.y;

	dst->z.x = m->x.z;
	dst->z.y = m->y.z;
	dst->z.z = m->z.z;
	dst->z.w = m->w.z;

	dst->w.x = m->x.w;
	dst->w.y = m->y.w;
	dst->w.z = m->z.w;
	dst->w.w = m->w.w;
}

void mat4f_mul(struct mat4f* dst, const struct mat4f* lhs,
	       const struct mat4f* rhs)
{
	// mat4f_t tmp;
	// mat4f_zero(&tmp);
	// mat4f_transpose(&tmp, rhs);
	// dst->x.x = vec4f_dot(&lhs->x, &rhs->x);
	// dst->x.y = vec4f_dot(&lhs->x, &rhs->y);
	// dst->x.z = vec4f_dot(&lhs->x, &rhs->z);
	// dst->x.w = vec4f_dot(&lhs->x, &rhs->w);
	// dst->y.x = vec4f_dot(&lhs->y, &rhs->x);
	// dst->y.y = vec4f_dot(&lhs->y, &rhs->y);
	// dst->y.z = vec4f_dot(&lhs->y, &rhs->z);
	// dst->y.w = vec4f_dot(&lhs->y, &rhs->w);
	// dst->z.x = vec4f_dot(&lhs->z, &rhs->x);
	// dst->z.y = vec4f_dot(&lhs->z, &rhs->y);
	// dst->z.z = vec4f_dot(&lhs->z, &rhs->z);
	// dst->z.w = vec4f_dot(&lhs->z, &rhs->w);
	// dst->w.x = vec4f_dot(&lhs->w, &rhs->x);
	// dst->w.y = vec4f_dot(&lhs->w, &rhs->y);
	// dst->w.z = vec4f_dot(&lhs->w, &rhs->z);
	// dst->w.w = vec4f_dot(&lhs->w, &rhs->w);
	dst->x.x = (lhs->x.x * rhs->x.x) + (lhs->x.y * rhs->y.x) +
		     (lhs->x.z * rhs->z.x) + (lhs->x.w * rhs->w.x);
	dst->x.y = (lhs->x.x * rhs->x.y) + (lhs->x.y * rhs->y.y) +
		     (lhs->x.z * rhs->z.y) + (lhs->x.w * rhs->w.y);
	dst->x.z = (lhs->x.x * rhs->x.z) + (lhs->x.y * rhs->y.z) +
		     (lhs->x.z * rhs->z.z) + (lhs->x.w * rhs->w.z);
	dst->x.w = (lhs->x.x * rhs->x.w) + (lhs->x.y * rhs->y.w) +
		     (lhs->x.z * rhs->z.w) + (lhs->x.w * rhs->w.w);

	dst->y.x = (lhs->y.x * rhs->x.x) + (lhs->y.y * rhs->y.x) +
		     (lhs->y.z * rhs->z.x) + (lhs->y.w * rhs->w.x);
	dst->y.y = (lhs->y.x * rhs->x.y) + (lhs->y.y * rhs->y.y) +
		     (lhs->y.z * rhs->z.y) + (lhs->y.w * rhs->w.y);
	dst->y.z = (lhs->y.x * rhs->x.z) + (lhs->y.y * rhs->y.z) +
		     (lhs->y.z * rhs->z.z) + (lhs->y.w * rhs->w.z);
	dst->y.w = (lhs->y.x * rhs->x.w) + (lhs->y.y * rhs->y.w) +
		     (lhs->y.z * rhs->z.w) + (lhs->y.w * rhs->w.w);

	dst->z.x = (lhs->z.x * rhs->x.x) + (lhs->z.y * rhs->y.x) +
		     (lhs->z.z * rhs->z.x) + (lhs->z.w * rhs->w.x);
	dst->z.y = (lhs->z.x * rhs->x.y) + (lhs->z.y * rhs->y.y) +
		     (lhs->z.z * rhs->z.y) + (lhs->z.w * rhs->w.y);
	dst->z.z = (lhs->z.x * rhs->x.z) + (lhs->z.y * rhs->y.z) +
		     (lhs->z.z * rhs->z.z) + (lhs->z.w * rhs->w.z);
	dst->z.w = (lhs->z.x * rhs->x.w) + (lhs->z.y * rhs->y.w) +
		     (lhs->z.z * rhs->z.w) + (lhs->z.w * rhs->w.w);

	dst->w.x = (lhs->w.x * rhs->x.x) + (lhs->w.y * rhs->y.x) +
		     (lhs->w.z * rhs->z.x) + (lhs->w.w * rhs->w.x);
	dst->w.y = (lhs->w.x * rhs->x.y) + (lhs->w.y * rhs->y.y) +
		     (lhs->w.z * rhs->z.y) + (lhs->w.w * rhs->w.y);
	dst->w.z = (lhs->w.x * rhs->x.z) + (lhs->w.y * rhs->y.z) +
		     (lhs->w.z * rhs->z.z) + (lhs->w.w * rhs->w.z);
	dst->w.w = (lhs->w.x * rhs->x.w) + (lhs->w.y * rhs->y.w) +
		     (lhs->w.z * rhs->z.w) + (lhs->w.w * rhs->w.w);
}

void mat4f_translate_v3(struct mat4f* dst, const struct vec3f* v)
{
	vec4f_t v4;
	vec4f_from_vec3(&v4, v);
	mat4f_translate(dst, &v4);
}

void mat4f_translate(struct mat4f* dst, const struct vec4f* v)
{
	dst->x.x = dst->y.y = dst->z.z = dst->w.w = 1.f;
	dst->x.w = v->x;
	dst->y.w = v->y;
	dst->z.w = v->z;
	dst->x.y = dst->x.z = dst->y.x = dst->y.z = dst->z.x =
		dst->z.y = dst->w.x = dst->w.y = dst->w.z = 0.f;
}

void mat4f_scale_v3(struct mat4f* dst, const struct vec3f* v)
{
	struct vec4f v4;
	vec4f_from_vec3(&v4, v);
	mat4f_scale(dst, &v4);
}

void mat4f_scale(struct mat4f* dst, const struct vec4f* v)
{
	dst->x.x = v->x;
	dst->y.y = v->y;
	dst->z.z = v->z;
	dst->w.w = 1.f;
	dst->x.y = dst->x.z = dst->x.w = dst->y.x = dst->y.z =
		dst->y.w = dst->z.x = dst->z.y = dst->z.w = dst->w.x =
			dst->w.y = dst->w.z = 0.f;
}

void mat4f_ortho_lh(struct mat4f* dst, f32 width, f32 height, f32 z_near,
		    f32 z_far)
{
	mat4f_zero(dst);
	f32 range = 1.f / (z_far - z_near);
	dst->x.x = 2.f / width;
	dst->y.y = 2.f / height;
	dst->z.z = range;
	dst->w.z = -range * z_near;
	dst->w.w = 1.f;
}

void mat4f_perspective_fov_lh(struct mat4f* dst, f32 fov, f32 aspect,
			      f32 z_near, f32 z_far)
{
	f32 h = 1.f / tanf(DEG_TO_RAD(fov) * 0.5f);
	f32 w = h / aspect;
	f32 a = z_far / (z_far - z_near);
	f32 b = (z_far * z_near) / (z_far - z_near);
	mat4f_identity(dst);
	dst->x.x = w;
	dst->y.y = h;
	dst->z.z = a;
	dst->z.w = 1.f;
	dst->w.z = -b;
}

void mat4f_look_at_lh(struct mat4f* dst, const struct vec3f* eye,
		      const struct vec3f* dir, const struct vec3f* up)
{
	struct vec3f eye_dir;
	vec3f_sub(&eye_dir, dir, eye);
	struct vec3f z_axis; //TODO: FIXME
	vec3f_norm(&z_axis, &eye_dir);
	struct vec3f x_axis;
	vec3f_cross(&x_axis, up, &z_axis);
	vec3f_norm(&x_axis, &x_axis);
	struct vec3f y_axis;
	vec3f_cross(&y_axis, &z_axis, &x_axis);
	vec3f_t neg_eye;
	// vec3f_negate(&neg_eye, &eye);
	f32 eye_x = vec3f_dot(&x_axis, eye);
	f32 eye_y = vec3f_dot(&y_axis, eye);
	f32 eye_z = vec3f_dot(&z_axis, eye);
	dst->x.x = x_axis.x;
	dst->x.y = y_axis.x;
	dst->x.z = z_axis.x;
	dst->x.w = 0.f;
	dst->y.x = x_axis.y;
	dst->y.y = y_axis.y;
	dst->y.z = z_axis.y;
	dst->y.w = 0.f;
	dst->z.x = x_axis.z;
	dst->z.y = y_axis.z;
	dst->z.z = z_axis.z;
	dst->z.w = 0.f;
	dst->w.x = -eye_x;
	dst->w.y = -eye_y;
	dst->w.z = -eye_z;
	dst->w.w = 1.f;
	// mat4f_transpose(dst, dst);
}
