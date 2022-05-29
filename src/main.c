#include <core/logger.h>
#include <core/types.h>
#include <core/utils.h>
#include <core/vector.h>
#include <platform/platform.h>
#include <gui/gui.h>
#include <gfx/gfx.h>
#include <gfx/camera.h>

#include "asset.h"
#include "command.h"
#include "input.h"

#define APP_NAME "Bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define GFX_ADAPTER_INDEX 0
#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720

static const rgba_t kClearColor = {
	.r = 0, .g = 0, .b = 0, .a = 255,
};

struct application {
	const char* name;
	u32 version;
	bool running;
	VECTOR(gui_window_t*) windows;
	struct input_state* inputs;
	struct asset_manager* assets;
	camera_t cam;
};

result app_init_gfx(struct application* app, const struct gfx_config* cfg)
{
	rect_t viewport = { .x = 0, .y = 0, .w = VIEW_WIDTH, .h = VIEW_HEIGHT };
	ENSURE_OK(gfx_init(cfg, GFX_D3D11 | GFX_USE_ZBUFFER));
	ENSURE_OK(gfx_init_sampler_state());
	ENSURE_OK(gfx_init_rasterizer(GFX_CULLING_NONE, 0));
	vec3f_t cam_eye = {0.f, 0.25f, -1.f};
	vec3f_t cam_dir = {0.f, 0.f, 0.f};
	vec3f_t cam_up = {0.f, 1.f, 0.f};
	gfx_camera_new(&app->cam);
	gfx_camera_persp(&app->cam, &cam_eye, &cam_dir, &cam_up,
			 &viewport, 60.f, Z_NEAR, Z_FAR);
	return RESULT_OK;
}

result app_init(struct application* app, s32 version, const char* asset_cfg)
{
	if (app == NULL)
		return RESULT_NULL;
	app->running = false;
	app->version = version;
	vec_init(app->windows);
	ENSURE_OK(gui_init());
	char ver_str[16];
	version_string(app->version, ver_str);
	char window_title[256];
	snprintf(window_title, (sizeof(APP_NAME) + 1 + 16) + 1, "%s %s", APP_NAME, ver_str);
	vec_push_back(app->windows, gui_create_window(window_title, 100, 100, VIEW_WIDTH, VIEW_HEIGHT, 0, NULL));
	vec_push_back(app->windows, gui_create_window("canvas_view", 0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, gui->windows.elems[0]));
	void* window_handle = gui_get_window_handle(gui->windows.elems[1]);
	const struct gfx_config gfx_cfg = {
		.window = { window_handle },
		.adapter = GFX_ADAPTER_INDEX,
		.width = VIEW_WIDTH,
		.height = VIEW_HEIGHT,
		.fps_num = 144,
		.fps_den = 1,
		.pix_fmt = GFX_FORMAT_BGRA,
		.fullscreen = false,
	};
	ENSURE_OK(app_init_gfx(app, &gfx_cfg));
	app->inputs = inp_new();
	inp_bind_virtual_key(app->inputs, kCommandQuit, SCANCODE_ESCAPE);
	inp_bind_virtual_key(app->inputs, kCommandPlayerUp, SCANCODE_W);
	inp_bind_virtual_key(app->inputs, kCommandPlayerDown, SCANCODE_S);
	inp_bind_virtual_key(app->inputs, kCommandPlayerLeft, SCANCODE_A);
	inp_bind_virtual_key(app->inputs, kCommandPlayerRight, SCANCODE_D);
	inp_bind_virtual_key(app->inputs, kCommandPlayerSpeed, SCANCODE_LSHIFT);
	app->assets = asset_manager_new();
	asset_manager_load_toml(asset_cfg, app->assets);
	return RESULT_OK;
}

void app_refresh(struct application* app)
{
	app->running = true;
	while (app->running) {
		gui_refresh();

		gui_event_t evt;
		while (gui_poll_event(&evt)) {
			inp_refresh_pressed(app->inputs, &evt);
			if (inp_cmd_get_state(app->inputs, kCommandQuit))
				app->running = false;
		}
		gfx_render_clear(&kClearColor);
		gfx_render_end(false, 0);
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		asset_manager_free(app->assets);
		inp_free(app->inputs);
		gfx_shutdown();
		gui_shutdown();
	}
}

int main(int argc, char** argv)
{
	logger(LOG_INFO, "Platform: %s", os_platform_to_string(os_get_platform()));
	logger(LOG_INFO, "Architecture: %s", os_architecture_to_string(os_get_architecture()));
	logger(LOG_INFO, "Compiler: %s", os_compiler_to_string(os_get_compiler()));
	logger(LOG_INFO, "Pointer size: %zu bytes", sizeof(intptr_t));
	struct application app;
	const u32 app_ver =
		pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	ENSURE_OK(app_init(&app, app_ver, "assets/assets.toml"));
	app_refresh(&app);
	app_shutdown(&app);
	return RESULT_OK;
}
