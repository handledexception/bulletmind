#include "math/mat4.h"
#include "math/vec3.h"
#include "math/utils.h"

void mat4f_transpose(mat4f_t* dst, const mat4f_t* m)
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

void mat4f_mul(mat4f_t* dst, const mat4f_t* lhs, const mat4f_t* rhs)
{
	mat4f_t tmp;
	mat4f_zero(&tmp);
	mat4f_transpose(&tmp, rhs);
	dst->x.x = vec4f_dot(&lhs->x, &tmp.x);
	dst->x.y = vec4f_dot(&lhs->x, &tmp.y);
	dst->x.z = vec4f_dot(&lhs->x, &tmp.z);
	dst->x.w = vec4f_dot(&lhs->x, &tmp.w);

	dst->y.x = vec4f_dot(&lhs->y, &tmp.x);
	dst->y.y = vec4f_dot(&lhs->y, &tmp.y);
	dst->y.z = vec4f_dot(&lhs->y, &tmp.z);
	dst->y.w = vec4f_dot(&lhs->y, &tmp.w);

	dst->z.x = vec4f_dot(&lhs->z, &tmp.x);
	dst->z.y = vec4f_dot(&lhs->z, &tmp.y);
	dst->z.z = vec4f_dot(&lhs->z, &tmp.z);
	dst->z.w = vec4f_dot(&lhs->z, &tmp.w);

	dst->w.x = vec4f_dot(&lhs->w, &tmp.x);
	dst->w.y = vec4f_dot(&lhs->w, &tmp.y);
	dst->w.z = vec4f_dot(&lhs->w, &tmp.z);
	dst->w.w = vec4f_dot(&lhs->w, &tmp.w);
}

void mat4f_ortho_lh(mat4f_t* dst, f32 width, f32 height, f32 z_near, f32 z_far)
{
	mat4f_zero(dst);
	f32 range = 1.f / (z_far - z_near);
	dst->x.x = 2.f / width;
	dst->y.y = 2.f / height;
	dst->z.z = range;
	dst->w.z = -range * z_near;
	dst->w.w = 1.f;
}

void mat4f_perspective_fov_lh(mat4f_t* dst, f32 fov, f32 aspect, f32 z_near,
			      f32 z_far)
{
	mat4f_zero(dst);
	f32 h = 1.f / tanf(DEG_TO_RAD(fov) * 0.5f);
	f32 w = h / aspect;
	f32 a = z_far / (z_far - z_near);
	f32 b = (z_far * z_near) / (z_far - z_near);
	dst->x.x = w;
	dst->y.y = h;
	dst->z.z = a;
	dst->z.w = 1.f;
	dst->w.z = -b;
}

void mat4f_look_at_lh(mat4f_t* dst, const vec3f_t* pos, const vec3f_t* dir,
		      const vec3f_t* up)
{
	vec3f_t look_vec;
	vec3f_sub(&look_vec, dir, pos);
	vec3f_t z_axis; //TODO: FIXME
	vec3f_norm(&z_axis, &look_vec);
	vec3f_t x_axis;
	vec3f_cross(&x_axis, up, &z_axis);
	vec3f_norm(&x_axis, &x_axis);
	vec3f_t y_axis;
	vec3f_cross(&y_axis, &z_axis, &x_axis);
	f32 eye_x = vec3f_dot(&x_axis, pos);
	f32 eye_y = vec3f_dot(&y_axis, pos);
	f32 eye_z = vec3f_dot(&z_axis, pos);
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
}
