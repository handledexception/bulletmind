#include "core/memory.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "math/quat.h"

void gfx_camera_new(camera_t* cam)
{
	quat_t q1, q2, dst;
	quat_identity(&q1);
	quat_identity(&q2);
	q1.x = 0.1;
	q1.y = 0.2;
	q1.z = 0.3;
	q2.x = 0.4;
	q2.y = 0.5;
	q2.z = 0.6;
	f32 dot = quat_dot(&q1, &q2);
	quat_mul(&dst, &q1, &q2);
	if (cam == NULL)
		cam = (camera_t*)BM_ALLOC(sizeof(*cam));
	gfx_camera_init(cam);
}

void gfx_camera_free(camera_t* cam)
{
	if (cam != NULL) {
		BM_FREE((void*)cam);
		cam = NULL;
	}
}

void gfx_camera_init(camera_t* cam)
{
	if (cam) {
		vec3f_zero(&cam->eye);
		vec3f_zero(&cam->dir);
		vec3f_zero(&cam->up);
		mat4f_identity(&cam->view_matrix);
		mat4f_identity(&cam->proj_matrix);
		cam->viewport.x = 0;
		cam->viewport.y = 0;
		cam->viewport.w = 0;
		cam->viewport.h = 0;
		cam->fov = 0.f;
		cam->z_far = 0.f;
		cam->z_near = 0.f;
		cam->projection = GFX_PROJECTION_PERSPECTIVE;
	}
}

void gfx_camera_set_view_dir(camera_t* cam, vec3f_t dir)
{
	vec3f_t view_dir;
	vec3f_copy(&view_dir, &dir);
	vec3f_norm(&view_dir, &view_dir);
}
void gfx_camera_set_orientation(camera_t* cam);
float gfx_camera_get_focal_length(const camera_t* cam);
void gfx_camera_set_world_up(camera_t* cam);

void gfx_camera_ortho(camera_t* cam, const vec3f_t* eye, const vec3f_t* dir,
		      const vec3f_t* up, const rect_t* viewport, f32 z_near,
		      f32 z_far)
{
	vec3f_copy(&cam->eye, eye);
	vec3f_copy(&cam->dir, dir);
	vec3f_copy(&cam->up, up);
	rect_copy(&cam->viewport, viewport);
	cam->z_near = z_near;
	cam->z_far = z_far;
	cam->projection = GFX_PROJECTION_ORTHOGRAPIC;
	gfx_camera_update_view_matrix(cam);
	gfx_camera_update_proj_matrix(cam);
}

void gfx_camera_persp(camera_t* cam, const vec3f_t* eye, const vec3f_t* dir,
		      const vec3f_t* up, const rect_t* viewport, f32 fov,
		      f32 z_near, f32 z_far)
{
	vec3f_copy(&cam->eye, eye);
	vec3f_copy(&cam->dir, dir);
	vec3f_copy(&cam->up, up);
	rect_copy(&cam->viewport, viewport);
	cam->fov = fov;
	cam->z_near = z_near;
	cam->z_far = z_far;
	cam->projection = GFX_PROJECTION_PERSPECTIVE;
	gfx_camera_update_view_matrix(cam);
	gfx_camera_update_proj_matrix(cam);
}

void gfx_camera_update_view_matrix(camera_t* cam)
{
	if (cam) {
		if (cam->projection == GFX_PROJECTION_ORTHOGRAPIC) {
			mat4f_look_at_lh(&cam->view_matrix, &cam->eye, &cam->dir, &cam->up);
		} else if (cam->projection == GFX_PROJECTION_PERSPECTIVE) {
			f32 aspect = (f32)cam->viewport.w / (f32)cam->viewport.h;
			mat4f_look_at_lh(&cam->view_matrix, &cam->eye, &cam->dir, &cam->up);
		}
	}
}
void gfx_camera_update_proj_matrix(camera_t* cam)
{
	if (cam) {
		if (cam->projection == GFX_PROJECTION_ORTHOGRAPIC) {
			mat4f_ortho_lh(&cam->proj_matrix, (f32)cam->viewport.w, (f32)cam->viewport.h, cam->z_near, cam->z_far);
		} else if (cam->projection == GFX_PROJECTION_PERSPECTIVE) {
			f32 aspect = (f32)cam->viewport.w / (f32)cam->viewport.h;
			mat4f_perspective_fov_lh(&cam->proj_matrix, cam->fov, aspect, cam->z_near, cam->z_far);
		}
	}
}
