#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "resource.h"
#include "memarena.h"
#include "engine.h"
#include "toml_config.h"
#include "utils.h"

#include "platform/platform.h"

#include <assert.h>

//TODO(paulh): Need to add some string utils like concatenation!
static const char *kAssetsToml = "config/assets.toml";

//TODO(paulh): Need to add logging!
int game_res_init(engine_t *eng)
{
	toml_table_t *conf = NULL;
	toml_array_t *asset_list = NULL;

	if (eng == NULL)
		return 1;

	if (!read_toml_config(kAssetsToml, &conf))
		return 1;

	printf("Opened game resources config: %s\n", kAssetsToml);

	asset_list = toml_array_in(conf, "assets");
	if (asset_list == NULL)
		return 1;

	const size_t num_assets = toml_array_nelem(asset_list);
	if (num_assets > MAX_GAME_RESOURCES) {
		printf("Too many assets specified in config %s\n", kAssetsToml);
		return 1;
	}

	printf("Found %d assets in game resources config.", num_assets);

	eng->game_resources = arena_alloc(
		&g_mem_arena, sizeof(game_resource_t *) * num_assets,
		DEFAULT_ALIGNMENT);

	// Load the assets into game resource objects
	bool attr_ok = false;
	size_t num_assets_loaded = 0;
	for (size_t asset_idx = 0; asset_idx < num_assets; asset_idx++) {
		toml_table_t *asset = toml_table_at(asset_list, asset_idx);
		if (asset == NULL) {
			printf("Error reading asset config %zu\n", asset_idx);
			continue;
		}

		char *asset_name = NULL;
		char *asset_path = NULL;
		char *asset_type_str = NULL;

		attr_ok = read_table_string(asset, "name", &asset_name);
		if (attr_ok)
			attr_ok = read_table_string(asset, "path", &asset_path);
		if (attr_ok)
			attr_ok = read_table_string(asset, "type",
						    &asset_type_str);

		if (!attr_ok) {
			printf("Error reading attributes from TOML!\n");
			continue;
		}

		if (!os_file_exists(asset_path)) {
			printf("Game resource file not found: %s\n", asset_path);
			continue;
		}

		const asset_type_t asset_type =
			asset_type_from_string(asset_type_str);

		eng->game_resources[asset_idx] = make_game_resource(
			eng, asset_name, asset_path, asset_type);

		printf("Loaded game resource: %s (%s)\n", asset_name, asset_type_to_string(asset_type));

		num_assets_loaded += 1;
	}

	if (num_assets_loaded != num_assets) {
		printf("Error loading all %zu assets!", num_assets);
		return 1;
	}

	return 0;
}

game_resource_t *make_game_resource(engine_t *eng, const char *asset_name,
				    const char *asset_path,
				    asset_type_t asset_type)
{
	game_resource_t *resource = NULL;

	const size_t sz_path = strlen(asset_path) + 1;
	assert(sz_path <= MAX_PATH);
	if (sz_path > MAX_PATH)
		return NULL;

	// Sprite types (sprite, sprite sheet, sprite font)
	if (asset_type == kAssetTypeSprite ||
	    asset_type == kAssetTypeSpriteFont) {
		sprite_t *sprite = NULL;
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
		toml_table_t *nfo = NULL;
		// NOTE: Sprite sheet configs originated from Aseprite JSON massaged into a TOML file
		if (read_toml_config(asset_path, &nfo)) {
			// Read sprite sheet metadata
			toml_table_t *meta = toml_table_in(nfo, "meta");
			char *sprite_path = NULL;
			i32 sheet_width = 0;
			i32 sheet_height = 0;
			i32 frame_scale_factor = 1;
			read_table_string(meta, "path", &sprite_path);
			read_table_int32(meta, "width", &sheet_width);
			read_table_int32(meta, "height", &sheet_height);
			read_table_int32(meta, "scale", &frame_scale_factor);

			// Read sprite sheet frames info
			toml_array_t *frames = toml_array_in(nfo, "frames");
			const size_t num_frames =
				(size_t)toml_array_nelem(frames);

			sprite_sheet_t *sprite_sheet = arena_alloc(
				&g_mem_arena, sizeof(sprite_sheet_t),
				DEFAULT_ALIGNMENT);

			//TODO(paulh): need a filesystem path string processor to get base dir of path
			sprite_t *sprite = NULL;
			sprite_load(sprite_path, &sprite);
			sprite_create_texture(eng->renderer, sprite);
			sprite->scaling = frame_scale_factor;

			sprite_sheet->width = sheet_width;
			sprite_sheet->height = sheet_height;
			sprite_sheet->backing_sprite = sprite;
			sprite_sheet->num_frames = num_frames;
			sprite_sheet->frames = (ss_frame_t *)arena_alloc(
				&g_mem_arena, sizeof(ss_frame_t) * num_frames,
				DEFAULT_ALIGNMENT);

			// Load frame array from sprite sheet asset file
			for (size_t i = 0; i < num_frames; i++) {
				ss_frame_t *ss_frame = &sprite_sheet->frames[i];
				toml_table_t *frame_nfo =
					toml_table_at(frames, i);
				i32 x = 0;
				i32 y = 0;
				i32 width = 0;
				i32 height = 0;
				f64 duration = 0.f;

				read_table_int32(frame_nfo, "x", &x);
				read_table_int32(frame_nfo, "y", &y);
				read_table_int32(frame_nfo, "w", &width);
				read_table_int32(frame_nfo, "h", &height);
				read_table_f64(frame_nfo, "duration",
					       &duration);

				ss_frame->bounds.x = x;
				ss_frame->bounds.y = y;
				ss_frame->bounds.w = width;
				ss_frame->bounds.h = height;
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
	}
	else if (asset_type == kAssetTypeAudioClip) {

	}
	else
		printf("Unknown asset type %d!\n", (int)asset_type);

	return resource;
}

asset_type_t asset_type_from_string(const char *asset_type_str)
{
	if (!strcmp(asset_type_str, "sprite_sheet"))
		return kAssetTypeSpriteSheet;
	if (!strcmp(asset_type_str, "sprite_font"))
		return kAssetTypeSpriteFont;
	if (!strcmp(asset_type_str, "sprite"))
		return kAssetTypeSprite;
	if (!strcmp(asset_type_str, "audio_clip"))
		return kAssetTypeAudioClip;
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
	case kAssetTypeAudioClip:
		return "Audio Clip";
	case kAssetTypeMax:
		return NULL;
	}
	return NULL;
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
