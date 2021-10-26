#include "gfx/gfx.h"
#include "gfx/camera.h"

void gfx_camera_ortho(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 z_near, f32 z_far)
{
    mat4f_look_at_lh(&cam->view_matrix, pos, dir, up);
    mat4f_ortho_lh(&cam->proj_matrix, viewport->w, viewport->h, z_near, z_far);
}

void gfx_camera_persp(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 fov, f32 z_near, f32 z_far)
{
    mat4f_look_at_lh(&cam->view_matrix, pos, dir, up);
    f32 aspect = (f32)viewport->w / (f32)viewport->h;
    mat4f_perspective_fov_lh(&cam->proj_matrix, fov, aspect, z_near, z_far);
}
