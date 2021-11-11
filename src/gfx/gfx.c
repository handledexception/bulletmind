#include "gfx/gfx.h"
#include "math/types.h"

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

u32 gfx_get_vertex_stride(enum gfx_vertex_type type)
{
    u32 stride = 0;
    switch (type) {
        case GFX_VERTEX_POS_UV:
            stride = sizeof(struct vec3f) + sizeof(struct vec2f);
            break;
        case GFX_VERTEX_POS_NORM_UV:
            stride = (sizeof(struct vec3f) * 2) + sizeof(struct vec2f);
            break;
        case GFX_VERTEX_POS_COLOR:
            stride = sizeof(struct vec3f) + sizeof(struct vec4f);
            break;
        case GFX_VERTEX_POS_NORM_COLOR:
            stride = (sizeof(struct vec3f) * 2) + sizeof(struct vec4f);
            break;
        default:
        case GFX_VERTEX_UNKNOWN:
            stride = 0;
            break;
    }
    return stride;
}

// gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg)
// {
//     struct gfx_config new_cfg = *cfg;
//     gfx_system_t* gfx_sys = thread_gfx;
// }
