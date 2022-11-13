#pragma once

#include "math/types.h"
#include "core/rect.h"

typedef struct camera_s {
	vec3f_t eye;
	vec3f_t dir;
	vec3f_t up;
	mat4f_t view_matrix;
	mat4f_t proj_matrix;
	rect_t viewport;
	f32 fov;
	f32 z_near;
	f32 z_far;
	enum gfx_projection projection;
} camera_t;

void gfx_camera_new(camera_t* cam);
void gfx_camera_free(camera_t* cam);
void gfx_camera_init(camera_t* cam);

void gfx_camera_set_view_dir(camera_t* cam, vec3f_t dir);
void gfx_camera_set_orientation(camera_t* cam);
float gfx_camera_get_focal_length(const camera_t* cam);
void gfx_camera_set_world_up(camera_t* cam);

void gfx_camera_ortho(camera_t* cam, const vec3f_t* eye, const vec3f_t* dir,
		      const vec3f_t* up, const rect_t* viewport, f32 z_near,
		      f32 z_far);
void gfx_camera_persp(camera_t* cam, const vec3f_t* eye, const vec3f_t* dir,
		      const vec3f_t* up, const rect_t* viewport, f32 fov,
		      f32 z_near, f32 z_far);
void gfx_camera_update_view_matrix(camera_t* cam);
void gfx_camera_update_proj_matrix(camera_t* cam);
