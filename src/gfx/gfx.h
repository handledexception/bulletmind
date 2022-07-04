#ifndef H_GFX
#define H_GFX

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"

#include "gfx/enums.h"

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

struct gfx_adapter;
struct gfx_buffer;
struct gfx_device;
struct gfx_display;
struct gfx_module;
struct gfx_monitor;
struct gfx_pixel_shader;
struct gfx_sampler_state;
struct gfx_shader;
struct gfx_shader_var;
struct gfx_swapchain;
struct gfx_system;
struct gfx_texture;
struct gfx_vertex_shader;
struct gfx_zstencil;
struct gfx_zstencil_state;

typedef struct gfx_adapter gfx_adapter_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_display gfx_display_t;
typedef struct gfx_module gfx_module_t;
typedef struct gfx_monitor gfx_monitor_t;
typedef struct gfx_pixel_shader gfx_pixel_shader_t;
typedef struct gfx_sampler_state gfx_sampler_state_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct gfx_shader_var gfx_shader_var_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_system gfx_system_t;
typedef struct gfx_texture gfx_texture_t;
typedef struct gfx_vertex_shader gfx_vertex_shader_t;
typedef struct gfx_zstencil gfx_zstencil_t;
typedef struct gfx_zstencil_state gfx_zstencil_state_t;

struct gfx_window {
#if defined(_WIN32)
	void* hwnd;
#endif
};

struct gfx_config {
	enum gfx_module_type module;
	struct gfx_window window;
	u32 adapter;
	u32 width;
	u32 height;
	u32 fps_num;
	u32 fps_den;
	bool fullscreen;
	enum gfx_pixel_format pix_fmt;
};

struct gfx_system {
	gfx_module_t* module;      /* platform graphics module */
	enum gfx_module_type type; /* module type (D3D11, OpenGL, etc.) */
	VECTOR(struct gfx_adapter) adapters; /* graphics adapters */
};

struct gfx_shader_var {
	char name[256];                /* friendly name */
	enum gfx_shader_var_type type; /* shader variable type */
	void* data;                    /* data blob */
};

struct gfx_shader {
	enum gfx_shader_type type;
	void* impl;
	gfx_buffer_t* cbuffer;
	VECTOR(gfx_shader_var_t) vars;
};

struct gfx_raster_opts {
	enum gfx_culling_mode culling_mode;
	enum gfx_raster_flags raster_flags;
};

struct gfx_sampler_config {
	enum gfx_sample_filter filter;
	enum gfx_sample_address_mode address_u;
	enum gfx_sample_address_mode address_v;
	enum gfx_sample_address_mode address_w;
	s32 max_anisotropy;
	u32 border_color;
};

struct gfx_texture_desc {
	enum gfx_texture_type type;
	enum gfx_pixel_format pix_fmt;
	u32 width;
	u32 height;
	u32 mip_levels;
	u32 flags;
};

struct gfx_texture {
	enum gfx_texture_type type;
	u8* data;
	size_t size;
	void* impl; // gfx_texture2d, etc.
};

