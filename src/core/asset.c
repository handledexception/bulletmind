#include "core/asset.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/path.h"
#include "core/string.h"
#include "core/toml_config.h"
#include "core/vector.h"
#include "platform/platform.h"
#include "media/image.h"

#include "gfx/gfx.h"
#include "gui/gui.h"

#include <toml.h>

struct asset_manager {
	VECTOR(asset_t*) assets;
	struct asset_sytems* systems;
	toml_table_t* toml;
};

struct asset_manager* asset_manager_new(void)
{
	struct asset_manager* mgr = BM_ALLOC(sizeof(*mgr));
	asset_manager_init(mgr);
	return mgr;
}

void asset_manager_init(struct asset_manager* mgr)
{
	if (mgr != NULL) {
		vec_init(mgr->assets);
		mgr->toml = NULL;
	}
}

void asset_manager_free(struct asset_manager* mgr)
{
	if (mgr != NULL) {
		for (size_t i = 0; i < mgr->assets.num_elems; i++) {
			asset_free(mgr->assets.elems[i]);
			mgr->assets.elems[i] = NULL;
		}
		vec_free(mgr->assets);
		if (mgr->toml != NULL) {
			toml_free(mgr->toml);
			mgr->toml = NULL;
		}
		BM_FREE(mgr);
	}
}

result asset_manager_load_toml(const char* path, struct asset_manager* mgr)
{
	if (mgr == NULL)
		return RESULT_NULL;
	if (!os_path_exists(path))
		return RESULT_NOT_FOUND;

	toml_array_t* asset_array = NULL;
	if (!read_toml_config(path, &mgr->toml))
		return RESULT_ERROR;
	asset_array = toml_array_in(mgr->toml, "assets");
	if (asset_array == NULL) {
		toml_free(mgr->toml);
		return RESULT_NULL;
	}
	int num_assets = toml_array_nelem(asset_array);
	if (num_assets > MAX_ASSETS) {
		toml_free(mgr->toml);
		return RESULT_RANGE;
	}
	logger(LOG_INFO, "Found %d assets in config %s", num_assets, path);
	for (int i = 0; i < num_assets; i++) {
		toml_table_t* asset_table = toml_table_at(asset_array, i);
		if (asset_table == NULL) {
			logger(LOG_WARNING,
			       "Problem reading asset at index %d!", i);
			continue;
		}
		asset_t* asset = NULL;
		if (!asset_from_toml(asset_table, mgr, &asset)) {
			logger(LOG_ERROR, "Error making asset at index %d!", i);
			asset_free(asset);
			asset = NULL;
		} else {
			vec_push_back(mgr->assets, &asset);
		}
	}
	return RESULT_OK;
}

result asset_manager_find(const char* name, struct asset_manager* mgr,
			  asset_t** asset)
{
	if (!mgr)
		return RESULT_NULL;
	for (size_t i = 0; i < mgr->assets.num_elems; i++) {
		asset_t* ass = (asset_t*)mgr->assets.elems[i];
		if (ass != NULL && !strcmp(ass->name, name)) {
			*asset = ass;
			return RESULT_OK;
		}
	}
	return RESULT_NOT_FOUND;
}

asset_t* asset_new()
{
	asset_t* asset = BM_ALLOC(sizeof(*asset));
	asset_init(asset);
	asset_acquire(asset);
	return asset;
}

void asset_init(asset_t* asset)
{
	memset(asset->name, 0, BM_ASSET_NAME_MAX_LENGTH);
	memset(asset->path, 0, BM_MAX_PATH);
	asset->kind = ASSET_UNKNOWN;
	asset->data = NULL;
	asset->lazy_load = false;
}

