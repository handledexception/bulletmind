#include "gfx/gfx.h"
#include "media/image.h"

#include "math/types.h"

struct gfx_sprite {
    struct media_image* img;
    vec2f_t scale;
};

struct gfx_ss_frame {
    rect_t bbox;
    f32 duration;
};

struct gfx_sprite_sheet {
    s32 sheet_width;
    s32 sheet_height;
    s32 cel_width;
    s32 cel_height;
    s32 num_frames;
    gfx_ss_frame_t* frames;
    struct media_image* image;
};
