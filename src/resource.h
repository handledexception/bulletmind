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

#include "core/c99defs.h"
#include "core/types.h"

#define MAX_GAME_RESOURCES 256

// forward decl
typedef struct engine_s engine_t;
typedef struct toml_table_t toml_table_t;

typedef enum {
	kAssetTypeSprite,
	kAssetTypeSpriteSheet,
	kAssetTypeSpriteFont,
	kAssetTypeAudioClip,
	kAssetTypeMax
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
asset_type_t asset_type_from_string(const char *asset_type_str);
const char *asset_type_to_string(asset_type_t type);
const char *asset_type_enum_to_string(const asset_type_t asset_type);
