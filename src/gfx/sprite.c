#include "gfx/gfx.h"
#include "media/image.h"
#include "math/types.h"

struct gfx_sprite* gfx_sprite_new()
{
	struct gfx_sprite* sprite =
		(struct gfx_sprite*)BM_ALLOC(sizeof(*sprite));
	sprite->img = media_image_new();
	sprite->tex = NULL;
	return sprite;
}

void gfx_sprite_free(struct gfx_sprite* sprite)
{
	if (sprite != NULL) {
		if (sprite->img != NULL) {
			media_image_free(sprite->img);
			sprite->img = NULL;
		}
		if (sprite->tex != NULL) {
			gfx_texture_destroy(sprite->tex);
		}
		BM_FREE(sprite);
		sprite = NULL;
	}
}

result gfx_sprite_make_texture(struct gfx_sprite* sprite)
{
	result res = RESULT_OK;
	if (sprite != NULL) {
		if (sprite->img != NULL) {
			res = gfx_texture_from_image(sprite->img, &sprite->tex);
		}
	}
	return res;
}

struct gfx_sheet* gfx_sheet_new(u32 num_frames)
{
	struct gfx_sheet* sheet = (struct gfx_sheet*)BM_ALLOC(sizeof(*sheet));
	sheet->sprite = gfx_sprite_new();
	sheet->frames = BM_ALLOC(sizeof(struct gfx_sheet_frame) * num_frames);
	sheet->cel_height = 0;
	sheet->cel_width = 0;
	sheet->sheet_height = 0;
	sheet->sheet_width = 0;
	sheet->num_frames = num_frames;
	return sheet;
}

void gfx_sheet_free(struct gfx_sheet* sheet)
{
	if (sheet != NULL) {
		if (sheet->sprite != NULL) {
			gfx_sprite_free(sheet->sprite);
		}
		if (sheet->frames != NULL) {
			BM_FREE(sheet->frames);
			sheet->frames = NULL;
		}
		BM_FREE(sheet);
		sheet = NULL;
	}
}
