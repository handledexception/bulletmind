#include "core/types.h"
#include "core/logger.h"
#include "core/utils.h"
#include "core/vector.h"
#include "math/types.h"
#include "platform/platform.h"
#include "gui/gui.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/scene.h"
#include "media/image.h"

#include "asset.h"
#include "command.h"
#include "input.h"

#ifdef BM_WINDOWS
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN
#endif

#define APP_NAME "Bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_ASSETS_TOML_PATH "assets/assets.toml"
#define GFX_ADAPTER_INDEX 0
#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720

static const rgba_t kClearColor = {
	.r = 0,
	.g = 0,
	.b = 0,
	.a = 255,
};

struct application {
	const char* name;
	u32 version;
	bool running;
	VECTOR(gui_window_t*) windows;
	struct input_state* inputs;
	struct asset_manager* assets;
	camera_t cam;
	VECTOR(struct gfx_scene*) scenes;
	gfx_buffer_t* vbuf;
	gfx_buffer_t* ibuf;
};

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	rect_t viewport = {.x = 0, .y = 0, .w = VIEW_WIDTH, .h = VIEW_HEIGHT};
	ENSURE_OK(gfx_init(cfg, GFX_D3D11 | GFX_USE_ZBUFFER));
	ENSURE_OK(gfx_init_sampler_state());
	ENSURE_OK(gfx_init_rasterizer(GFX_CULLING_NONE, 0));
	vec3f_t cam_eye = {0.f, 0.f, -1.f};
	vec3f_t cam_dir = {0.f, 0.f, 1.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_new(&app->cam);
	gfx_camera_ortho(&app->cam, &cam_eye, &cam_dir, &cam_up, &viewport, Z_NEAR, Z_FAR);
	// gfx_camera_persp(&app->cam, &cam_eye, &cam_dir, &cam_up, &viewport, 90.f, Z_NEAR, Z_FAR);

	return RESULT_OK;
}

void app_refresh_gfx(struct application* app)
{
	gfx_render_clear(&kClearColor);
	size_t vbd_size = 0;
	size_t vbd_offs = 0;
	size_t tex_vert_size = 0;
	size_t vert_stride = 0;
	u8* vbuf_data = gfx_buffer_get_data(app->vbuf);
	u32* ibuf_data = (u32*)gfx_buffer_get_data(app->ibuf);
	size_t vertex_buffer_size = (BM_GFX_MAX_VERTICES * sizeof(vec3f_t)) +
				    (BM_GFX_MAX_VERTICES * sizeof(vec2f_t));
	size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
	memset(vbuf_data, 0, vertex_buffer_size);
	memset(ibuf_data, 0, index_buffer_size);
	for (size_t sdx = 0; sdx < app->scenes.num_elems; sdx++) {
		struct gfx_scene* scene =
			(struct gfx_scene*)app->scenes.elems[sdx];
		gfx_shader_t* vs = scene->vertex_shader;
		gfx_shader_t* ps = scene->pixel_shader;
		vert_stride = gfx_get_vertex_stride(
			GFX_VERTEX_POS_UV); // FIXME: need a function to get vertex shader stride from gfx_shader_t*
		// Copy vertex buffer data
		for (u32 vdx = 0; vdx < scene->vert_data->num_vertices; vdx++) {
			memcpy((void*)&vbuf_data[vbd_offs],
			       (const void*)&scene->vert_data->positions[vdx],
			       sizeof(struct vec3f));
			vbd_offs += sizeof(struct vec3f);
			struct texture_vertex* tex_vert =
				&scene->vert_data->tex_verts[vdx];
			tex_vert_size = tex_vert->size;
			memcpy((void*)&vbuf_data[vbd_offs],
			       (const void*)tex_vert->data, tex_vert_size);
			vbd_offs += tex_vert_size;
		}
		vbd_size += (sizeof(vec3f_t) * scene->vert_data->num_vertices) +
			    (tex_vert_size * scene->vert_data->num_vertices);
		for (u32 idx = 0; idx < 6; idx++) {
			memcpy(&ibuf_data[idx], &scene->index_data[idx],
			       sizeof(u32));
		}

		gfx_set_vertex_shader(vs);
		gfx_set_pixel_shader(ps);
		gfx_system_bind_input_layout(vs);
		gfx_bind_sampler_state((gfx_texture_t*)ps->vars.elems[0].data,
				       0);
		gfx_bind_vertex_buffer(app->vbuf, (u32)vert_stride, 0);
		gfx_bind_index_buffer(app->ibuf, 0);
		if (gfx_shader_cbuffer_fill(vs) > 0) {
			gfx_buffer_upload_constants(vs);
		}
		if (gfx_shader_cbuffer_fill(ps) > 0) {
			gfx_buffer_upload_constants(ps);
		}
		gfx_buffer_copy(app->vbuf, vbuf_data, vbd_size);
		gfx_buffer_copy(app->ibuf, ibuf_data, 6 * sizeof(u32));
		gfx_system_bind_render_target();
		gfx_toggle_depth(false);
		gfx_bind_primitive_topology(GFX_TOPOLOGY_TRIANGLE_LIST);
		gfx_bind_rasterizer();
		gfx_render_begin(true);
		gfx_render_end(false, 0);
	}
}

