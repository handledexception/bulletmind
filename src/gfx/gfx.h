#ifndef H_GFX
#define H_GFX

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Z_NEAR 0.0f
#define Z_FAR 100.f
#define FOV_Y 90.f
#define BM_GFX_MAX_VERTICES 1024

typedef struct vec2f vec2f_t;
typedef struct vec3f vec3f_t;
typedef struct vec4f vec4f_t;
typedef struct mat4f mat4f_t;
typedef struct rgba rgba_t;

typedef enum {
	GFX_D3D11 = 1 << 0,
	GFX_OPENGL = 1 << 1,
	GFX_USE_ZBUFFER = 1 << 2,
	GFX_INVALID = 1 << 3
} gfx_system_flags;

struct gfx_system;
struct gfx_device;
struct gfx_swapchain;
struct gfx_shader {
	enum gfx_shader_type type;
	void* impl;
	gfx_buffer_t* cbuffer;
	VECTOR(gfx_shader_var_t) vars;
};
struct gfx_vertex_shader;
struct gfx_buffer;
struct gfx_texture;
struct gfx_zstencil;
struct gfx_sampler_state;
struct gfx_zstencil_state;

typedef struct gfx_system gfx_system_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct gfx_pixel_shader gfx_pixel_shader_t;
typedef struct gfx_vertex_shader gfx_vertex_shader_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_texture gfx_texture_t;

