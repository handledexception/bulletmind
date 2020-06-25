#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "resource.h"
#include "memarena.h"
#include "engine.h"
#include "toml_config.h"
#include "utils.h"

#include <assert.h>

//TODO(paulh): Need to add some string utils like concatenation!
static const char* kAssetsToml = "config/assets.toml";

static const asset_type_t sprite_types[3] = {
    ASSET_TYPE_SPRITE,
    ASSET_TYPE_SPRITE_SHEET,
    ASSET_TYPE_SPRITE_FONT,
};

int game_res_init(engine_t* eng) {
    toml_table_t* conf = NULL;
    toml_array_t* asset_list = NULL;

    if (!read_toml_config(kAssetsToml, &conf))
        return 1;

    asset_list = toml_array_in(conf, "assets");
    if (asset_list) {
        eng->game_resources = arena_alloc(
            &mem_arena,
            sizeof(game_resource_t*) * MAX_GAME_RESOURCES,
            DEFAULT_ALIGNMENT
        );

        const size_t num_assets = toml_array_nelem(asset_list);
        if (num_assets > MAX_GAME_RESOURCES) {
            printf("Too many assets specified in config %s\n", kAssetsToml);
            return 1;
        }

        for (size_t i = 0; i < num_assets; i++) {
            toml_table_t* asset = toml_table_at(asset_list, i);

            char* asset_name = NULL;
            char* asset_path = NULL;
            char* asset_type_str = NULL;

            read_table_string(asset, "name", &asset_name);
            read_table_string(asset, "path", &asset_path);
            read_table_string(asset, "type", &asset_type_str);

            const asset_type_t asset_type = asset_type_string_to_enum(asset_type_str);

            eng->game_resources[i] = make_game_resource(asset_name, asset_path, asset_type);
        }
    }
    else {
        printf("No assets found in %s\n!", kAssetsToml);
        return 1;
    }

    return 0;
}

game_resource_t* make_game_resource(const char* asset_name, const char* asset_path, asset_type_t asset_type) {
    game_resource_t* resource = NULL;

    const size_t sz_path = strlen(asset_path) + 1;
    assert(sz_path <= MAX_PATH);
    if (sz_path > MAX_PATH)
        return NULL;

    // Sprite types (sprite, sprite sheet, sprite font)
    if (is_sprite(asset_type)) {
        if (asset_type == ASSET_TYPE_SPRITE || asset_type == ASSET_TYPE_SPRITE_FONT) {
            sprite_t* sprite = NULL;
            if (sprite_load(asset_path, &sprite) &&
                sprite_create_texture(engine->renderer, sprite)) {

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
            // NOTE: Sprite sheet configs are JSON from Aseprite converted to a TOML file
            if (read_toml_config(asset_path, &nfo)) {
                toml_table_t* meta = toml_table_in(nfo, "meta");
                toml_array_t* frames = toml_array_in(nfo, "frames");
                sprite_t* sprite = NULL;
                sprite_load(asset_path, &sprite);
            }
        }
    }
    else
        printf("Unknown asset type!\n");

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

bool is_sprite(asset_type_t asset_type) {
    for (size_t tdx = 0; tdx < ARRAY_SIZE(sprite_types); tdx++)
        if (asset_type == sprite_types[tdx])
            return true;
    return false;
}

#undef _CRT_SECURE_NO_WARNINGS
