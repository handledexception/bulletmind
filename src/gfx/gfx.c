#include "gfx/gfx.h"

u32 gfx_get_bits_per_pixel(enum gfx_pixel_format pf)
{
    u32 bpp = 0;
    switch (pf) {
    case GFX_FORMAT_BGRA:
        bpp = 32;
        break;
    case GFX_FORMAT_RGBA:
        bpp = 32;
        break;
    case GFX_FORMAT_ARGB:
        bpp = 32;
        break;
    case GFX_FORMAT_RGB24:
        bpp = 24;
        break;
    case GFX_FORMAT_NV12:
        bpp = 16;
        break;
    case GFX_FORMAT_DEPTH_U16:
        bpp = 16;
        break;
    case GFX_FORMAT_DEPTH_U24:
        bpp = 24;
        break;
    case GFX_FORMAT_DEPTH_F32:
        bpp = 32;
        break;
    case GFX_FORMAT_UNKNOWN:
    default:
        break;
    }
    return bpp;
}

// gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg)
// {
//     struct gfx_config new_cfg = *cfg;
//     gfx_system_t* gfx_sys = thread_gfx;
// }
