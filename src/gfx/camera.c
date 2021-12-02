#include "gfx/gfx.h"
#include "gfx/camera.h"

void gfx_camera_init(camera_t* cam)
{
	if (cam == NULL)
		cam = (camera_t*)bm_malloc(sizeof(*cam));
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
}

void gfx_camera_free(camera_t* cam)
{
	if (cam != NULL) {
		free((void*)cam);
		cam = NULL;
	}
}

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

	mat4f_look_at_lh(&cam->view_matrix, eye, dir, up);
	mat4f_ortho_lh(&cam->proj_matrix, viewport->w, viewport->h, z_near,
		       z_far);
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

	f32 aspect = (f32)viewport->w / (f32)viewport->h;
	mat4f_look_at_lh(&cam->view_matrix, &cam->eye, &cam->dir, &cam->up);
	mat4f_perspective_fov_lh(&cam->proj_matrix, fov, aspect, z_near, z_far);
}
