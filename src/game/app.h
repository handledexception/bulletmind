#pragma once

#include "core/types.h"
#include "core/vector.h"

struct asset_manager;
struct gfx_config;
struct input_state;
typedef struct gfx_scene gfx_scene_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gui_window gui_window_t;
typedef struct entity entity_t;
typedef struct imgui_draw_data imgui_draw_data_t;
typedef struct camera_s camera_t;

typedef enum {
	GAME_STATE_INIT,
	GAME_STATE_RUN,
	GAME_STATE_EXIT,
} game_state_t;

struct fps_cam {
	f32 yaw;
	f32 pitch;
	f32 prev_pitch;
	f32 sensitivity;
	camera_t* cam;
};

struct application {
	const char* name;
	u32 version;
	game_state_t game_state;
	entity_t* entities;
	gui_window_t* window;
	struct input_state* inputs;
	struct asset_manager* assets;
	struct fps_cam cam;
	imgui_draw_data_t* ig_draw;
	VECTOR(gfx_scene_t*) scenes;
	gfx_buffer_t* vbuf;
	gfx_buffer_t* ibuf;
	uint64_t frame_count;
    f64 time_start;
	f64 frame_timer;
	f64 frame_time_start;
	f64 frame_time_end;
};

result app_init(struct application* app, const char* name, s32 version, u32 vx, u32 vy,
		const char* assets_toml_path);
result app_init_gfx(struct application* app, const struct gfx_config* cfg);
result app_init_inputs(struct application* app);
result app_init_scenes(struct application* app);
result app_init_assets(struct application* app, const char* assets_toml_path);
void app_init_game(struct application* app);
void app_refresh(struct application* app);
void app_refresh_entities(struct application* app);
void app_refresh_mouse(struct application* app);
void app_toggle_mouse_capture(struct application* app);
void app_shutdown(struct application* app);
void app_frame_step(struct application* app);
void app_frame_start(struct application* app);
void app_frame_end(struct application* app);
f64 app_frame_time(struct application* app);
