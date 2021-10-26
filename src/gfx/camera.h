#ifndef H_BM_GFX_CAMERA
#define H_BM_GFX_CAMERA

#include "math/types.h"
#include "core/rect.h"

typedef struct {
    vec3f_t pos;
    vec3f_t dir;
    vec3f_t up;
    mat4f_t view_matrix;
    mat4f_t proj_matrix;
    rect_t viewport;
    f32 fov;
    f32 z_near;
    f32 z_far;
} camera_t;

void gfx_camera_ortho(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 z_near, f32 z_far);
void gfx_camera_persp(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 fov, f32 z_near, f32 z_far);

#endif
