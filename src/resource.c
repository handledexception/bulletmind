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

#include "audio.h"
#include "engine.h"
#include "resource.h"
#include "toml_config.h"

#include "core/logger.h"
#include "core/memory.h"
#include "core/utils.h"

#include "platform/platform.h"

#include <assert.h>

//TODO(paulh): Need to add some string utils like concatenation!
static const char* kAssetsToml = "assets/assets.toml";

//TODO(paulh): Need to add logging!
bool game_res_init(engine_t* eng)
{
	toml_table_t* conf = NULL;
	toml_array_t* asset_list = NULL;

	if (eng == NULL)
		return false;

	if (!read_toml_config(kAssetsToml, &conf))
		return false;

	logger(LOG_INFO, "Opened game resources config: %s\n", kAssetsToml);

	asset_list = toml_array_in(conf, "assets");
	if (asset_list == NULL)
		return false;

	const size_t num_assets = toml_array_nelem(asset_list);
	if (num_assets > MAX_GAME_RESOURCES) {
		logger(LOG_ERROR, "Too many assets specified in config %s\n",
		       kAssetsToml);
		return false;
	}

	logger(LOG_INFO, "Found %d assets in game resources config.",
	       num_assets);

	eng->game_resources = arena_alloc(&g_mem_arena,
					  sizeof(game_resource_t*) * num_assets,
					  DEFAULT_ALIGNMENT);

	// Load the assets into game resource objects
	bool attr_ok = false;
	size_t num_assets_loaded = 0;
	for (size_t asset_idx = 0; asset_idx < num_assets; asset_idx++) {
		toml_table_t* asset = toml_table_at(asset_list, asset_idx);
		if (asset == NULL) {
			logger(LOG_ERROR, "Error reading asset config %zu\n",
			       asset_idx);
			return false;
		}

		char* asset_name = NULL;
		char* asset_path = NULL;
		char* asset_type_str = NULL;

		attr_ok = read_table_string(asset, "name", &asset_name);
		if (attr_ok)
			attr_ok = read_table_string(asset, "path", &asset_path);
		if (attr_ok)
			attr_ok = read_table_string(asset, "type",
						    &asset_type_str);

		if (!attr_ok) {
			logger(LOG_ERROR,
			       "Error reading attributes from TOML!\n");
			return false;
		}

		if (!os_file_exists(asset_path)) {
			logger(LOG_ERROR, "Game resource file not found: %s\n",
			       asset_path);
			return false;
		}

		const asset_type_t asset_type =
			asset_type_from_string(asset_type_str);

		eng->game_resources[asset_idx] = make_game_resource(
			eng, asset_name, asset_path, asset_type);

		if (asset_type == kAssetTypeSprite) {
			s32 sprite_scale = 1;
			if (!read_table_int32(asset, "scale", &sprite_scale))
				sprite_scale = 1;
			sprite_t* s =
				(sprite_t*)eng->game_resources[asset_idx]->data;
			s->scaling = sprite_scale;
		}

		logger(LOG_INFO, "Loaded game resource: %s (%s)\n", asset_name,
		       asset_type_to_string(asset_type));

		num_assets_loaded += 1;
	}

	if (num_assets_loaded != num_assets) {
		logger(LOG_ERROR,
		       "Error loading assets! %zu/%zu assets loaded.\n",
		       num_assets_loaded, num_assets);
		return false;
	}

	logger(LOG_INFO, "Successfully loaded %zu/%zu assets.\n",
	       num_assets_loaded, num_assets);

	return true;
}

