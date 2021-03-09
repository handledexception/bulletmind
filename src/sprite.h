#pragma once

#include "c99defs.h"
#include "types.h"

#include <SDL.h>

typedef enum {
	IMG_TYPE_TARGA,
	IMG_TYPE_BMP,
	IMG_TYPE_PNG,
	IMG_TYPE_JPEG,
	IMG_TYPE_RAW,
} imgtype_t;

typedef enum {
	BGR24,
	RGB24,
	RGBA32,
	BGRA32,
	ARGB32,
} pix_fmt_t;

typedef struct sprite_s {
	u8 *data;
	imgtype_t type;
	pix_fmt_t pix_fmt;
	SDL_Surface *surface;
	SDL_Texture *texture;
	i32 scaling;
	bool has_alpha;
} sprite_t;

#define MAX_SPRITE_SHEET_FRAMES 32

typedef struct ss_frame_s {
	SDL_Rect bounds;
	f32 duration;
} ss_frame_t;

typedef struct sprite_sheet_s {
	i32 width;
	i32 height;
	sprite_t *backing_sprite;
	size_t num_frames;
	ss_frame_t *frames;
} sprite_sheet_t;

bool sprite_load(const char *path, sprite_t **out);
void sprite_create(u8 *data, u32 w, u32 h, u32 bpp, u32 stride, u32 format,
		   sprite_t **out);
bool sprite_create_texture(SDL_Renderer *ren, sprite_t *img);
void sprite_shutdown(sprite_t *img);
