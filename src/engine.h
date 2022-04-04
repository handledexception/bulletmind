/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

// #include "entity.h"
#include "font.h"
#include "sprite.h"

#include "core/vector.h"

#include "math/types.h"

#include "gfx/camera.h"
#include "gfx/gfx.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;

struct input_state;
typedef struct game_resource_s game_resource_t;
typedef struct audio_state_s audio_state_t;

typedef enum {
	kEngineModeStartup,
	kEngineModePlay,
	kEngineModeConsole,
	kEngineModeQuit,
	kEngineModeShutdown
} engine_mode_t;

#define MAX_SPAWN_TIMERS 1024
#define DEFAULT_SFX_VOLUME 12
#define DEFAULT_MUSIC_VOLUME 25

typedef struct entity_s entity_t;
typedef struct gfx_system gfx_system_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_shader_var gfx_shader_var_t;
struct gfx_scene;
struct gui_window;

struct engine_gfx {
	gfx_system_t* system;
	camera_t camera;
	VECTOR(struct gfx_scene*) scenes;
	gfx_buffer_t* vertex_buffer;
	u8* vbuffer_data;
	gfx_buffer_t* cbuffer; // TODO(paulh): should go with shader(s)?
	u8* cbuffer_data;
	size_t cbuffer_size;
};

typedef struct engine_s {
	s32 adapter_index;
	f32 target_fps;
	f64 target_frametime;
	u64 frame_count;
	VECTOR(struct gui_window*) windows;
	bool fullscreen;
	struct engine_gfx gfx;
	struct input_state* inputs;
	rect_t window_rect;
	rect_t cam_rect;
	rect_t cam_inset;
	vec2f_t render_scale;
	f64 spawn_timer[MAX_SPAWN_TIMERS];
	engine_mode_t mode;
	bool debug;
	bool console;
	entity_t* ent_list;
	game_resource_t** game_resources;
} engine_t;

extern engine_t* engine;

bool eng_init(const char* name, s32 version, engine_t* eng);
void eng_refresh_commands(engine_t* eng);
void eng_refresh(engine_t* eng, f64 dt);
void eng_shutdown(engine_t* eng);

void eng_init_time(void);
u64 eng_get_time_ns(void);
f64 eng_get_time_sec(void);

game_resource_t* eng_get_resource(engine_t* eng, const char* name);

void eng_play_sound(engine_t* eng, const char* name, s32 volume);
void eng_stop_music(engine_t* eng);

void eng_toggle_fullscreen(engine_t* eng, bool fullscreen);
