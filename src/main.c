/*
 * Copyright (c) 2022 Paul Hindt
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

// bulletmind
// (c) 2019-2022 Paul Hindt
// v1.1644998400

#define APP_NAME "bulletmind"
#define APP_VER_MAJ 1
#define APP_VER_MIN 0
#define APP_VER_REV 0
#define APP_VER_KIND "dev"

#include "core/logger.h"
#include "core/memory.h"
#include "core/string.h"
#include "core/time_convert.h"
#include "core/utils.h"
#include "core/vector.h"

#include "math/vec2.h"
#include "math/utils.h"

#include "platform/platform.h"

#include "entity.h"
#include "font.h"
#include "sprite.h"
#include "input.h"
#include "engine.h"
#include "resource.h"
#include "render.h"
#include <SDL.h>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH_HALF WINDOW_WIDTH / 2
#define WINDOW_HEIGHT_HALF WINDOW_HEIGHT / 2

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480
#define CAMERA_INSET_WIDTH (CAMERA_WIDTH - 64)
#define CAMERA_INSET_HEIGHT (CAMERA_HEIGHT - 64)
#define CAMERA_WIDTH_HALF CAMERA_WIDTH / 2
#define CAMERA_HEIGHT_HALF CAMERA_HEIGHT / 2
#define TARGET_FPS 144.0

#define WORLD_TILES_WIDTH 16
#define WORLD_TILES_HEIGHT 16
#define TILE_WIDTH 64
#define TILE_HEIGHT 64

#define CONSOLE_SPEED 10

static u8 world_map[WORLD_TILES_WIDTH * WORLD_TILES_HEIGHT] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static sprite_t* tilemap_sprite = NULL;

void generate_tilemap(u32 width, u32 height)
{
	// world_map = (u8*)malloc(width * height);
	// for (size_t i = 0; i < width * width; i++) {
	// 	world_map[i] = rand() % 2;
	// }
	tilemap_sprite = (sprite_t*)malloc(sizeof(sprite_t));
	tilemap_sprite->type = IMG_TYPE_TARGA;
	tilemap_sprite->has_alpha = false;
	tilemap_sprite->pix_fmt = RGB24;
	tilemap_sprite->surface = NULL;
}

sprite_t* world_map_tile_index(engine_t* engine, rect_t* tile_rect,
			       rect_t* camera)
{
	s32 index = tile_rect->x + (tile_rect->y * WORLD_TILES_WIDTH);
	sprite_t* tile = NULL;
	game_resource_t* resource = NULL;
	switch (world_map[index]) {
	case 0:
		resource = eng_get_resource(engine, "tiled_wall_64x64");
		tile = (sprite_t*)resource->data;
		break;
	case 1:
		resource = eng_get_resource(engine, "machines_floor_64x64");
		tile = (sprite_t*)resource->data;
		break;
		// case 2:
		// 	resource = eng_get_resource(engine, "shadowy_wall_16x16");
		// 	break;
		// case 3:
		// 	resource = eng_get_resource(engine, "grass_dirt_block");
		// 	tile = (sprite_t *)resource->data;
		// 	break;
	}

	tile->surface->clip_rect.x =
		tile_rect->x * tile_rect->w; // - camera->x;
	tile->surface->clip_rect.y =
		tile_rect->y * tile_rect->h; // - camera->y;

	return tile;
}

int main(int argc, char** argv)
{
	char s[26] = "\"Main screen turn on...\"";
	str_upper_no_copy(s, 0);
	logger(LOG_INFO, "%s\n", s);

	// Allocate memory arena - 8MiB
	mem_arena_backing_buffer = (u8*)mem_alloc(ARENA_TOTAL_BYTES);
	mem_arena_init(&mem_arena, (void*)mem_arena_backing_buffer,
		       (size_t)ARENA_TOTAL_BYTES);

	size_t sz_engine = sizeof(engine_t);
	engine = (engine_t*)bm_mem_arena_alloc(&mem_arena, sz_engine);
	memset(engine, 0, sizeof(engine_t));
	engine->inputs =
		bm_mem_arena_alloc(&mem_arena, sizeof(struct input_state));
	engine->adapter_index = -1; // SDL default to first available
	engine->window_rect.x = -1; // SDL window position centered
	engine->window_rect.y = -1; // ""
	engine->window_rect.w = WINDOW_WIDTH;
	engine->window_rect.h = WINDOW_HEIGHT;
	engine->cam_rect.x = 0;
	engine->cam_rect.y = 0;
	engine->cam_rect.w = CAMERA_WIDTH;
	engine->cam_rect.h = CAMERA_HEIGHT;
	engine->cam_inset.x = 0;
	engine->cam_inset.y = 0;
	engine->cam_inset.w = CAMERA_INSET_WIDTH;
	engine->cam_inset.h = CAMERA_INSET_HEIGHT;
	center_in_rect(&engine->cam_inset, &engine->cam_rect,
		       &engine->cam_inset);
	engine->render_scale.x = (f32)WINDOW_WIDTH / (f32)CAMERA_WIDTH;
	engine->render_scale.y = (f32)WINDOW_HEIGHT / (f32)CAMERA_HEIGHT;
	engine->target_fps = TARGET_FPS;
	for (size_t i = 0; i < MAX_SPAWN_TIMERS; i++)
		engine->spawn_timer[i] = 0.0;
#if defined(BM_DEBUG)
	engine->debug = true;
#else
	engine->debug = false;
#endif
	engine->fullscreen = false;
	engine->console = false;

	const u32 app_version =
		pack_version(APP_VER_MAJ, APP_VER_MIN, APP_VER_REV);

	if (!eng_init(APP_NAME, app_version, engine)) {
		logger(LOG_ERROR, "Something went wrong!\n");
		return -1;
	}

	// main loop
	f64 dt = 0.0;
	while (engine->mode != kEngineModeShutdown) {
		u64 frame_start_ns = os_get_time_ns();

		switch (engine->mode) {
		case kEngineModeStartup: {
			ent_spawn_player_and_satellite(engine->ent_list,
						       engine->cam_rect.w,
						       engine->cam_rect.h);
			engine->mode = kEngineModePlay;
			break;
		}
		case kEngineModePlay:
		case kEngineModeConsole: {
			eng_refresh(engine, dt);
			break;
		}
		default:
		case kEngineModeQuit: {
			engine->mode = kEngineModeShutdown;
			break;
		}
		}

		do {
			dt = nsec_to_sec_f64(os_get_time_ns() - frame_start_ns);
			if (dt > FRAME_TIME(5)) {
				dt = FRAME_TIME(5);
			}
		} while (dt < engine->target_frametime);
		//printf("%f\n", dt);
		engine->frame_count++;
	}

	eng_shutdown(engine);
	engine = NULL;
	mem_free(mem_arena_backing_buffer);
	mem_arena_backing_buffer = NULL;

	return 0;
}