void asset_free(asset_t* asset)
{
	if (asset != NULL) {
		asset_release(asset);
		s32 refs = os_atomic_get_s32(&asset->ref_count);
		if (refs > 0)
			return;

		switch (asset->kind) {
		case ASSET_SPRITE:
		case ASSET_SPRITE_FONT: {
			struct gfx_sprite* sprite =
				(struct gfx_sprite*)asset->data;
			gfx_sprite_free(sprite);
			break;
		}
		case ASSET_SPRITE_SHEET: {
			struct gfx_sheet* sheet =
				(struct gfx_sheet*)asset->data;
			gfx_sheet_free(sheet);
			break;
		}
		case ASSET_SOUND:
		case ASSET_MUSIC:
			break;
		case ASSET_SHADER:
			gfx_shader_free(asset->data);
			break;
		case ASSET_UNKNOWN:
			break;
		}

		asset_init(asset);
		BM_FREE(asset);
		asset = NULL;
	}
}

bool asset_acquire(asset_t* asset)
{
	if (asset) {
		s32 refs = os_atomic_get_s32(&asset->ref_count);
		if (refs < 0)
			os_atomic_set_s32(&asset->ref_count, 1);
		else
			os_atomic_inc_s32(&asset->ref_count);
		return true;
	}

	return false;
}

bool asset_release(asset_t* asset)
{
	if (asset) {
		os_atomic_dec_s32(&asset->ref_count);
		return true;
	}
	return false;
}

bool asset_from_toml(const toml_table_t* table, struct asset_manager* mgr,
		     asset_t** asset)
{
	bool attr_ok = false;
	const char* asset_name = NULL;
	const char* asset_path = NULL;
	const char* asset_kind_str = NULL;
	if (read_table_string(table, "name", &asset_name) &&
	    read_table_string(table, "path", &asset_path) &&
	    read_table_string(table, "kind", &asset_kind_str)) {
		attr_ok = true;
	}
	if (!attr_ok) {
		logger(LOG_ERROR, "Error reading asset attributes!");
		return false;
	}

	logger(LOG_INFO, "Asset: %s | Path: %s | Kind: %s", asset_name,
	       asset_path, asset_kind_str);

	asset_kind_t asset_kind = asset_kind_from_string(asset_kind_str);
	asset_t* new_asset = asset_new();
	new_asset->kind = asset_kind;
	strncpy(new_asset->name, asset_name,
		strnlen(asset_name, BM_ASSET_NAME_MAX_LENGTH));
	strncpy(new_asset->path, asset_path, strnlen(asset_path, BM_MAX_PATH));

	bool res = false;
	switch (asset_kind) {
	case ASSET_SPRITE:
		res = asset_make_sprite(table, new_asset);
		break;
	case ASSET_SPRITE_SHEET: {
		res = asset_make_sprite_sheet(table, new_asset);
		break;
	}
	case ASSET_SPRITE_FONT:
		logger(LOG_WARNING,
		       "asset_from_toml: sprite font not yet implemented!");
		break;
	case ASSET_SOUND:
		logger(LOG_WARNING,
		       "asset_from_toml: sound effect not yet implemented!");
		break;
	case ASSET_MUSIC:
		logger(LOG_WARNING,
		       "asset_from_toml: music not yet implemented!");
		break;
	case ASSET_SHADER:
		res = asset_make_shader(table, new_asset);
		break;
	case ASSET_UNKNOWN:
	default:
		break;
	}

	*asset = new_asset;
	return res;
}

bool asset_make_shader(const toml_table_t* table, asset_t* asset)
{
	if (asset == NULL || gfx == NULL) {
		logger(LOG_WARNING,
		       "asset_make_shader: Asset is NULL or graphics system not yet initialized!");
		return false;
	}
	const char* ext = path_get_extension(asset->path);
	if (!strcmp(ext, "hlsl")) {
		const char* entrypoint = NULL;
		const char* target = NULL;
		enum gfx_shader_type shader_type = GFX_SHADER_UNKNOWN;
		enum gfx_vertex_type vertex_type = GFX_VERTEX_UNKNOWN;
		if (cstr_contains(asset->path, ".vs")) {
			entrypoint = "VSMain";
			target = "vs_5_0";
			shader_type = GFX_SHADER_VERTEX;
			const char* vertex_type_str = NULL;
			read_table_string(table, "vertex_type",
					  &vertex_type_str);
			vertex_type =
				gfx_vertex_type_from_string(vertex_type_str);
		} else if (cstr_contains(asset->path, ".ps")) {
			entrypoint = "PSMain";
			target = "ps_5_0";
			shader_type = GFX_SHADER_PIXEL;
		}
		gfx_shader_t* shader = gfx_shader_new(shader_type);
		if (shader != NULL) {
			gfx_shader_compile_from_file(asset->path, entrypoint,
						     target, shader);
			gfx_shader_build_program(shader);
			if (shader_type == GFX_SHADER_VERTEX) {
				gfx_vertex_shader_set_vertex_type(
					(gfx_vertex_shader_t*)shader->impl,
					vertex_type);
				gfx_shader_new_input_layout(shader);
			}
			asset->data = (void*)shader;
		}
		return true;
	}
	return false;
}