result app_init_inputs(struct application* app)
{
	if (app->inputs)
		inp_free(app->inputs);
	app->inputs = inp_new();
	inp_bind_virtual_key(app->inputs, kCommandQuit, SCANCODE_ESCAPE);
	inp_bind_virtual_key(app->inputs, kCommandPlayerUp, SCANCODE_W);
	inp_bind_virtual_key(app->inputs, kCommandPlayerDown, SCANCODE_S);
	inp_bind_virtual_key(app->inputs, kCommandPlayerLeft, SCANCODE_A);
	inp_bind_virtual_key(app->inputs, kCommandPlayerRight, SCANCODE_D);
	inp_bind_virtual_key(app->inputs, kCommandPlayerSpeed, SCANCODE_LSHIFT);
	return RESULT_OK;
}

result app_init_assets(struct application* app, const char* assets_toml_path)
{
	if (app->assets)
		asset_manager_free(app->assets);
	app->assets = asset_manager_new();
	return asset_manager_load_toml(assets_toml_path, app->assets);
}

result app_init_scenes(struct application* app)
{
	asset_t* img_asset;
	ENSURE_OK(asset_manager_find("metro", app->assets, &img_asset));
	gfx_shader_var_t tex_var = {.name = "metro",
					 .type = GFX_SHADER_VAR_TEX,
					 .data = NULL,
					 .own_data = true};
	struct media_image* image = (struct media_image*)img_asset->data;
	ENSURE_OK(gfx_texture_from_image(image,
					 (gfx_texture_t**)&tex_var.data));

	asset_t* vs_asset = NULL;
	asset_t* ps_asset = NULL;
	ENSURE_OK(asset_manager_find("pos_uv_vs", app->assets, &vs_asset));
	// ENSURE_OK(asset_manager_find("fullscreen_vs", app->assets, &vs_asset));
	ENSURE_OK(asset_manager_find("pos_uv_ps", app->assets, &ps_asset));

	size_t vertex_buffer_size = (BM_GFX_MAX_VERTICES * sizeof(vec3f_t)) +
				    (BM_GFX_MAX_VERTICES * sizeof(vec2f_t));
	gfx_buffer_new(NULL, vertex_buffer_size, GFX_BUFFER_VERTEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->vbuf);
	size_t index_buffer_size = sizeof(u32) * BM_GFX_MAX_INDICES;
	gfx_buffer_new(NULL, index_buffer_size, GFX_BUFFER_INDEX,
		       GFX_BUFFER_USAGE_DYNAMIC, &app->ibuf);

	struct gfx_scene* sprite = gfx_scene_new(4, 6, GFX_VERTEX_POS_UV);
	sprite->vertex_shader = (gfx_shader_t*)vs_asset->data;
	sprite->pixel_shader = (gfx_shader_t*)ps_asset->data;
	vec3f_t positions[4] = {
		{-1.f, -1.f, 0.f},
		{-1.f,  1.f, 0.f},
		{ 1.f,  1.f, 0.f},
		{ 1.f, -1.f, 0.f},
	};
	memcpy(sprite->vert_data->positions, positions, sizeof(vec3f_t) * 4);
	vec2f_t uv = {0.f, 1.f};
	memcpy(sprite->vert_data->tex_verts[0].data, &uv, sizeof(vec2f_t));
	uv.x = 0.f;
	uv.y = 0.f;
	memcpy(sprite->vert_data->tex_verts[1].data, &uv, sizeof(vec2f_t));
	uv.x = 1.f;
	uv.y = 0.f;
	memcpy(sprite->vert_data->tex_verts[2].data, &uv, sizeof(vec2f_t));
	uv.x = 1.f;
	uv.y = 1.f;
	memcpy(sprite->vert_data->tex_verts[3].data, &uv, sizeof(vec2f_t));

	sprite->index_data[0] = 0;
	sprite->index_data[1] = 1;
	sprite->index_data[2] = 2;
	sprite->index_data[3] = 0;
	sprite->index_data[4] = 2;
	sprite->index_data[5] = 3;

	mat4f_t world_matrix;
	mat4f_t trans_matrix, scale_matrix;
	mat4f_identity(&world_matrix);
	mat4f_identity(&trans_matrix);
	mat4f_identity(&scale_matrix);
	const vec4f_t trans_vec = {0.f, 0.f, 0.f, 0.f};
	const vec4f_t scale_vec = {
		// 1.f,
		(float)image->width - (float)VIEW_WIDTH,
		(float)image->height - (float)VIEW_HEIGHT,
		// (float)VIEW_HEIGHT / (float)image->height,
		// image->width * 0.667f, image->height * 0.667f,
		0.f, 0.f};
	mat4f_translate(&trans_matrix, &trans_vec);
	mat4f_scale(&scale_matrix, &scale_vec);
	mat4f_mul(&world_matrix, &world_matrix, &trans_matrix);
	mat4f_mul(&world_matrix, &world_matrix, &scale_matrix);

	mat4f_t view_proj_matrix;
	mat4f_identity(&view_proj_matrix);
	mat4f_mul(&view_proj_matrix, &app->cam.view_matrix,
		  &app->cam.proj_matrix);
	mat4f_transpose(&view_proj_matrix, &view_proj_matrix);
	gfx_shader_var_t world_var = {.name = "world",
				      .type = GFX_SHADER_VAR_MAT4,
				      .data = NULL,
				      .own_data = true};
	gfx_shader_var_set(&world_var, &world_matrix);
	gfx_shader_var_t view_proj_var = {.name = "view_proj",
					  .type = GFX_SHADER_VAR_MAT4,
					  .data = NULL,
					  .own_data = true};
	gfx_shader_var_set(&view_proj_var, &view_proj_matrix);

	vec_push_back(app->scenes, &sprite);
	gfx_shader_add_var(sprite->vertex_shader, &world_var);
	gfx_shader_add_var(sprite->vertex_shader, &view_proj_var);
	gfx_shader_add_var(sprite->pixel_shader, &tex_var);
	vec2f_t vp_res = {VIEW_WIDTH, VIEW_HEIGHT};
	gfx_shader_var_t viewport_res_var = {.name = "viewport_res",
					     .type = GFX_SHADER_VAR_VEC2,
					     .data = NULL,
					     .own_data = true};
	gfx_shader_var_set(&viewport_res_var, &vp_res);
	gfx_shader_add_var(sprite->pixel_shader, &viewport_res_var);
	size_t cbuf_size = gfx_shader_get_vars_size(sprite->vertex_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &sprite->vertex_shader->cbuffer);
	cbuf_size = gfx_shader_get_vars_size(sprite->pixel_shader);
	gfx_buffer_new(NULL, cbuf_size, GFX_BUFFER_CONSTANT,
		       GFX_BUFFER_USAGE_DYNAMIC,
		       &sprite->pixel_shader->cbuffer);

	return RESULT_OK;
}

