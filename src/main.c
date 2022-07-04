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
#define GFX_ADAPTER_INDEX 1
#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 1024

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
	u8* vbuf_data;
	gfx_buffer_t* vbuf;
	u8* cbuf_data;
	gfx_buffer_t* cbuf;
};

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	rect_t viewport = {.x = 0, .y = 0, .w = VIEW_WIDTH, .h = VIEW_HEIGHT};
	ENSURE_OK(gfx_init(cfg, GFX_D3D11 | GFX_USE_ZBUFFER));
	ENSURE_OK(gfx_init_sampler_state());
	ENSURE_OK(gfx_init_rasterizer(GFX_CULLING_NONE, 0));
	vec3f_t cam_eye = {0.f, 0.25f, -1.f};
	vec3f_t cam_dir = {0.f, 0.f, 0.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_new(&app->cam);
	gfx_camera_persp(&app->cam, &cam_eye, &cam_dir, &cam_up, &viewport,
			 60.f, Z_NEAR, Z_FAR);
	return RESULT_OK;
}

void app_refresh_gfx(struct application* app)
{
	gfx_render_clear(&kClearColor);
	gfx_system_bind_render_target();
	gfx_toggle_zstencil(true);
	gfx_bind_primitive_topology(GFX_TOPOLOGY_TRIANGLE_LIST);
	gfx_shader_t* vs = gfx_system_get_vertex_shader();
	gfx_system_bind_input_layout(vs);
	gfx_bind_rasterizer();
	gfx_bind_sampler_state(NULL, 0); // TODO: this func probably not correct
	size_t vertex_stride = gfx_get_vertex_stride(gfx_get_vertex_type());
	gfx_bind_vertex_buffer(app->vbuf, (u32)vertex_stride, 0);
	gfx_render_begin();
	gfx_render_end(false, 0);
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
	mat4f_t world_matrix;
	mat4f_t trans_matrix, scale_matrix;
	mat4f_identity(&world_matrix);
	mat4f_identity(&trans_matrix);
	mat4f_identity(&scale_matrix);
	const vec4f_t trans_vec = {0.f, -0.5f, 1.0f, 1.f};
	const vec4f_t scale_vec = {1.f, 1.f, 1.f, 1.f};
	mat4f_translate(&trans_matrix, &trans_vec);
	mat4f_scale(&scale_matrix, &scale_vec);
	mat4f_mul(&world_matrix, &trans_matrix, &scale_matrix);

	mat4f_t view_proj_matrix;
	mat4f_identity(&view_proj_matrix);
	mat4f_mul(&view_proj_matrix, &app->cam.view_matrix,
		  &app->cam.proj_matrix);
	mat4f_transpose(&view_proj_matrix, &view_proj_matrix);
	gfx_shader_var_t world_var = {.name = "world",
				      .type = GFX_SHADER_VAR_MAT4,
				      .data = &world_matrix};
	gfx_shader_var_t view_proj_var = {.name = "view_proj",
					  .type = GFX_SHADER_VAR_MAT4,
					  .data = &view_proj_matrix};

	asset_t* vertex_shader = NULL;
	asset_t* pixel_shader = NULL;
	asset_t* vs_pos_uv = NULL;
	asset_t* ps_pos_uv = NULL;
	ENSURE_OK(asset_manager_find("pos_color_vs", app->assets,
				     &vertex_shader));
	ENSURE_OK(
		asset_manager_find("pos_color_ps", app->assets, &pixel_shader));
	ENSURE_OK(asset_manager_find("pos_uv_vs", app->assets, &vs_pos_uv));
	ENSURE_OK(asset_manager_find("pos_uv_ps", app->assets, &ps_pos_uv));
	gfx_set_vertex_shader((gfx_shader_t*)vs_pos_uv->data);
	gfx_set_pixel_shader((gfx_shader_t*)ps_pos_uv->data);
	size_t vertex_buffer_size = (sizeof(vec3f_t) * BM_GFX_MAX_VERTICES) +
				    (sizeof(vec4f_t) * BM_GFX_MAX_VERTICES);
	app->vbuf_data = (u8*)MEM_ALLOC(vertex_buffer_size);
	memset(app->vbuf_data, 0, vertex_buffer_size);
	gfx_buffer_create(app->vbuf_data, vertex_buffer_size, GFX_BUFFER_VERTEX,
			  GFX_BUFFER_USAGE_DYNAMIC, &app->vbuf);
	struct gfx_scene* sprite = gfx_scene_new(4, 2, GFX_VERTEX_POS_UV);
	sprite->vertex_shader = (gfx_shader_t*)vs_pos_uv->data;
	sprite->pixel_shader = (gfx_shader_t*)ps_pos_uv->data;
	sprite->vert_data->positions[0] = (vec3f_t){-1.f, 1.f, 0.f};
	sprite->vert_data->positions[1] = (vec3f_t){1.f, 1.f, 0.f};
	sprite->vert_data->positions[2] = (vec3f_t){1.f, 1.f, 0.f};
	sprite->vert_data->positions[3] = (vec3f_t){-1.f, -1.f, 0.f};
	vec2f_t uv = {0.f, 0.f};
	memcpy(sprite->vert_data->tex_verts[0].data, &uv, sizeof(vec2f_t));
	uv.x = 1.f;
	uv.y = 0.f;
	memcpy(sprite->vert_data->tex_verts[1].data, &uv, sizeof(vec2f_t));
	uv.x = 1.f;
	uv.y = 1.f;
	memcpy(sprite->vert_data->tex_verts[2].data, &uv, sizeof(vec2f_t));
	uv.x = 0.f;
	uv.y = 1.f;
	memcpy(sprite->vert_data->tex_verts[3].data, &uv, sizeof(vec2f_t));
	vec_push_back(app->scenes, &sprite);
	gfx_shader_add_var(sprite->vertex_shader, &world_var);
	gfx_shader_add_var(sprite->vertex_shader, &view_proj_var);
	gfx_shader_cbuffer_new(sprite->vertex_shader);

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
		gui_create_window(window_title, 100, 100, vx, vy, 0, NULL);
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
		.fps_num = 60,
		.fps_den = 1,
		.fullscreen = false,
		.pix_fmt = GFX_FORMAT_BGRA,
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
			// if (evt.type == GUI_EVENT_MOUSE_MOTION)
			// 	printf("mx: %d my: %d\n", evt.mouse.screen_pos.x, evt.mouse.screen_pos.y);
			inp_refresh_pressed(app->inputs, &evt);
			if (inp_cmd_get_state(app->inputs, kCommandQuit))
				app->running = false;
		}
		struct mouse_device mouse;
		gui_get_global_mouse_state(&mouse);
		// printf("sx: %d sy: %d | wx: %d wy: %d\n",
		// 	mouse.screen_pos.x, mouse.screen_pos.y,
		// 	mouse.window_pos.x, mouse.window_pos.y);
		app_refresh_gfx(app);
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		BM_MEM_FREE(app->vbuf_data);
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
