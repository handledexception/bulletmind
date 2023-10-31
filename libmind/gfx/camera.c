#include "core/memory.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "math/quat.h"

camera_t* gfx_camera_new()
{
	camera_t* cam = BM_ALLOC(sizeof(camera_t));
	*cam = gfx_camera_default();
	return cam;
}

void gfx_camera_free(camera_t* cam)
{
	if (cam) {
		BM_FREE(cam);
		cam = NULL;
	}
}

camera_t gfx_camera_default()
{
	camera_t cam = {0};
	cam.angles.x = 0.0f;
	cam.angles.y = 0.0f;
	cam.angles.z = 0.0f;
	cam.transform = vqs_default();
	cam.transform.position.z = 1.0f;
	cam.fov = 70.0f;
	cam.ortho_scale = 1.0f;
	cam.near_plane = 0.1f;
	cam.far_plane = 1000.0f;
	cam.viewport.x = 0;
	cam.viewport.y = 0;
	cam.viewport.w = 640;
	cam.viewport.h = 480;
	cam.projection = GFX_PROJECTION_ORTHOGRAPIC;
	return cam;
}

camera_t gfx_camera_from_viewport(rect_t vp)
{
	camera_t cam = gfx_camera_default();
	cam.viewport.x = vp.x;
	cam.viewport.y = vp.y;
	cam.viewport.w = vp.w;
	cam.viewport.h = vp.h;
	return cam;
}

camera_t gfx_camera_persp(rect_t vp)
{
	camera_t cam = gfx_camera_from_viewport(vp);
	cam.projection = GFX_PROJECTION_PERSPECTIVE;
	cam.transform.position.z = 1.0f;
	return cam;
}

camera_t gfx_camera_ortho(rect_t vp)
{
	return gfx_camera_from_viewport(vp);
}

void gfx_camera_set_pos(camera_t* cam, vec3f_t pos)
{
	if (cam)
		cam->transform.position = vec3_copy(pos);
}

vec3f_t gfx_camera_forward(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation,
			   vec3_set(0.0f, 0.0f, -1.0f));
}
vec3f_t gfx_camera_backward(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation, vec3_set(0.0f, 0.0f, 1.0f));
}
vec3f_t gfx_camera_up(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation, vec3_set(0.0f, 1.0f, 0.0f));
}
vec3f_t gfx_camera_down(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation,
			   vec3_set(0.0f, -1.0f, 0.0f));
}
vec3f_t gfx_camera_left(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation,
			   vec3_set(-1.0f, 0.0f, 0.0f));
}
vec3f_t gfx_camera_right(const camera_t* cam)
{
	return quat_rotate(cam->transform.rotation, vec3_set(1.0f, 0.0f, 0.0f));
}

vec3f_t gfx_camera_world_to_screen(const camera_t* cam, vec3f_t coords)
{
	// Transform world coords to screen coords to place billboarded UI elements in world
	mat4f_t vp = gfx_camera_get_view_proj(cam);
	vec4f_t p4 = vec4_set(coords.x, coords.y, coords.z, 1.f);
	p4 = mat4_mul_vec4(vp, p4);
	p4.x /= p4.w;
	p4.y /= p4.w;
	p4.z /= p4.w;

	// Bring into ndc
	p4.x = p4.x * 0.5f + 0.5f;
	p4.y = p4.y * 0.5f + 0.5f;

	// Bring into screen space
	p4.x = p4.x * (f32)cam->viewport.w;
	p4.y = map_range(1.f, 0.f, 0.f, 1.f, p4.y) * (f32)cam->viewport.h;

	return vec3_set(p4.x, p4.y, p4.z);
}
vec3f_t gfx_camera_screen_to_world(const camera_t* cam, vec3f_t coords,
				   s32 view_x, s32 view_y)
{
	vec3f_t wc = vec3_default();

	// Get inverse of view projection from camera
	mat4f_t inverse_vp = mat4_inverse(gfx_camera_get_view_proj(cam));
	f32 w_x = (f32)coords.x - (f32)view_x;
	f32 w_y = (f32)coords.y - (f32)view_y;
	f32 w_z = (f32)coords.z;

	// Transform from ndc
	vec4f_t in;
	in.x = (w_x / (f32)cam->viewport.w) * 2.f - 1.f;
	in.y = 1.f - (w_y / (f32)cam->viewport.h) * 2.f;
	in.z = 2.f * w_z - 1.f;
	in.w = 1.f;

	// To world coords
	vec4f_t out = mat4_mul_vec4(inverse_vp, in);
	if (out.w == 0.f) {
		// Avoid div by zero
		return wc;
	}

	out.w = fabsf(out.w) > MED_EPSILON ? 1.f / out.w : 0.0001f;
	wc = vec3_set(out.x * out.w, out.y * out.w, out.z * out.w);

	return wc;
}
mat4f_t gfx_camera_get_view_proj(const camera_t* cam)
{
	mat4f_t view = gfx_camera_get_view(cam);
	mat4f_t proj = gfx_camera_get_proj(cam);
	return mat4_mul(proj, view);
}

mat4f_t gfx_camera_get_view(const camera_t* cam)
{
	vec3f_t up = gfx_camera_up(cam);
	vec3f_t fwd = gfx_camera_forward(cam);
	vec3f_t tgt = vec3_add(fwd, cam->transform.position);
	return mat4_lookat(cam->transform.position, tgt, up);
}

mat4f_t gfx_camera_get_proj(const camera_t* cam)
{
	mat4f_t proj_mat = mat4_identity();
	f32 aspect = (f32)cam->viewport.w / (f32)cam->viewport.h;
	switch (cam->projection) {
	case GFX_PROJECTION_PERSPECTIVE: {
		proj_mat = mat4_persp(cam->fov, aspect, cam->near_plane,
				      cam->far_plane);
		break;
	}
	case GFX_PROJECTION_ORTHOGRAPIC: {
		f32 distance = 0.5f * (cam->far_plane - cam->near_plane);
		const f32 ortho_scale = cam->ortho_scale;
		proj_mat = mat4_ortho(-ortho_scale * aspect,
				      ortho_scale * aspect, -ortho_scale,
				      ortho_scale, -distance, distance);
	} break;
	}

	return proj_mat;
}

void gfx_camera_orient(camera_t* cam, f32 yaw, f32 pitch)
{
	quat_t x = quat_angle_axis(DEG_TO_RAD(yaw),
				   vec3_set(0.f, 1.f, 0.f)); // Absolute up
	quat_t y = quat_angle_axis(DEG_TO_RAD(pitch),
				   gfx_camera_right(cam)); // Relative right
	cam->transform.rotation =
		quat_mul(quat_mul(x, y), cam->transform.rotation);
}