result app_init(struct application* app, s32 version, u32 vx, u32 vy,
		const char* assets_toml_path)
{
	if (app == NULL)
		return RESULT_NULL;
	app->running = false;
	app->version = version;
	vec_init(app->windows);
	vec_init(app->scenes);

	ENSURE_OK(gui_init());
	char ver_str[16];
	version_string(app->version, ver_str);
	char window_title[256];
	snprintf(window_title, (sizeof(APP_NAME) + 1 + 16) + 1, "%s %s",
		 APP_NAME, ver_str);
	gui_window_t* main_window =
		gui_create_window(window_title, 0, 0, vx, vy, 0, NULL);
	gui_center_window(main_window);
	vec_push_back(app->windows, &main_window);
	gui_window_t* view_window = gui_create_window(
		"canvas_view", 0, 0, vx, vy, 0, gui->windows.elems[0]);
	vec_push_back(app->windows, &view_window);
	void* gfx_view_handle = gui_get_window_handle(app->windows.elems[1]);
	const struct gfx_config gfx_cfg = {
		.module = GFX_MODULE_DX11,
		.window = {gfx_view_handle},
		.adapter = GFX_ADAPTER_INDEX,
		.width = vx,
		.height = vy,
		.fps_num = 144,
		.fps_den = 1,
		.fullscreen = false,
		.pix_fmt = PIX_FMT_RGBA32,
	};
	ENSURE_OK(app_init_gfx(app, &gfx_cfg));
	ENSURE_OK(app_init_inputs(app));
	ENSURE_OK(app_init_assets(app, assets_toml_path));
	ENSURE_OK(app_init_scenes(app));

	return RESULT_OK;
}

