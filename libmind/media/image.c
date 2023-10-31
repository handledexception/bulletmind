#include "media/image.h"
#include "media/targa.h"
#include "core/path.h"

#define ALIGN_SIZE(size, align) size = (((size) + (align - 1)) & (~(align - 1)))

void video_frame_new(u32 width, u32 height, enum pixel_format pix_fmt,
		     struct video_frame* frame)
{
	size_t size = 0;
	int alignment = mem_base_alignment();
	size_t offsets[MAX_AV_PLANES];

	if (!frame)
		return;

	memset(frame, 0, sizeof(struct video_frame));
	memset(offsets, 0, sizeof(offsets));

	switch (pix_fmt) {
	case PIX_FMT_RGB24:
		size = width * height * 3;
		ALIGN_SIZE(size, alignment);
		frame->data[0] = BM_ALLOC(size);
		frame->stride[0] = width * 3;
		break;
	case PIX_FMT_ARGB32:
	case PIX_FMT_RGBA32:
	case PIX_FMT_BGRA32:
		size = width * height * 4;
		ALIGN_SIZE(size, alignment);
		frame->data[0] = BM_ALLOC(size);
		frame->stride[0] = width * 4;
		break;
	}
}

struct media_image* media_image_new()
{
	struct media_image* img;
	img = (struct media_image*)BM_ALLOC(sizeof(struct media_image));
	memset(img, 0, sizeof(struct media_image));
	return img;
}

void media_image_free(struct media_image* img)
{
	if (img != NULL) {
		video_frame_free(&img->frame);
		BM_FREE(img);
		memset(img, 0, sizeof(struct media_image));
	}
}

result media_image_load(const char* path, struct media_image* img)
{
	FILE* file_ptr = NULL;
	size_t fsize = 0;
	u8* file_buf = NULL;

	if (path == NULL || img == NULL)
		return RESULT_NULL;

	// absolute most janky file extension comparison
	const char* file_ext = path_get_extension(path);
	if (strcmp(file_ext, "tga") == 0) {
		img->pix_fmt = PIX_FMT_BGRA32;
		img->img_fmt = IMG_TYPE_TARGA;
		file_ptr = fopen(path, "rb");
		fseek(file_ptr, 0, SEEK_END);
		fsize = ftell(file_ptr);
		fseek(file_ptr, 0, SEEK_SET);
		file_buf = (u8*)BM_ALLOC(fsize);
		if (file_ptr == NULL) {
			logger(LOG_ERROR,
			       "media_image_load: file %s has no data!\n",
			       path);
			return RESULT_NO_DATA;
		} else if (fread(file_buf, sizeof(u8), fsize, file_ptr) !=
			   fsize) {
			logger(LOG_ERROR,
			       "media_image_load: could not read to end of file %s\n",
			       path);
			BM_FREE(file_buf);
			file_buf = NULL;
			return RESULT_RANGE;
		}

		tga_header_t* header = (tga_header_t*)file_buf;
		size_t tga_header_size = sizeof(*header);
		// make sure we have a valid minimal TGA header and raw unmapped RGB data
		if (tga_header_size != 18 || header->color_map_type > 0 ||
		    header->image_type != 2) {
			logger(LOG_ERROR,
			       "media_image_load: bad TGA header size (%zu bytes). 18 bytes expected.\n",
			       tga_header_size);
			BM_FREE(file_buf);
			file_buf = NULL;
			return RESULT_NULL;
		}

		u16 width = header->width;
		u16 height = header->height;
		u8 bytes_per_pixel = header->bpp / 8;
		s32 stride = width * bytes_per_pixel;
		size_t pixel_size = width * height * bytes_per_pixel;

		video_frame_new(width, height, img->pix_fmt, &img->frame);

		logger(LOG_INFO,
		       "media_image_load: %s, %dx%d, %d bytes per pixel\n",
		       path, width, height, bytes_per_pixel);

		u8* tga_pixels = file_buf + tga_header_size;

		if ((header->desc >> 5) & 1) {
			// origin bit 1 = upper-left origin pixel
			memcpy(img->frame.data[0], tga_pixels, pixel_size);
		} else {
			// origin bit 0 = lower-left origin pixel
			for (s32 c = 0; c < height; c++) {
				memcpy(img->frame.data[0] + stride * c,
				       tga_pixels + stride * (height - (c + 1)),
				       stride);
			}
		}

		img->width = width;
		img->height = height;
		img->frame.stride[0] = stride;

		BM_FREE(file_buf);
		fclose(file_ptr);
	} else {
		logger(LOG_ERROR, "media_image_load: unsupported extension %s",
		       file_ext);
		return RESULT_UNKNOWN;
	}

	return RESULT_OK;
}
