#ifndef H_GFX
#define H_GFX

#include "core/types.h"
#include "core/export.h"

#define Z_NEAR 0.1f
#define Z_FAR 1000.f
#define FOV_Y 90.f

typedef struct vec2f vec2f_t;
typedef struct vec3f vec3f_t;
typedef struct vec4f vec4f_t;
typedef struct mat4f mat4f_t;

typedef enum {
    BM_GFX_D3D11 = 1 << 0,
    BM_GFX_OPENGL = 1 << 1,
    BM_GFX_INVALID = 1 << 2
} gfx_system_flags;

struct gfx_system;
struct gfx_device;
struct gfx_swapchain;
struct gfx_shader;
struct gfx_buffer;
struct gfx_texture;
struct gfx_zstencil;
struct gfx_sampler_state;
struct gfx_zstencil_state;

typedef struct gfx_system gfx_system_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_texture gfx_texture_t;

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
    GFX_FORMAT_DEPTH_U16,
    GFX_FORMAT_DEPTH_U24,
    GFX_FORMAT_DEPTH_F32,
    GFX_FORMAT_UNKNOWN,
    GFX_FORMAT_LAST = GFX_FORMAT_UNKNOWN
};

enum gfx_texture_type {
    GFX_TEXTURE_1D,
    GFX_TEXTURE_2D,
    GFX_TEXTURE_3D,
    GFX_TEXTURE_CUBE,
};

enum gfx_shader_type {
    GFX_SHADER_VERTEX,
    GFX_SHADER_PIXEL,
    GFX_SHADER_COMPUTE,
    GFX_SHADER_GEOMETRY,
    GFX_SHADER_UNKNOWN
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

enum gfx_topology {
    GFX_TOPOLOGY_POINT_LIST,
    GFX_TOPOLOGY_LINE_LIST,
    GFX_TOPOLOGY_LINE_STRIP,
    GFX_TOPOLOGY_TRIANGLE_LIST,
    GFX_TOPOLOGY_TRIANGLE_STRIP
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
    GFX_RASTER_ANTIALIAS_LINES = (1 << 4),
};

struct gfx_raster_opts {
    enum gfx_culling_mode culling_mode;
    enum gfx_raster_flags raster_flags;
};

enum gfx_sample_filter {
    GFX_FILTER_POINT,
    GFX_FILTER_LINEAR,
    GFX_FILTER_ANISOTROPIC,
	GFX_FILTER_MIN_MAG_POINT_MIP_LINEAR,
	GFX_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
	GFX_FILTER_MIN_POINT_MAG_MIP_LINEAR,
	GFX_FILTER_MIN_LINEAR_MAG_MIP_POINT,
	GFX_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
	GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT,
    GFX_FILTER_UNKNOWN,
};

enum gfx_sample_address_mode {
    GFX_SAMPLER_ADDRESS_REPEAT,
    GFX_SAMPLER_ADDRESS_CLAMP,
    GFX_SAMPLER_ADDRESS_MIRROR,
    GFX_SAMPLER_ADDRESS_UNKNOWN,
};

struct gfx_sampler_config {
    enum gfx_sample_filter filter;
    enum gfx_sample_address_mode address_u;
    enum gfx_sample_address_mode address_v;
    enum gfx_sample_address_mode address_w;
    s32 max_anisotropy;
    u32 border_color;
};

enum gfx_texture_flags {
    GFX_TEXTURE_SHARED = (1 << 0),
    GFX_TEXTURE_KEYED_MUTEX = (1 << 1),
    GFX_TEXTURE_DYNAMIC = (1 << 2),
    GFX_TEXTURE_IS_RENDER_TARGET = (1 << 3),
    GFX_TEXTURE_IS_ZSTENCIL = (1 << 4),
    GFX_TEXTURE_GENERATE_MIPMAPS = (1 << 5),
};

struct gfx_texture_desc {
    enum gfx_texture_type type;
    enum gfx_pixel_format pix_fmt;
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

// 1D, 2D, 3D texel data
struct texture_vertex {
    size_t size;
    void* data;
};

struct gfx_vertex_data {
    size_t num_vertices;
    struct vec3f* positions;
    struct vec3f* normals;
    struct vec3f* tangents;
    struct vec4f* colors;
    struct texture_vertex* tex_verts;
};

enum gfx_vertex_type {
    GFX_VERTEX_POS_UV = 0,
    GFX_VERTEX_FIRST = GFX_VERTEX_POS_UV,
    GFX_VERTEX_POS_NORM_UV = 1,
    GFX_VERTEX_POS_COLOR = 2,
    GFX_VERTEX_POS_NORM_COLOR = 3,
    GFX_VERTEX_UNKNOWN = 4,
    GFX_VERTEX_LAST = GFX_VERTEX_UNKNOWN
};

enum gfx_shader_var_type {
    GFX_SHADER_PARAM_BOOL,
    GFX_SHADER_PARAM_S32,
    GFX_SHADER_PARAM_U32,
    GFX_SHADER_PARAM_F32,
    GFX_SHADER_PARAM_F64,
    GFX_SHADER_PARAM_VEC2,
    GFX_SHADER_PARAM_VEC3,
    GFX_SHADER_PARAM_VEC4,
    GFX_SHADER_PARAM_MAT4,
    GFX_SHADER_PARAM_TEX
};

typedef struct {
    const char* name;
    enum gfx_shader_var_type type;
    size_t size;                    // size of the variable (16-byte aligned)
    size_t offset;                  // offset inside of constant buffer
    void* data;
    gfx_buffer_t* cbuffer;
} gfx_shader_var_t;

u32 gfx_get_bits_per_pixel(enum gfx_pixel_format pf);

u32 gfx_texture_get_width(gfx_texture_t* texture);
u32 gfx_texture_get_height(gfx_texture_t* texture);
void gfx_texture_get_size(gfx_texture_t* texture, vec2f_t* size);
// BM_GFX_EXPORT gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg);

#endif