// 1D, 2D, 3D texel data
struct texture_vertex {
	size_t size;
	void* data;
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

extern gfx_system_t* gfx;
extern bool gfx_sys_ok;
extern bool gfx_module_ok;

BM_EXPORT result gfx_init(const struct gfx_config* cfg, s32 flags);
BM_EXPORT void gfx_shutdown(void);

#if defined(_WIN32)
BM_EXPORT result gfx_init_dx11(const struct gfx_config* cfg, s32 flags);
BM_EXPORT void gfx_shutdown_dx11(void);
BM_EXPORT bool gfx_ok(void);
#endif

/* system ------------------------------------------------------------------ */
BM_EXPORT result gfx_enumerate_adapters(struct vector* adapters,
					bool enum_displays);
BM_EXPORT result gfx_enumerate_displays(const gfx_adapter_t* adapter,
					struct vector* displays);
BM_EXPORT void gfx_set_viewport(u32 width, u32 height);
BM_EXPORT gfx_shader_t* gfx_system_get_vertex_shader();
BM_EXPORT gfx_shader_t* gfx_system_get_pixel_shader();
BM_EXPORT void gfx_system_bind_render_target(void);
BM_EXPORT void gfx_system_bind_input_layout(gfx_shader_t* shader);
BM_EXPORT result gfx_create_swap_chain(const struct gfx_config* cfg);
BM_EXPORT result gfx_create_device(s32 adapter);
BM_EXPORT void gfx_destroy_device(void);


BM_EXPORT void gfx_render_clear(const rgba_t* color);
BM_EXPORT void gfx_render_begin(void);
BM_EXPORT void gfx_render_end(bool vsync, u32 flags);
BM_EXPORT void gfx_set_vertex_shader(gfx_shader_t* vs);
BM_EXPORT void gfx_set_pixel_shader(gfx_shader_t* ps);
BM_EXPORT enum gfx_vertex_type gfx_get_vertex_type(void);
BM_EXPORT void gfx_init_sprite(gfx_buffer_t* vertex_buffer);
BM_EXPORT void gfx_destroy_sprite(gfx_buffer_t* vertex_buffer);
BM_EXPORT void gfx_init_cube(gfx_buffer_t* vertex_buffer);
BM_EXPORT void gfx_draw_sprite(gfx_texture_t* texture, u32 width, u32 height,
			       u32 flags);

/* buffer ------------------------------------------------------------------ */
BM_EXPORT result gfx_buffer_create(const void* data, size_t size,
				   enum gfx_buffer_type type,
				   enum gfx_buffer_usage usage,
				   gfx_buffer_t** buffer);
BM_EXPORT void gfx_buffer_free(gfx_buffer_t* buffer);
BM_EXPORT size_t gfx_buffer_get_size(gfx_buffer_t* buffer);
BM_EXPORT result gfx_buffer_copy(gfx_buffer_t* buffer, const void* data,
				 size_t size);
BM_EXPORT void gfx_bind_vertex_buffer(gfx_buffer_t* vb, u32 stride, u32 offset);
BM_EXPORT void gfx_buffer_upload_constants(const gfx_shader_t* shader);
BM_EXPORT enum gfx_vertex_type gfx_vertex_type_from_string(const char* s);

/* shader ------------------------------------------------------------------ */
BM_EXPORT void gfx_shader_init(gfx_shader_t* shader);
BM_EXPORT gfx_shader_t* gfx_shader_new(enum gfx_shader_type type);
BM_EXPORT void gfx_shader_free(gfx_shader_t* shader);
BM_EXPORT size_t gfx_shader_cbuffer_resize(gfx_shader_t* shader);
BM_EXPORT size_t gfx_shader_cbuffer_fill(gfx_shader_t* shader);
BM_EXPORT void gfx_vertex_shader_init(gfx_vertex_shader_t* vs);
BM_EXPORT gfx_vertex_shader_t* gfx_vertex_shader_create(void);
BM_EXPORT void gfx_vertex_shader_free(gfx_vertex_shader_t* vs);
BM_EXPORT void gfx_pixel_shader_init(gfx_pixel_shader_t* ps);
BM_EXPORT gfx_pixel_shader_t* gfx_pixel_shader_create();
BM_EXPORT void gfx_pixel_shader_free(gfx_pixel_shader_t* ps);
BM_EXPORT result gfx_shader_compile_from_file(const char* path,
					      const char* entrypoint,
					      const char* target,
					      gfx_shader_t* shader);
BM_EXPORT result gfx_shader_build_program(gfx_shader_t* shader);
BM_EXPORT result gfx_shader_new_input_layout(gfx_shader_t* vs);

BM_EXPORT gfx_vertex_shader_t*
gfx_vertex_shader_create_from_file(const char* path, const char* entrypoint,
				   const char* target,
				   enum gfx_vertex_type type);

BM_EXPORT void gfx_vertex_shader_free(gfx_vertex_shader_t* vs);

BM_EXPORT enum gfx_vertex_type
gfx_vertex_shader_get_vertex_type(gfx_vertex_shader_t* vs);
BM_EXPORT void gfx_vertex_shader_set_vertex_type(gfx_vertex_shader_t* vs,
						 enum gfx_vertex_type type);
BM_EXPORT void gfx_bind_primitive_topology(enum gfx_topology topo);
BM_EXPORT void
gfx_vertex_shader_bind_input_layout(const gfx_vertex_shader_t* vs);

BM_EXPORT gfx_shader_var_t* gfx_shader_var_new(const char* name,
					       enum gfx_shader_var_type type);
BM_EXPORT void gfx_shader_var_init(gfx_shader_var_t* var);
BM_EXPORT void gfx_shader_var_free(gfx_shader_var_t* var);
BM_EXPORT void gfx_shader_var_set(gfx_shader_var_t* var, const void* data);
BM_EXPORT bool gfx_shader_add_var(gfx_shader_t* shader,
				  const gfx_shader_var_t* var);
BM_EXPORT bool gfx_shader_set_var_by_name(gfx_shader_t* shader,
					  const char* name, const void* value,
					  size_t size);
BM_EXPORT gfx_shader_var_t* gfx_shader_get_var_by_name(gfx_shader_t* shader,
						       const char* name);
BM_EXPORT size_t gfx_shader_var_size(enum gfx_shader_var_type type);
BM_EXPORT const char* gfx_shader_type_to_string(enum gfx_shader_type type);
/* sampler ----------------------------------------------------------------- */
BM_EXPORT result gfx_init_sampler_state(void);
BM_EXPORT void gfx_bind_sampler_state(gfx_texture_t* texture, u32 slot);

/* rasterizer -------------------------------------------------------------- */
BM_EXPORT result gfx_init_rasterizer(enum gfx_culling_mode culling,
				     enum gfx_raster_flags flags);
BM_EXPORT void gfx_bind_rasterizer(void);

/* texture ----------------------------------------------------------------- */
BM_EXPORT result gfx_texture_create(const u8* data,
				    const struct gfx_texture_desc* desc,
				    gfx_texture_t** texture);
BM_EXPORT void gfx_texture_init(gfx_texture_t* texture);
BM_EXPORT void gfx_texture_destroy(gfx_texture_t* texture);
BM_EXPORT result gfx_texture2d_create(const u8* data,
				      const struct gfx_texture_desc* desc,
				      gfx_texture_t** texture2d);
BM_EXPORT void gfx_texture2d_destroy(gfx_texture_t* texture);
BM_EXPORT result gfx_render_target_init(u32 width, u32 height,
					enum gfx_pixel_format pf);
BM_EXPORT void gfx_render_target_destroy(void);
BM_EXPORT void gfx_set_render_target(gfx_texture_t* texture,
				     gfx_texture_t* zstencil);
BM_EXPORT result gfx_create_zstencil_state(bool enable,
					   struct gfx_zstencil_state** state);
BM_EXPORT result gfx_init_zstencil(u32 width, u32 height,
				   enum gfx_pixel_format pix_fmt, bool enabled);
BM_EXPORT void gfx_destroy_zstencil(void);
BM_EXPORT void gfx_bind_zstencil_state(const struct gfx_zstencil_state* state);
BM_EXPORT void gfx_toggle_zstencil(bool enabled);

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
