#ifndef _H_RESOURCE
#define _H_RESOURCE

#include "c99defs.h"

#define MAX_GAME_RESOURCES 256

// forward decl
typedef struct engine_s engine_t;

typedef enum {
    ASSET_TYPE_FIRST = 0,
    ASSET_TYPE_SPRITE = ASSET_TYPE_FIRST,
    ASSET_TYPE_SPRITE_SHEET,
    ASSET_TYPE_SPRITE_FONT,
    ASSET_TYPE_AUDIO_CLIP,
    ASSET_TYPE_LAST,
    ASSET_TYPE_UNKNOWN = ASSET_TYPE_LAST,
} asset_type_t;

typedef struct game_resource_s {
    char name[256];
    char path[256];
    asset_type_t type;
    void* data;
} game_resource_t;

int game_res_init(engine_t* eng);
game_resource_t* make_game_resource(const char* asset_name, const char* asset_path, asset_type_t asset_type);
asset_type_t asset_type_string_to_enum(const char* asset_type_str);
const char* asset_type_enum_to_string(asset_type_t* asset_type);
bool is_sprite(asset_type_t asset_type);

#endif
