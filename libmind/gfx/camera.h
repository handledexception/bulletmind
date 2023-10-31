#pragma once

#include "math/types.h"
#include "core/rect.h"

typedef struct camera_s {
	vqs_t transform;
	vec3f_t angles; // yaw, pitch, roll
	f32 fov;
	f32 aspect;
	f32 ortho_scale;
	f32 near_plane;
	f32 far_plane;
	rect_t viewport;
	enum gfx_projection projection;
} camera_t;

camera_t* gfx_camera_new();
void gfx_camera_free(camera_t* cam);
camera_t gfx_camera_default();
camera_t gfx_camera_from_viewport(rect_t vp);
camera_t gfx_camera_persp(rect_t vp);
camera_t gfx_camera_ortho(rect_t vp);
void gfx_camera_set_pos(camera_t* cam, vec3f_t pos);
vec3f_t gfx_camera_forward(const camera_t* cam);
vec3f_t gfx_camera_backward(const camera_t* cam);
vec3f_t gfx_camera_up(const camera_t* cam);
vec3f_t gfx_camera_down(const camera_t* cam);
vec3f_t gfx_camera_left(const camera_t* cam);
vec3f_t gfx_camera_right(const camera_t* cam);
vec3f_t gfx_camera_world_to_screen(const camera_t* cam, vec3f_t coords);
vec3f_t gfx_camera_screen_to_world(const camera_t* cam, vec3f_t coords,
				   s32 view_x, s32 view_y);
mat4f_t gfx_camera_get_view_proj(const camera_t* cam);
mat4f_t gfx_camera_get_view(const camera_t* cam);
mat4f_t gfx_camera_get_proj(const camera_t* cam);
void gfx_camera_orient(camera_t* cam, f32 yaw, f32 pitch);