struct gfx_window {
#if defined(_WIN32)
	void* hwnd;
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

enum gfx_vertex_type {
	GFX_VERTEX_POS_UV = 0,
	GFX_VERTEX_FIRST = GFX_VERTEX_POS_UV,
	GFX_VERTEX_POS_NORM_UV = 1,
	GFX_VERTEX_POS_COLOR = 2,
	GFX_VERTEX_POS_NORM_COLOR = 3,
	GFX_VERTEX_UNKNOWN = 4,
	GFX_VERTEX_LAST = GFX_VERTEX_UNKNOWN
};

struct gfx_vertex_data {
	enum gfx_vertex_type type;
	struct vec3f* positions;
	struct vec3f* normals;
	struct vec3f* tangents;
	struct vec4f* colors;
	struct texture_vertex* tex_verts;
	size_t num_vertices;
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

typedef struct gfx_shader_var {
	const char* name;
	enum gfx_shader_var_type type;
	size_t offset; // offset inside of constant buffer
	void* data;
} gfx_shader_var_t;

/* system ------------------------------------------------------------------ */
BM_EXPORT result gfx_enumerate_adapters(gfx_system_t* gfx, u32 adapter_index,
					u32* count);
BM_EXPORT result gfx_enumerate_adapter_monitors(gfx_system_t* gfx);
BM_EXPORT void gfx_set_viewport(gfx_system_t* gfx, u32 width, u32 height);
BM_EXPORT gfx_system_t* gfx_system_init(const struct gfx_config* cfg,
					s32 flags);
BM_EXPORT void gfx_system_shutdown(gfx_system_t* gfx);
BM_EXPORT void gfx_system_bind_render_target(gfx_system_t* gfx);
BM_EXPORT result gfx_create_swap_chain(gfx_system_t* gfx,
				       const struct gfx_config* cfg);
BM_EXPORT result gfx_create_device_dependent_resources(gfx_system_t* gfx,
						       s32 adapter);
BM_EXPORT void gfx_destroy_device_dependent_resources(gfx_system_t* gfx);

BM_EXPORT void gfx_render_clear(gfx_system_t* gfx, const rgba_t* color);
BM_EXPORT void gfx_render_begin(gfx_system_t* gfx);
BM_EXPORT void gfx_render_end(gfx_system_t* gfx, bool vsync, u32 flags);
BM_EXPORT void gfx_set_vertex_shader(gfx_system_t* gfx, gfx_shader_t* vs);
BM_EXPORT void gfx_set_pixel_shader(gfx_system_t* gfx, gfx_shader_t* ps);
BM_EXPORT enum gfx_vertex_type gfx_get_vertex_type(gfx_system_t* gfx);
BM_EXPORT void gfx_init_sprite(gfx_system_t* gfx, gfx_buffer_t* vertex_buffer);
BM_EXPORT void gfx_init_cube(gfx_system_t* gfx, gfx_buffer_t* vertex_buffer);
BM_EXPORT void gfx_draw_sprite(gfx_system_t* gfx, gfx_texture_t* texture,
			       u32 width, u32 height, u32 flags);

/* buffer ------------------------------------------------------------------ */
BM_EXPORT result gfx_buffer_create(gfx_system_t* gfx, const void* data,
				   size_t size, enum gfx_buffer_type type,
				   enum gfx_buffer_usage usage,
				   gfx_buffer_t** buffer);
BM_EXPORT void gfx_buffer_free(gfx_buffer_t* buffer);
BM_EXPORT result gfx_buffer_copy(gfx_system_t* gfx, gfx_buffer_t* buffer,
				 const void* data, size_t size);
BM_EXPORT void gfx_bind_vertex_buffer(gfx_system_t* gfx, gfx_buffer_t* vb,
				      u32 stride, u32 offset);
BM_EXPORT void gfx_buffer_upload_constants(gfx_system_t* gfx,
					   const gfx_buffer_t* buffer,
					   enum gfx_shader_type type);
BM_EXPORT enum gfx_vertex_type gfx_vertex_type_from_string(const char* s);

/* shader ------------------------------------------------------------------ */
BM_EXPORT void gfx_shader_init(gfx_shader_t* shader);
BM_EXPORT gfx_shader_t* gfx_shader_create(enum gfx_shader_type type);
BM_EXPORT void gfx_shader_free(gfx_shader_t* shader);
BM_EXPORT void gfx_vertex_shader_init(gfx_vertex_shader_t* vs);
BM_EXPORT gfx_vertex_shader_t* gfx_vertex_shader_create();
BM_EXPORT void gfx_vertex_shader_free(gfx_vertex_shader_t* vs);
BM_EXPORT void gfx_pixel_shader_init(gfx_pixel_shader_t* ps);
BM_EXPORT gfx_pixel_shader_t* gfx_pixel_shader_create();
BM_EXPORT void gfx_pixel_shader_free(gfx_pixel_shader_t* ps);
BM_EXPORT result gfx_shader_compile_from_file(gfx_system_t* gfx,
						    const char* path,
						    const char* entrypoint,
						    const char* target,
						    gfx_shader_t* shader);
BM_EXPORT result gfx_shader_build_program(gfx_system_t* gfx, gfx_shader_t* shader);

BM_EXPORT gfx_vertex_shader_t*
gfx_vertex_shader_create_from_file(gfx_system_t* gfx, const char* path,
				   const char* entrypoint, const char* target,
				   enum gfx_vertex_type type);

BM_EXPORT void gfx_vertex_shader_free(gfx_vertex_shader_t* vs);

BM_EXPORT result gfx_vertex_shader_create_input_layout(gfx_system_t* gfx,
						       gfx_vertex_shader_t* vs);
BM_EXPORT enum gfx_vertex_type gfx_shader_get_vertex_type(gfx_shader_t* vs);
BM_EXPORT void gfx_bind_primitive_topology(gfx_system_t* gfx,
					   enum gfx_topology topo);
BM_EXPORT void
gfx_vertex_shader_bind_input_layout(gfx_system_t* gfx,
				    const gfx_vertex_shader_t* vs);
BM_EXPORT bool gfx_shader_add_var(gfx_shader_t* shader, gfx_shader_var_t* var);
BM_EXPORT bool gfx_shader_set_var_by_name(gfx_shader_t* shader,
					  const char* name, const void* value,
					  size_t size);
BM_EXPORT gfx_shader_var_t* gfx_shader_get_var_by_name(gfx_shader_t* shader,
						       const char* name);
BM_EXPORT size_t gfx_get_shader_var_size(enum gfx_shader_var_type type);
BM_EXPORT const char* gfx_shader_type_to_string(enum gfx_shader_type type);
/* sampler ----------------------------------------------------------------- */
BM_EXPORT result gfx_init_sampler_state(gfx_system_t* gfx);
BM_EXPORT void gfx_bind_sampler_state(gfx_system_t* gfx, gfx_texture_t* texture,
				      u32 slot);

/* rasterizer -------------------------------------------------------------- */
BM_EXPORT result gfx_init_rasterizer(gfx_system_t* gfx,
				     enum gfx_culling_mode culling,
				     enum gfx_raster_flags flags);
BM_EXPORT void gfx_bind_rasterizer(gfx_system_t* gfx);

/* texture ----------------------------------------------------------------- */
BM_EXPORT result gfx_create_texture2d(gfx_system_t* gfx, const u8* data,
				      const struct gfx_texture_desc* desc,
				      gfx_texture_t** texture2d);
BM_EXPORT result gfx_create_texture(gfx_system_t* gfx, const u8* data,
				    const struct gfx_texture_desc* desc,
				    gfx_texture_t** texture);
BM_EXPORT void gfx_destroy_texture(gfx_texture_t* texture);
BM_EXPORT result gfx_init_render_target(gfx_system_t* gfx, u32 width,
					u32 height, enum gfx_pixel_format pf);
BM_EXPORT void gfx_set_render_target(gfx_system_t* gfx, gfx_texture_t* texture,
				     gfx_texture_t* zstencil);
BM_EXPORT result gfx_create_zstencil_state(gfx_system_t* gfx, bool enable,
					   struct gfx_zstencil_state** state);
BM_EXPORT result gfx_init_zstencil(gfx_system_t* gfx, u32 width, u32 height,
				   enum gfx_pixel_format pix_fmt, bool enabled);
BM_EXPORT void gfx_destroy_zstencil(gfx_system_t* gfx);
BM_EXPORT void gfx_bind_zstencil_state(gfx_system_t* gfx,
				       const struct gfx_zstencil_state* state);
BM_EXPORT void gfx_toggle_zstencil(gfx_system_t* gfx, bool enabled);

/* misc -------------------------------------------------------------------- */
BM_EXPORT u32 gfx_get_bits_per_pixel(enum gfx_pixel_format pf);
BM_EXPORT u32 gfx_texture_get_width(gfx_texture_t* texture);
BM_EXPORT u32 gfx_texture_get_height(gfx_texture_t* texture);
BM_EXPORT void gfx_texture_get_size(gfx_texture_t* texture, vec2f_t* size);
BM_EXPORT u32 gfx_get_vertex_stride(enum gfx_vertex_type type);

// BM_EXPORT gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg);

#ifdef __cplusplus
}
#endif

#endif // H_GFX
