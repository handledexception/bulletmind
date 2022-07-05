#pragma once
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

#include "core/types.h"

#define MAX_ASSETS 1024
#define BM_ASSET_NAME_MAX_LENGTH 1024

// forward decl
typedef struct toml_table_t toml_table_t;

typedef enum {
	ASSET_SPRITE,
	ASSET_SPRITE_SHEET,
	ASSET_SPRITE_FONT,
	ASSET_SOUND,
	ASSET_MUSIC,
	ASSET_SHADER,
	ASSET_UNKNOWN,
	ASSET_MAX = ASSET_UNKNOWN
} asset_kind_t;

typedef struct asset asset_t;
typedef struct asset_manager asset_manager_t;

struct asset {
	char name[BM_ASSET_NAME_MAX_LENGTH];
	char path[BM_MAX_PATH];
	asset_kind_t kind;
	void* data;
	bool lazy_load;
};

struct asset_manager* asset_manager_new(void);
void asset_manager_init(struct asset_manager* mgr);
void asset_manager_free(struct asset_manager* mgr);
result asset_manager_load_toml(const char* path, struct asset_manager* mgr);
result asset_manager_find(const char* name, struct asset_manager* mgr,
			  asset_t** asset);

asset_t* asset_new();
void asset_free(asset_t* asset);
void asset_init(asset_t* asset);
bool asset_from_toml(const toml_table_t* table, struct asset_manager* mgr,
		     asset_t** asset);
bool asset_make_shader(const toml_table_t* table, asset_t* asset);
bool asset_make_sprite(const toml_table_t* table, asset_t* asset);
asset_kind_t asset_kind_from_string(const char* kind);
