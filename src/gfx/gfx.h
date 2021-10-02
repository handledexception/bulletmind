#ifndef H_GFX
#define H_GFX

#include "core/types.h"
#include "core/export.h"

typedef enum {
    BM_GFX_D3D11 = 1 << 0,
    BM_GFX_OPENGL = 1 << 1,
    BM_GFX_INVALID = 1 << 2
} gfx_system_flags;

struct gfx_buffer;
struct gfx_texture;
struct gfx_zstencil;
struct gfx_vertex_buffer;
struct gfx_index_buffer;
struct gfx_sampler_state;
struct gfx_shader;
struct gfx_swapchain;
struct gfx_device;
struct gfx_system;

typedef struct gfx_system gfx_system_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_swapchain gfx_swapchain_t;

struct gfx_window {
#if defined(_WIN32)
    void *hwnd;
#endif
};

enum gfx_pixel_format {
    GFX_FORMAT_BGRA,
    GFX_FORMAT_FIRST = GFX_FORMAT_BGRA,
    GFX_FORMAT_RGBA,
    GFX_FORMAT_ARGB,
    GFX_FORMAT_RGB24,
    GFX_FORMAT_NV12,
    GFX_FORMAT_UNKNOWN,
    GFX_FORMAT_LAST = GFX_FORMAT_UNKNOWN
};

enum gfx_shader_type {
    GFX_SHADER_VERTEX,
    GFX_SHADER_FIRST = GFX_SHADER_VERTEX,
    GFX_SHADER_PIXEL,
    GFX_SHADER_COMPUTE,
    GFX_SHADER_GEOMETRY,
    GFX_SHADER_UNKNOWN,
    GFX_SHADER_LAST = GFX_SHADER_UNKNOWN
};

struct gfx_config {
    struct gfx_window window;
    s32 adapter;
    u32 width;
    u32 height;
    u32 fps_num;
    u32 fps_den;
    bool fullscreen;
    enum gfx_pixel_format pix_fmt;
};

enum gfx_culling_mode {
    GFX_CULLING_NONE,
    GFX_CULLING_FRONT_FACE,
    GFX_CULLING_BACK_FACE,
};

enum gfx_raster_flags {
    GFX_RASTER_WINDING_CCW = (1 << 0),
    GFX_RASTER_SCISSOR = (1 << 1),
    GFX_RASTER_MULTI_SAMPLE = (1 << 2),
    GFX_RASTER_WIREFRAME = (1 << 3),
};

enum gfx_texture_address_mode {
    GFX_TEXTURE_ADDRESS_REPEAT,
    GFX_TEXTURE_ADDRESS_FIRST = GFX_TEXTURE_ADDRESS_REPEAT,
    GFX_TEXTURE_ADDRESS_CLAMP,
    GFX_TEXTURE_ADDRESS_MIRROR,
    GFX_TEXTURE_ADDRESS_UNKNOWN,
    GFX_TEXTURE_ADDRESS_LAST = GFX_TEXTURE_ADDRESS_UNKNOWN
};

enum gfx_texture_flags {
    GFX_TEXTURE_SHARED = (1 << 0),
    GFX_TEXTURE_KEYED_MUTEX = (1 << 1),
    GFX_TEXTURE_DYNAMIC = (1 << 2),
    GFX_TEXTURE_IS_RENDER_TARGET = (1 << 3),
    GFX_TEXTURE_GENERATE_MIPMAPS = (1 << 4),
};

struct gfx_texture_desc {
    enum gfx_pixel_format pix_fmt;
    enum gfx_texture_address_mode address_mode;
    u32 width;
    u32 height;
    u32 mip_levels;
    u32 flags;
};

enum gfx_buffer_type {
    GFX_BUFFER_VERTEX,
    GFX_BUFFER_INDEX,
    GFX_BUFFER_CONSTANT,
    GFX_BUFFER_UNKNOWN
};

enum gfx_buffer_usage {
    GFX_BUFFER_USAGE_DEFAULT,
    GFX_BUFFER_USAGE_DYNAMIC,
    GFX_BUFFER_USAGE_IMMUTABLE,
    GFX_BUFFER_USAGE_STAGING,
    GFX_BUFFER_USAGE_UNKNOWN
};

u32 gfx_get_bits_per_pixel(enum gfx_pixel_format pf);

// BM_GFX_EXPORT gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg);

#endif
