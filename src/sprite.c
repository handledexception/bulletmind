#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "sprite.h"
#include "memarena.h"
#include "utils.h"

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
	FILE* fptr = NULL;
	size_t fsize = 0;
	u8* fbuf = NULL;
	sprite_t* img = NULL;

	if (path == NULL || out == NULL)
		return false;

	// absolute most janky file extension comparison
	const char* file_ext = file_extension(path);
	if (strcmp(file_ext, "tga") == 0) {
		img = (sprite_t*)arena_alloc(&mem_arena, sizeof(sprite_t), DEFAULT_ALIGNMENT);
		img->type = IMG_TYPE_TARGA;

		fptr = fopen(path, "rb");
		fseek(fptr, 0, SEEK_END);
		fsize = ftell(fptr);
		fseek(fptr, 0, SEEK_SET);
		fbuf = (u8*)malloc(fsize);
		if (fptr == NULL) {
			printf("sprite_load: file %s has no data!\n", path);
			return false;
		} else if (fread(fbuf, sizeof(u8), fsize, fptr) != fsize) {
			printf("sprite_load: could not read to end of file %s\n", path);
			free(fbuf);
			fbuf = NULL;
			return false;
		}

		tga_header_t* header = (tga_header_t*)fbuf;
		size_t tga_header_size = sizeof(*header);
		// make sure we have a valid minimal TGA header and raw unmapped RGB data
		if (tga_header_size != 18 || header->color_map_type > 0 ||
		    header->image_type != 2) {
			printf("sprite_load: Incorrect TGA header size! (%zu bytes) Should be 18 bytes.\n",
			       tga_header_size);
			free(fbuf);
			fbuf = NULL;
			return false;
		}

		u16 width = header->width;
		u16 height = header->height;
		u8 bytes_per_pixel = header->bpp / 8;
		i32 stride = width * bytes_per_pixel;
		size_t pixel_size = width * height * bytes_per_pixel;

		img->data = (u8*)arena_alloc(&mem_arena, pixel_size, DEFAULT_ALIGNMENT);

		printf("sprite_load: %s, %dx%d %d bytes per pixel\n", path, width, height,
		       bytes_per_pixel);

		u8* tga_pixels = fbuf + tga_header_size;

		// origin bit 1 = upper-left origin pixel
		if ((header->desc >> 5) & 1) {
			memcpy(img->data, tga_pixels, pixel_size);
		}
		// origin bit 0 = lower-left origin pixel
		else {
			for (i32 c = 0; c < height; c++) {
				memcpy(img->data + stride * c,
				       tga_pixels + stride * (height - (c + 1)), stride);
			}
		}

		u32 pix_fmt = SDL_PIXELFORMAT_BGR24;
		if (bytes_per_pixel == 3) {
			pix_fmt = SDL_PIXELFORMAT_BGR24;
			img->has_alpha = false;
		} else if (bytes_per_pixel == 4) {
			pix_fmt = SDL_PIXELFORMAT_BGRA32;
			img->has_alpha = true;
		}

		img->surface = SDL_CreateRGBSurfaceWithFormatFrom(img->data, width, height,
								  header->bpp, stride, pix_fmt);

		free(fbuf);
		fclose(fptr);

		*out = img;
	} else {
		return false;
	}

	return true;
}

void sprite_create(u8* data, u32 w, u32 h, u32 bpp, u32 stride, u32 format, sprite_t** out)
{
	sprite_t* img = (sprite_t*)arena_alloc(&mem_arena, sizeof(sprite_t), DEFAULT_ALIGNMENT);
	img->type = IMG_TYPE_RAW;
	size_t pixel_size = w * h * (bpp / 8);
	img->data = (u8*)arena_alloc(&mem_arena, pixel_size, DEFAULT_ALIGNMENT);
	memcpy(img->data, data, pixel_size);
	img->surface = SDL_CreateRGBSurfaceWithFormatFrom(img->data, w, h, bpp, stride, format);

	*out = img;
}

bool sprite_create_texture(SDL_Renderer* ren, sprite_t* img)
{
	bool res = false;

	if (img != NULL) {
		img->texture = SDL_CreateTexture((SDL_Renderer*)ren, img->surface->format->format,
						 SDL_TEXTUREACCESS_STATIC, img->surface->w,
						 img->surface->h);

		if (img->texture != NULL) {
			SDL_UpdateTexture(img->texture, NULL, img->surface->pixels,
					  img->surface->pitch);

			SDL_SetTextureBlendMode(img->texture, img->has_alpha ? SDL_BLENDMODE_BLEND
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

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
