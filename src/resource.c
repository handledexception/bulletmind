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

#include <assert.h>

//TODO(paulh): Need to add some string utils like concatenation!
static const char* kAssetsToml = "config/assets.toml";

//TODO(paulh): Need to add logging!
int game_res_init(engine_t* eng) {
    toml_table_t* conf = NULL;
    toml_array_t* asset_list = NULL;

    if (eng == NULL)
        return 1;

    if (!read_toml_config(kAssetsToml, &conf))
        return 1;

    asset_list = toml_array_in(conf, "assets");
    if (asset_list == NULL)
        return 1;

    const size_t num_assets = toml_array_nelem(asset_list);
    if (num_assets > MAX_GAME_RESOURCES) {
        printf("Too many assets specified in config %s\n", kAssetsToml);
        return 1;
    }

    eng->game_resources = arena_alloc(
        &mem_arena,
        sizeof(game_resource_t*) * num_assets,
        DEFAULT_ALIGNMENT
    );

    // Load the assets into game resource objects
    bool attr_ok = false;
    size_t num_assets_loaded = 0;
    for (size_t asset_idx = 0; asset_idx < num_assets; asset_idx++) {
        toml_table_t* asset = toml_table_at(asset_list, asset_idx);
        if (asset == NULL) {
            printf("Error reading asset config %zu\n", asset_idx);
            continue;
        }

        char* asset_name = NULL;
        char* asset_path = NULL;
        char* asset_type_str = NULL;

        attr_ok = read_table_string(asset, "name", &asset_name);
        if (attr_ok)
            attr_ok = read_table_string(asset, "path", &asset_path);
        if (attr_ok)
            attr_ok = read_table_string(asset, "type", &asset_type_str);

        if (!attr_ok) {
            printf("Error reading attributes from TOML!\n");
            continue;
        }

        const asset_type_t asset_type = asset_type_string_to_enum(asset_type_str);

        eng->game_resources[asset_idx] =
            make_game_resource(eng, asset_name, asset_path, asset_type);

        num_assets_loaded += 1;
    }

    if (num_assets_loaded != num_assets) {
        printf("Error loading all %zu assets!", num_assets);
        return 1;
    }

    return 0;
}

game_resource_t* make_game_resource(engine_t* eng, const char* asset_name, const char* asset_path, asset_type_t asset_type) {
    game_resource_t* resource = NULL;

    const size_t sz_path = strlen(asset_path) + 1;
    assert(sz_path <= MAX_PATH);
    if (sz_path > MAX_PATH)
        return NULL;

    // Sprite types (sprite, sprite sheet, sprite font)
    if (asset_type == ASSET_TYPE_SPRITE || asset_type == ASSET_TYPE_SPRITE_FONT) {
        sprite_t* sprite = NULL;
        if (sprite_load(asset_path, &sprite) &&
            sprite_create_texture(eng->renderer, sprite)) {

            resource = arena_alloc(
                &mem_arena,
                sizeof(game_resource_t),
                DEFAULT_ALIGNMENT
            );

            sprintf(resource->name, "%s", asset_name);
            sprintf(resource->path, "%s", asset_path);
            resource->type = asset_type;
            resource->data = sprite;
        }
    }
    else if (asset_type == ASSET_TYPE_SPRITE_SHEET) {
        toml_table_t* nfo = NULL;
        // NOTE: Sprite sheet configs originated from Aseprite JSON massaged into a TOML file
        if (read_toml_config(asset_path, &nfo)) {
            // Read sprite sheet metadata
            toml_array_t* frames = toml_array_in(nfo, "frames");
            const size_t num_frames = (size_t)toml_array_nelem(frames);

            toml_table_t* meta = toml_table_in(nfo, "meta");
            char* sprite_path = NULL;
            int32_t sheet_width = 0;
            int32_t sheet_height = 0;
            read_table_string(meta, "path", &sprite_path);
            read_table_int32(meta, "width", &sheet_width);
            read_table_int32(meta, "height", &sheet_height);

            sprite_sheet_t* sprite_sheet = arena_alloc(
                &mem_arena,
                sizeof(sprite_sheet_t),
                DEFAULT_ALIGNMENT
            );

            //TODO(paulh): need a filesystem path string processor to get base dir of path
            sprite_t* sprite = NULL;
            sprite_load(sprite_path, &sprite);

            sprite_sheet->backing_sprite = sprite;
            sprite_sheet->width = sheet_width;
            sprite_sheet->height = sheet_height;
            sprite_sheet->num_frames = num_frames;
            sprite_sheet->frames = (ss_frame_t*)arena_alloc(&mem_arena, sizeof(ss_frame_t) * num_frames, DEFAULT_ALIGNMENT);

            for (size_t i = 0; i < num_frames; i++) {
                ss_frame_t* ss_frame = &sprite_sheet->frames[i];
                toml_table_t* frame_nfo = toml_table_at(frames, i);
                int32_t x = 0;
                int32_t y = 0;
                int32_t width = 0;
                int32_t height = 0;
                double duration = 0.f;

                read_table_int32(frame_nfo, "x", &x);
                read_table_int32(frame_nfo, "y", &y);
                read_table_int32(frame_nfo, "w", &width);
                read_table_int32(frame_nfo, "h", &height);
                read_table_double(frame_nfo, "duration", &duration);

                ss_frame->bounds.x = x;
                ss_frame->bounds.y = y;
                ss_frame->bounds.w = width;
                ss_frame->bounds.h = height;
                ss_frame->duration = (float)duration;
            }

            resource = arena_alloc(
                &mem_arena,
                sizeof(game_resource_t),
                DEFAULT_ALIGNMENT
            );

            sprintf(resource->name, "%s", asset_name);
            sprintf(resource->path, "%s", asset_path);
            resource->type = asset_type;
            resource->data = sprite_sheet;
        }
    }
    else
        printf("Unknown asset type %d!\n", (int)asset_type);

    return resource;
}

asset_type_t asset_type_string_to_enum(const char* asset_type_str) {
    if (!strcmp(asset_type_str, "sprite_sheet"))
        return ASSET_TYPE_SPRITE_SHEET;
    if (!strcmp(asset_type_str, "sprite_font"))
        return ASSET_TYPE_SPRITE_FONT;
    if (!strcmp(asset_type_str, "sprite"))
        return ASSET_TYPE_SPRITE;
    if (!strcmp(asset_type_str, "audio_clip"))
        return ASSET_TYPE_AUDIO_CLIP;
    return ASSET_TYPE_UNKNOWN;
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
