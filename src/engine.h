#pragma once

#include "c99defs.h"
#include "types.h"
#include "entity.h"
#include "sprite.h"
#include "vector.h"

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;

typedef struct input_state_s input_state_t;

typedef struct game_resource_s game_resource_t;

typedef enum {
	kEngineStateStartup,
	kEngineStatePlay,
	kEngineStateQuit,
} engine_state_t;

typedef struct engine_s {
	i32 adapter_index;

	SDL_Window *window;
	SDL_Renderer *renderer;
	// SDL_Surface* scr_surface;
	// SDL_Texture* scr_texture;

	i32 wnd_width, wnd_height;
	i32 scr_width, scr_height;
	f32 scr_scale_x, scr_scale_y;
	rect_t scr_bounds;

	f32 target_fps;
	f64 target_frametime;
	i32 frame_count;

	engine_state_t state;
	bool debug;

	entity_t *ent_list;
	game_resource_t **game_resources;

	input_state_t *inputs;
} engine_t;

extern engine_t *engine;

bool eng_init(const char *name, i32 version, engine_t *eng);
void eng_refresh(engine_t *eng);
void eng_shutdown(engine_t *eng);

void eng_init_timing(void);
f64 eng_get_time(void);

game_resource_t *eng_get_resource(engine_t *eng, const char *name);