bool asset_make_sprite(const toml_table_t* table, asset_t* asset)
{
	struct gfx_sprite* sprite = gfx_sprite_new();
	if (media_image_load(asset->path, sprite->img) == RESULT_OK &&
	    gfx_sprite_make_texture(sprite) == RESULT_OK) {
		asset->data = (void*)sprite;
		return true;
	}
	return false;
}

bool asset_make_sprite_sheet(const toml_table_t* table, asset_t* asset)
{
	memset(asset->path, 0, BM_MAX_PATH);

	char* ss_toml_path = NULL;
	read_table_string(table, "path", &ss_toml_path);

	toml_table_t* ss_toml = NULL;
	read_toml_config(ss_toml_path, &ss_toml);

	toml_table_t* meta = toml_table_in(ss_toml, "meta");
	char* sprite_path = NULL;
	s32 sheet_width = 0;
	s32 sheet_height = 0;
	s32 scaling = 0;
	read_table_string(meta, "path", &sprite_path);
	read_table_int32(meta, "width", &sheet_width);
	read_table_int32(meta, "height", &sheet_height);
	read_table_int32(meta, "scale", &scaling);
	strncpy(asset->path, sprite_path, strnlen(sprite_path, BM_MAX_PATH));
	// Read sprite sheet frames info
	toml_array_t* frames = toml_array_in(ss_toml, "frames");
	s32 num_frames = toml_array_nelem(frames);
	struct gfx_sheet* sheet = gfx_sheet_new(num_frames);
	for (u32 i = 0; i < num_frames; i++) {
		struct gfx_sheet_frame* frm = &sheet->frames[i];
		toml_table_t* frame_nfo = toml_table_at(frames, i);
		s32 x = 0;
		s32 y = 0;
		s32 width = 0;
		s32 height = 0;
		f64 duration = 0;
		read_table_int32(frame_nfo, "x", &frm->bbox.x);
		read_table_int32(frame_nfo, "y", &frm->bbox.y);
		read_table_int32(frame_nfo, "w", &frm->bbox.w);
		read_table_int32(frame_nfo, "h", &frm->bbox.h);
		read_table_f64(frame_nfo, "duration", &frm->duration);
		logger(LOG_INFO,
		       "Sprite Sheet Frame %d: x=%d y=%d w=%d h=%d duration=%.3f",
		       i, frm->bbox.x, frm->bbox.y, frm->bbox.w, frm->bbox.h,
		       frm->duration);
	}
	sheet->sheet_width = sheet_width;
	sheet->sheet_height = sheet_height;
	sheet->num_frames = num_frames;
	if (media_image_load(asset->path, sheet->sprite->img) == RESULT_OK &&
	    gfx_sprite_make_texture(sheet->sprite) == RESULT_OK) {
		asset->data = (void*)sheet;
		return true;
	} else {
		asset->data = NULL;
	}
	return false;
}

asset_kind_t asset_kind_from_string(const char* kind)
{
	if (!strcmp(kind, "sprite_sheet"))
		return ASSET_SPRITE_SHEET;
	if (!strcmp(kind, "sprite_font"))
		return ASSET_SPRITE_FONT;
	if (!strcmp(kind, "sprite"))
		return ASSET_SPRITE;
	if (!strcmp(kind, "sfx"))
		return ASSET_SOUND;
	if (!strcmp(kind, "music"))
		return ASSET_MUSIC;
	if (!strcmp(kind, "shader"))
		return ASSET_SHADER;
	return ASSET_MAX;
}
