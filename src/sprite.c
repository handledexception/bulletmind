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

#include "sprite.h"
#include "core/mem_arena.h"
#include "core/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tga_header_s {
	u8 id_length;      // field 1
	u8 color_map_type; // field 2
	u8 image_type;     // field 3

	u8 color_map_spec[5]; // field 4
	/*
        u16 first_entry_index;
        u16 color_map_length;
        u8 color_map_entry_size;
    */

	u16 x_org; // field 5
	u16 y_org;
	u16 width;
	u16 height;
	u8 bpp;
	u8 desc; // bits 3-0 = alpha channel depth, bits 5-4 img direction
} tga_header_t;

bool sprite_load(const char* path, sprite_t** out)
{
	FILE* file_ptr = NULL;
	size_t fsize = 0;
	u8* file_buf = NULL;
	sprite_t* img = NULL;

	if (path == NULL || out == NULL)
		return false;

	// absolute most janky file extension comparison
	const char* file_ext = file_extension(path);
	if (strcmp(file_ext, "tga") == 0) {
		img = (sprite_t*)arena_alloc(&g_mem_arena, sizeof(sprite_t),
					     DEFAULT_ALIGNMENT);
		img->type = IMG_TYPE_TARGA;

		file_ptr = fopen(path, "rb");
		fseek(file_ptr, 0, SEEK_END);
		fsize = ftell(file_ptr);
		fseek(file_ptr, 0, SEEK_SET);
		file_buf = (u8*)malloc(fsize);
		if (file_ptr == NULL) {
			printf("sprite_load - file %s has no data!\n", path);
			return false;
		} else if (fread(file_buf, sizeof(u8), fsize, file_ptr) !=
			   fsize) {
			printf("sprite_load - could not read to end of file %s\n",
			       path);
			free(file_buf);
			file_buf = NULL;
			return false;
		}

		tga_header_t* header = (tga_header_t*)file_buf;
		size_t tga_header_size = sizeof(*header);
		// make sure we have a valid minimal TGA header and raw unmapped RGB data
		if (tga_header_size != 18 || header->color_map_type > 0 ||
		    header->image_type != 2) {
			printf("sprite_load - Incorrect TGA header size! (%zu bytes) Should be 18 bytes.\n",
			       tga_header_size);
			free(file_buf);
			file_buf = NULL;
			return false;
		}

		u16 width = header->width;
		u16 height = header->height;
		u8 bytes_per_pixel = header->bpp / 8;
		s32 stride = width * bytes_per_pixel;
		size_t pixel_size = width * height * bytes_per_pixel;

		img->data = (u8*)arena_alloc(&g_mem_arena, pixel_size,
					     DEFAULT_ALIGNMENT);

		printf("sprite_load - %s, %dx%d %d bytes per pixel\n", path,
		       width, height, bytes_per_pixel);

		u8* tga_pixels = file_buf + tga_header_size;

		// origin bit 1 = upper-left origin pixel
		if ((header->desc >> 5) & 1) {
			memcpy(img->data, tga_pixels, pixel_size);
		}
		// origin bit 0 = lower-left origin pixel
		else {
			for (s32 c = 0; c < height; c++) {
				memcpy(img->data + stride * c,
				       tga_pixels + stride * (height - (c + 1)),
				       stride);
			}
		}

		u32 pix_fmt = SDL_PIXELFORMAT_BGR24;
		if (bytes_per_pixel == 3) {
			pix_fmt = SDL_PIXELFORMAT_BGR24;
			img->pix_fmt = BGR24;
			img->has_alpha = false;
		} else if (bytes_per_pixel == 4) {
			pix_fmt = SDL_PIXELFORMAT_BGRA32;
			img->pix_fmt = BGRA32;
			img->has_alpha = true;
		}

		img->surface = SDL_CreateRGBSurfaceWithFormatFrom(
			img->data, width, height, header->bpp, stride, pix_fmt);

		free(file_buf);
		fclose(file_ptr);

		*out = img;
	} else {
		return false;
	}

	return true;
}

void sprite_create(u8* data, u32 w, u32 h, u32 bpp, u32 stride, u32 format,
		   sprite_t** out)
{
	sprite_t* img = (sprite_t*)arena_alloc(&g_mem_arena, sizeof(sprite_t),
					       DEFAULT_ALIGNMENT);
	img->type = IMG_TYPE_RAW;
	size_t pixel_size = w * h * (bpp / 8);
	img->data =
		(u8*)arena_alloc(&g_mem_arena, pixel_size, DEFAULT_ALIGNMENT);
	memcpy(img->data, data, pixel_size);
	img->surface = SDL_CreateRGBSurfaceWithFormatFrom(img->data, w, h, bpp,
							  stride, format);

	*out = img;
}

bool sprite_create_texture(SDL_Renderer* ren, sprite_t* img)
{
	bool res = false;

	if (img != NULL) {
		img->texture = SDL_CreateTexture((SDL_Renderer*)ren,
						 img->surface->format->format,
						 SDL_TEXTUREACCESS_STATIC,
						 img->surface->w,
						 img->surface->h);

		if (img->texture != NULL) {
			SDL_UpdateTexture(img->texture, NULL,
					  img->surface->pixels,
					  img->surface->pitch);

			SDL_SetTextureBlendMode(img->texture,
						img->has_alpha
							? SDL_BLENDMODE_BLEND
							: SDL_BLENDMODE_NONE);

			res = true;
		} else {
			SDL_DestroyTexture(img->texture);
			img->texture = NULL;
		}
	}

	return res;
}

void sprite_shutdown(sprite_t* img)
{
	if (img != NULL) {
		if (img->surface != NULL) {
			SDL_FreeSurface(img->surface);
			img->surface = NULL;
		}
		if (img->texture != NULL) {
			SDL_DestroyTexture(img->texture);
			img->texture = NULL;
		}
		printf("imagefile_shutdown: OK!\n");
	}
}
