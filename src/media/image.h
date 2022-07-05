#ifndef H_MEDIA
#define H_MEDIA

#include "core/export.h"
#include "core/memory.h"
#include "core/types.h"
#include "media/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_AV_PLANES 8

struct video_frame {
	u8* data[MAX_AV_PLANES];
	u32 stride[MAX_AV_PLANES];
};

struct media_image {
	struct video_frame frame;
	u32 width;
	u32 height;
	enum image_format img_fmt;
	enum pixel_format pix_fmt;
};

typedef struct media_image media_image_t;

BM_EXPORT void video_frame_new(u32 width, u32 height, enum pixel_format pix_fmt,
			       struct video_frame* frame);
static inline void video_frame_free(struct video_frame* frame)
{
	if (frame) {
		BM_MEM_FREE(frame->data[0]);
		memset(frame, 0, sizeof(struct video_frame));
	}
}

BM_EXPORT struct media_image* media_image_new();
BM_EXPORT void media_image_free(struct media_image* img);
BM_EXPORT result media_image_load(const char* path, struct media_image* img);


static inline struct video_frame*
video_frame_create(enum pixel_format format, uint32_t width, uint32_t height)
{
	struct video_frame* frame;

	frame = (struct video_frame*)MEM_ALLOC(sizeof(struct video_frame));
	video_frame_init(frame, format, width, height);
	return frame;
}

static inline void video_frame_destroy(struct video_frame* frame)
{
	if (frame) {
		BM_MEM_FREE(frame->data[0]);
		BM_MEM_FREE(frame);
	}
}

BM_EXPORT void video_frame_copy(struct video_frame* dst,
				const struct video_frame* src,
				enum video_format format, uint32_t height);

#ifdef __cplusplus
}
#endif

#endif // H_MEDIA