game_resource_t* make_game_resource(engine_t* eng, const char* asset_name,
				    const char* asset_path,
				    asset_type_t asset_type)
{
	game_resource_t* resource = NULL;

	const size_t sz_path = strlen(asset_path) + 1;
	assert(sz_path <= BM_MAX_PATH);
	if (sz_path > BM_MAX_PATH)
		return NULL;

	// Sprite types (sprite, sprite sheet, sprite font)
	if (asset_type == kAssetTypeSprite ||
	    asset_type == kAssetTypeSpriteFont) {
		sprite_t* sprite = NULL;
		if (sprite_load(asset_path, &sprite) &&
		    sprite_create_texture(eng->renderer, sprite)) {
			resource = arena_alloc(&g_mem_arena,
					       sizeof(game_resource_t),
					       DEFAULT_ALIGNMENT);
			sprintf(resource->name, "%s", asset_name);
			sprintf(resource->path, "%s", asset_path);
			resource->type = asset_type;
			resource->data = sprite;
		}
	} else if (asset_type == kAssetTypeSpriteSheet) {
		toml_table_t* nfo = NULL;
		// NOTE: Sprite sheet configs originated from Aseprite JSON massaged into a TOML file
		if (read_toml_config(asset_path, &nfo)) {
			// Read sprite sheet metadata
			toml_table_t* meta = toml_table_in(nfo, "meta");
			char* sprite_path = NULL;
			s32 sheet_width = 0;
			s32 sheet_height = 0;
			s32 frame_scale_factor = 1;
			read_table_string(meta, "path", &sprite_path);
			read_table_int32(meta, "width", &sheet_width);
			read_table_int32(meta, "height", &sheet_height);
			read_table_int32(meta, "scale", &frame_scale_factor);

			// Read sprite sheet frames info
			toml_array_t* frames = toml_array_in(nfo, "frames");
			const size_t num_frames =
				(size_t)toml_array_nelem(frames);

			sprite_sheet_t* sprite_sheet = arena_alloc(
				&g_mem_arena, sizeof(sprite_sheet_t),
				DEFAULT_ALIGNMENT);

			//TODO(paulh): need a filesystem path string processor to get base dir of path
			sprite_t* sprite = NULL;
			sprite_load(sprite_path, &sprite);
			sprite_create_texture(eng->renderer, sprite);
			sprite->scaling = frame_scale_factor;

			sprite_sheet->width = sheet_width;
			sprite_sheet->height = sheet_height;
			sprite_sheet->backing_sprite = sprite;
			sprite_sheet->num_frames = num_frames;
			sprite_sheet->frames = (ss_frame_t*)arena_alloc(
				&g_mem_arena, sizeof(ss_frame_t) * num_frames,
				DEFAULT_ALIGNMENT);

			// Load frame array from sprite sheet asset file
			for (size_t i = 0; i < num_frames; i++) {
				ss_frame_t* ss_frame = &sprite_sheet->frames[i];
				toml_table_t* frame_nfo =
					toml_table_at(frames, i);
				s32 x = 0;
				s32 y = 0;
				s32 width = 0;
				s32 height = 0;
				f64 duration = 0.f;

				read_table_int32(frame_nfo, "x", &x);
				read_table_int32(frame_nfo, "y", &y);
				read_table_int32(frame_nfo, "w", &width);
				read_table_int32(frame_nfo, "h", &height);
				read_table_f64(frame_nfo, "duration",
					       &duration);

				ss_frame->bbox.min.x = x;
				ss_frame->bbox.min.y = y;
				ss_frame->bbox.max.x = width;
				ss_frame->bbox.max.y = height;
				ss_frame->duration = (f32)duration;
			}

			resource = arena_alloc(&g_mem_arena,
					       sizeof(game_resource_t),
					       DEFAULT_ALIGNMENT);

			sprintf(resource->name, "%s", asset_name);
			sprintf(resource->path, "%s", asset_path);
			resource->type = asset_type;
			resource->data = sprite_sheet;
		}
	} else if (asset_type == kAssetTypeSoundEffect ||
		   asset_type == kAssetTypeMusic) {
		audio_chunk_t* audio_chunk = NULL;
		if (audio_load_sound(asset_path, &audio_chunk)) {
			resource = arena_alloc(&g_mem_arena,
					       sizeof(game_resource_t),
					       DEFAULT_ALIGNMENT);

			sprintf(resource->name, "%s", asset_name);
			sprintf(resource->path, "%s", asset_path);
			resource->type = asset_type;
			resource->data = (void*)audio_chunk;
		}
	} else
		logger(LOG_WARNING, "Unknown asset type %d!\n",
		       (int)asset_type);

	return resource;
}

asset_type_t asset_type_from_string(const char* asset_type_str)
{
	if (!strcmp(asset_type_str, "sprite_sheet"))
		return kAssetTypeSpriteSheet;
	if (!strcmp(asset_type_str, "sprite_font"))
		return kAssetTypeSpriteFont;
	if (!strcmp(asset_type_str, "sprite"))
		return kAssetTypeSprite;
	if (!strcmp(asset_type_str, "sfx"))
		return kAssetTypeSoundEffect;
	if (!strcmp(asset_type_str, "music"))
		return kAssetTypeMusic;
	return kAssetTypeMax;
}

const char* asset_type_to_string(asset_type_t type)
{
	switch (type) {
	case kAssetTypeSpriteSheet:
		return "Sprite Sheet";
	case kAssetTypeSpriteFont:
		return "Sprite Font";
	case kAssetTypeSprite:
		return "Sprite";
	case kAssetTypeSoundEffect:
		return "Sound Effect";
	case kAssetTypeMusic:
		return "Music";
	case kAssetTypeMax:
		return NULL;
	}
	return NULL;
}