void app_refresh(struct application* app)
{
	app->running = true;
	while (app->running) {
		gui_refresh();
		gui_event_t evt;
		while (gui_poll_event(&evt)) {
			if (evt.type == GUI_EVENT_MOUSE_MOTION)
				printf("sx: %d sy: %d | wx: %d wy: %d\n",
					evt.mouse.screen_pos.x, evt.mouse.screen_pos.y,
					evt.mouse.window_pos.x, evt.mouse.window_pos.y);
			inp_refresh_pressed(app->inputs, &evt);
			if (inp_cmd_get_state(app->inputs, kCommandQuit))
				app->running = false;
		}
		// struct mouse_device mouse;
		// gui_get_global_mouse_state(&mouse);
		// printf("sx: %d sy: %d | wx: %d wy: %d\n",
		// 	mouse.screen_pos.x, mouse.screen_pos.y,
		// 	mouse.window_pos.x, mouse.window_pos.y);
		app_refresh_gfx(app);
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		gfx_buffer_free(app->ibuf);
		gfx_buffer_free(app->vbuf);
		for (size_t i = 0; i < app->scenes.num_elems; i++)
			gfx_scene_free(app->scenes.elems[i]);
		vec_free(app->scenes);
		asset_manager_free(app->assets);
		inp_free(app->inputs);
		gfx_shutdown();
		gui_shutdown();
		vec_free(app->windows);
	}
}

// #if defined(BM_WINDOWS) && !defined(BM_DEBUG)
// int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
// #else
// #endif
int main(int argc, char** argv)
{
	logger(LOG_INFO, "%s %d.%d.%d (built %s %s)", APP_NAME, APP_VER_MAJ,
	       APP_VER_MIN, APP_VER_REV, BM_BUILD_DATE, BM_BUILD_TIME);
	logger(LOG_INFO, "Platform: %s", os_platform_string());
	logger(LOG_INFO, "Architecture: %s (%s)", os_architecture_string(),
	       os_bits_string());
	logger(LOG_INFO, "Compiler: %s", os_compiler_string());
	logger(LOG_INFO, "Pointer size: %zu bytes", sizeof(intptr_t));
	struct application app;
	memset(&app, 0, sizeof(struct application));
	const u32 app_ver = pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	ENSURE_OK(app_init(&app, app_ver, VIEW_WIDTH, VIEW_HEIGHT,
			   APP_ASSETS_TOML_PATH));
	app_refresh(&app);
	app_shutdown(&app);
	ENSURE_OK(mem_report_leaks());
	return RESULT_OK;
}
