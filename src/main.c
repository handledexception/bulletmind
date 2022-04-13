#include <core/types.h>
#include <core/utils.h>
#include <core/vector.h>
#include <gui/gui.h>
#include <gfx/gfx.h>
#include "command.h"
#include "input.h"
#include <SDL.h>
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0

struct application {
	const char* name;
	u32 version;
	bool running;
	VECTOR(gui_window_t*) windows;
	struct gfx_system* gfx;
	struct input_state* inputs;
};

result app_init(struct application* app, s32 version)
{
	if (app == NULL)
		return RESULT_NULL;
	app->running = false;
	app->version = version;
	vec_init(app->windows);
	ENSURE_OK(gui_init());
	char ver_str[16];
	version_string(app->version, ver_str);
	vec_push_back(app->windows, gui_create_window("Bulletmind", 100, 100, 1280, 720, 0, NULL));
	vec_push_back(app->windows, gui_create_window("app_main", 100, 100, 1280, 720, 0, gui->windows.elems[0]));
	app->inputs = inp_new();
	inp_bind_virtual_key(app->inputs, kCommandQuit, SCANCODE_ESCAPE);
	inp_bind_virtual_key(app->inputs, kCommandPlayerUp, SCANCODE_W);
	inp_bind_virtual_key(app->inputs, kCommandPlayerDown, SCANCODE_S);
	inp_bind_virtual_key(app->inputs, kCommandPlayerLeft, SCANCODE_A);
	inp_bind_virtual_key(app->inputs, kCommandPlayerRight, SCANCODE_D);
	inp_bind_virtual_key(app->inputs, kCommandPlayerSpeed, SCANCODE_LSHIFT);
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
	}
}

void app_shutdown(struct application* app)
{
	if (app != NULL) {
		if (app->inputs)
			inp_free(app->inputs);
		gui_shutdown();
	}
}

int main(int argc, char** argv)
{
	struct application app;
	const u32 app_ver =
		pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);
	ENSURE_OK(app_init(&app, app_ver));
	app_refresh(&app);
	app_shutdown(&app);
	return RESULT_OK;
}
