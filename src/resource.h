#pragma once

#include "c99defs.h"
#include "types.h"

#define MAX_GAME_RESOURCES 256

// forward decl
typedef struct engine_s engine_t;
typedef struct toml_table_t toml_table_t;

typedef enum {
	ASSET_TYPE_FIRST = 0,
	kAssetTypeSprite = ASSET_TYPE_FIRST,
	kAssetTypeSpriteSheet,
	kAssetTypeSpriteFont,
	kAssetTypeAudioClip,
	ASSET_TYPE_LAST,
	ASSET_TYPE_UNKNOWN = ASSET_TYPE_LAST,
} asset_type_t;

typedef struct game_resource_s {
	char name[256];
	char path[256];
	asset_type_t type;
	void *data;
} game_resource_t;

int game_res_init(engine_t *eng);
game_resource_t *make_game_resource(engine_t *eng, const char *asset_name,
				    const char *asset_path,
				    asset_type_t asset_type);
asset_type_t asset_type_string_to_enum(const char *asset_type_str);
const char *asset_type_enum_to_string(const asset_type_t asset_type);
