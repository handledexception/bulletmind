#pragma once

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"

#include "gfx/enums.h"
#include "math/types.h"
#include "media/enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Z_NEAR 0.1
#define Z_FAR 1000.f
#define FOV_Y 90.f
#define BM_GFX_MAX_VERTICES 256
#define BM_GFX_MAX_INDICES 256

#define GFX_VERTEX_HAS_POS(__vtype__)                                         \
	(__vtype__ == GFX_VERTEX_POS || __vtype__ == GFX_VERTEX_POS_UV || \
	 __vtype__ == GFX_VERTEX_POS_NORM_UV ||                           \
	 __vtype__ == GFX_VERTEX_POS_COLOR ||                             \
	 __vtype__ == GFX_VERTEX_POS_NORM_COLOR)
#define GFX_VERTEX_HAS_NORMAL(__vtype__)            \
	(__vtype__ == GFX_VERTEX_POS_NORM_UV || \
	 __vtype__ == GFX_VERTEX_POS_NORM_COLOR)
#define GFX_VERTEX_HAS_UV(__vtype__) \
	(__vtype__ == GFX_VERTEX_POS_UV || __vtype__ == GFX_VERTEX_POS_NORM_UV)
#define GFX_VERTEX_HAS_COLOR(__vtype__)           \
	(__vtype__ == GFX_VERTEX_POS_COLOR || \
	 __vtype__ == GFX_VERTEX_POS_NORM_COLOR)
#define GFX_VERTEX_HAS_TANGENT(__vtype__) (0)

struct media_image; // foward decl

typedef struct vec2f vec2f_t;
typedef struct vec3f vec3f_t;
typedef struct vec4f vec4f_t;
typedef struct mat4f mat4f_t;
typedef struct rgba rgba_t;

typedef struct gfx_adapter gfx_adapter_t;
typedef struct gfx_buffer gfx_buffer_t;
typedef struct gfx_device gfx_device_t;
typedef struct gfx_display gfx_display_t;
typedef struct gfx_module gfx_module_t;
typedef struct gfx_monitor gfx_monitor_t;
typedef struct gfx_pipeline_desc gfx_pipeline_desc_t;
typedef struct gfx_pixel_shader gfx_pixel_shader_t;
typedef struct gfx_blend_state gfx_blend_state_t;
typedef struct gfx_raster_state gfx_raster_state_t;
typedef struct gfx_sampler_state gfx_sampler_state_t;
typedef struct gfx_shader gfx_shader_t;
typedef struct gfx_shader_var gfx_shader_var_t;
typedef struct gfx_swapchain gfx_swapchain_t;
typedef struct gfx_system gfx_system_t;
typedef struct gfx_texture gfx_texture_t;
typedef struct gfx_vertex_shader gfx_vertex_shader_t;
typedef struct gfx_depth gfx_depth_t;
typedef struct gfx_depth_state gfx_depth_state_t;
typedef struct gfx_sprite gfx_sprite_t;
typedef struct gfx_sheet gfx_sheet_t;             /* sprite sheet */
typedef struct gfx_sheet_frame gfx_sheet_frame_t; /* sprite sheet frame */
typedef struct gfx_mesh gfx_mesh_t;

typedef struct gui_window gui_window_t;
struct gfx_config {
	enum gfx_module_type module;
	gui_window_t* window;
	u32 adapter;
	u32 width;
	u32 height;
	u32 fps_num;
	u32 fps_den;
	bool fullscreen;
	enum pixel_format pix_fmt;
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
	bool own_data;
};

struct gfx_shader {
	enum gfx_shader_type type;
	void* impl;
	gfx_buffer_t* cbuffer;
	VECTOR(gfx_shader_var_t) vars;
};

struct gfx_raster_desc {
	enum gfx_culling_mode culling_mode;
	enum gfx_winding_order winding_order;
	enum gfx_raster_flags raster_flags;
	f32 depth_bias;
	f32 depth_bias_clamp;
	f32 depth_bias_slope;
	bool depth_clip_enabled;
};

BM_EXPORT inline void gfx_raster_desc_init(struct gfx_raster_desc* desc)
{
	desc->culling_mode = GFX_CULLING_BACK_FACE;
	desc->winding_order = GFX_WINDING_CW;
	desc->raster_flags = 0;
	desc->depth_bias = 0.0f;
	desc->depth_bias_clamp = 0.0f;
	desc->depth_bias_slope = 0.0f;
	desc->depth_clip_enabled = true;
}

struct gfx_blend_desc {
	bool enabled;
	bool red_enabled;
	bool green_enabled;
	bool blue_enabled;
	bool alpha_enabled;
	enum gfx_blend_mode mode_src;
	enum gfx_blend_mode mode_dst;
	enum gfx_blend_op op;
	enum gfx_blend_mode mode_src_alpha;
	enum gfx_blend_mode mode_dst_alpha;
	enum gfx_blend_op op_alpha;
};

struct gfx_sampler_desc {
	enum gfx_sample_filter filter;
	enum gfx_sample_address_mode address_u;
	enum gfx_sample_address_mode address_v;
	enum gfx_sample_address_mode address_w;
	s32 max_anisotropy;
	float border_color[4];
};

struct gfx_texture_desc {
	enum gfx_texture_type type;
	enum pixel_format pix_fmt;
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

struct gfx_mesh {
	enum gfx_vertex_type type;
	struct vec3f* positions;
	struct vec3f* normals;
	struct vec3f* tangents;
	struct vec4f* colors;
	struct texture_vertex* tex_verts;
	size_t num_vertices;
};

gfx_mesh_t* gfx_mesh_new(enum gfx_vertex_type type, u32 num_verts);
void gfx_mesh_free(gfx_mesh_t* data);
size_t gfx_mesh_get_size(const gfx_mesh_t* mesh);

struct gfx_sprite {
	struct media_image* img;
	struct gfx_texture* tex;
	vec2f_t scale;
};

struct gfx_sheet_frame {
	rect_t bbox;
	f64 duration;
};

struct gfx_sheet {
	s32 sheet_width;
	s32 sheet_height;
	s32 cel_width;
	s32 cel_height;
	s32 num_frames;
	struct gfx_sheet_frame* frames;
	struct gfx_sprite* sprite;
};

// typedef struct gs_graphics_stencil_state_desc_t
// {
//     gs_graphics_stencil_func_type   func;        // Function to set for stencil test
//     uint32_t                        ref;         // Specifies reference val for stencil test
//     uint32_t                        comp_mask;   // Specifies mask that is ANDed with both ref val and stored stencil val
//     uint32_t                        write_mask;  // Specifies mask that is ANDed with both ref val and stored stencil val
//     gs_graphics_stencil_op_type     sfail;       // Action to take when stencil test fails
//     gs_graphics_stencil_op_type     dpfail;      // Action to take when stencil test passes but depth test fails
//     gs_graphics_stencil_op_type     dppass;      // Action to take when both stencil test passes and either depth passes or is not enabled
// } gs_graphics_stencil_state_desc_t;

struct gfx_pipeline_desc {
	// gs_graphics_blend_state_desc_t blend;       // Blend state desc for pipeline
	// gs_graphics_depth_state_desc_t depth;       // Depth state desc for pipeline
	struct gfx_raster_desc raster; // Raster state desc for pipeline
	// gs_graphics_stencil_state_desc_t stencil;   // Stencil state desc for pipeline
	// gs_graphics_compute_state_desc_t compute;   // Compute state desc for pipeline
	// gs_graphics_vertex_layout_desc_t layout; // Vertex layout desc for pipeline
};

extern gfx_system_t* gfx;
extern bool gfx_hardware_ready;
extern bool gfx_system_ready;

BM_EXPORT result gfx_init(const struct gfx_config* cfg, s32 flags);
BM_EXPORT void gfx_shutdown(void);
BM_EXPORT bool gfx_hardware_ok(void);
BM_EXPORT bool gfx_ok(void);

struct input_state;
typedef struct camera_s camera_t;
typedef struct imgui_draw_data {
	camera_t* cam;
	struct input_state* inputs;
	mat4f_t* view_mat;
	mat4f_t* proj_mat;
	f64 frame_time;
	u64 frame_count;
	size_t scene_count;
} imgui_draw_data_t;
#if defined(BM_USE_CIMGUI)
BM_EXPORT bool gfx_init_cimgui(void);
BM_EXPORT void gfx_shutdown_cimgui(void);
BM_EXPORT void gfx_cimgui_begin(void);
BM_EXPORT void gfx_cimgui_frame(imgui_draw_data_t* ctx);
BM_EXPORT void gfx_cimgui_end(void);
#endif

#if defined(_WIN32)
BM_EXPORT result gfx_init_dx11(const struct gfx_config* cfg, s32 flags);
BM_EXPORT result gfx_init_renderer(const struct gfx_config* cfg, s32 flags);
BM_EXPORT void gfx_shutdown_dx11(void);
#endif

/* system ------------------------------------------------------------------ */
BM_EXPORT result gfx_enumerate_adapters(struct vector* adapters,
					bool enum_displays);
BM_EXPORT result gfx_enumerate_displays(const gfx_adapter_t* adapter,
					struct vector* displays);
BM_EXPORT void gfx_set_viewport(u32 width, u32 height);
BM_EXPORT gfx_shader_t* gfx_system_get_vertex_shader();
BM_EXPORT gfx_shader_t* gfx_system_get_pixel_shader();
BM_EXPORT void gfx_system_sampler_push(gfx_sampler_state_t* sampler);
BM_EXPORT void gfx_system_sampler_pop();
BM_EXPORT void gfx_system_bind_render_target(void);
BM_EXPORT void gfx_system_bind_input_layout(gfx_shader_t* shader);
BM_EXPORT result gfx_create_swap_chain(const struct gfx_config* cfg);
BM_EXPORT result gfx_resize_swap_chain(u32 width, u32 height, enum pixel_format pix_fmt);
BM_EXPORT result gfx_create_device(s32 adapter);
BM_EXPORT void gfx_destroy_device(void);

BM_EXPORT void gfx_render_clear(const rgba_t* color);
BM_EXPORT void gfx_render_begin(bool draw_indexed);
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
BM_EXPORT result gfx_buffer_new(const void* data, size_t size,
				enum gfx_buffer_type type,
				enum gfx_buffer_usage usage,
				gfx_buffer_t** buf);
BM_EXPORT void gfx_buffer_free(gfx_buffer_t* buf);
BM_EXPORT size_t gfx_buffer_get_size(gfx_buffer_t* buf);
BM_EXPORT result gfx_buffer_copy(gfx_buffer_t* buf, const void* data,
				 size_t size);
BM_EXPORT void gfx_bind_vertex_buffer(gfx_buffer_t* vb, u32 stride, u32 offset);
BM_EXPORT void gfx_bind_index_buffer(gfx_buffer_t* ib, u32 offset);
BM_EXPORT void gfx_buffer_upload_constants(const gfx_shader_t* shader);
BM_EXPORT enum gfx_vertex_type gfx_vertex_type_from_string(const char* s);
BM_EXPORT u8* gfx_buffer_get_data(gfx_buffer_t* buf);

/* gfx_shader --------------------------------------------------------------- */
BM_EXPORT void gfx_shader_init(gfx_shader_t* shader);
BM_EXPORT gfx_shader_t* gfx_shader_new(enum gfx_shader_type type);
BM_EXPORT gfx_shader_t* gfx_shader_adopt(gfx_shader_t* other);
BM_EXPORT gfx_shader_t* gfx_shader_copy(gfx_shader_t* other);
BM_EXPORT void gfx_shader_free(gfx_shader_t* shader);
BM_EXPORT size_t gfx_shader_cbuffer_fill(gfx_shader_t* shader);
BM_EXPORT result gfx_shader_compile_from_file(const char* path,
					      const char* entrypoint,
					      const char* target,
					      gfx_shader_t* shader);
BM_EXPORT result gfx_shader_build_program(gfx_shader_t* shader);
BM_EXPORT result gfx_shader_new_input_layout(gfx_shader_t* vs);
/* gfx_vertex_shader -------------------------------------------------------- */
BM_EXPORT void gfx_vertex_shader_init(gfx_vertex_shader_t* vs);
BM_EXPORT gfx_vertex_shader_t* gfx_vertex_shader_new(void);
BM_EXPORT bool gfx_vertex_shader_acquire(gfx_vertex_shader_t* vs);
BM_EXPORT bool gfx_vertex_shader_release(gfx_vertex_shader_t* vs);
BM_EXPORT bool gfx_vertex_shader_free(gfx_vertex_shader_t* vs);
BM_EXPORT gfx_vertex_shader_t*
gfx_vertex_shader_new_from_file(const char* path, const char* entrypoint,
				const char* target, enum gfx_vertex_type type);
BM_EXPORT enum gfx_vertex_type
gfx_vertex_shader_get_vertex_type(gfx_vertex_shader_t* vs);
BM_EXPORT void gfx_vertex_shader_set_vertex_type(gfx_vertex_shader_t* vs,
						 enum gfx_vertex_type type);
BM_EXPORT void gfx_bind_primitive_topology(enum gfx_topology topo);
BM_EXPORT void
gfx_vertex_shader_bind_input_layout(const gfx_vertex_shader_t* vs);
/* gfx_pixel_shader --------------------------------------------------------- */
BM_EXPORT void gfx_pixel_shader_init(gfx_pixel_shader_t* ps);
BM_EXPORT gfx_pixel_shader_t* gfx_pixel_shader_new();
BM_EXPORT bool gfx_pixel_shader_acquire(gfx_pixel_shader_t* ps);
BM_EXPORT bool gfx_pixel_shader_release(gfx_pixel_shader_t* ps);
BM_EXPORT bool gfx_pixel_shader_free(gfx_pixel_shader_t* ps);

/* gfx_shader_var ----------------------------------------------------------- */
BM_EXPORT gfx_shader_var_t* gfx_shader_var_new(const char* name,
					       enum gfx_shader_var_type type);
BM_EXPORT void gfx_shader_var_init(gfx_shader_var_t* var);
BM_EXPORT void gfx_shader_var_free(gfx_shader_var_t* var);
BM_EXPORT void gfx_shader_var_set(gfx_shader_var_t* var, const void* data);
BM_EXPORT void gfx_shader_var_set_data_ref(gfx_shader_var_t* var, const void* data);
BM_EXPORT bool gfx_shader_add_var(gfx_shader_t* shader,
				  const gfx_shader_var_t var);
BM_EXPORT bool gfx_shader_set_var_by_name(gfx_shader_t* shader,
					  const char* name, const void* value,
					  bool is_reference);
BM_EXPORT size_t gfx_shader_get_vars_size(gfx_shader_t* shader);
BM_EXPORT gfx_shader_var_t* gfx_shader_get_var_by_name(gfx_shader_t* shader,
						       const char* name);
BM_EXPORT size_t gfx_shader_var_size(enum gfx_shader_var_type type);
BM_EXPORT const char* gfx_shader_type_to_string(enum gfx_shader_type type);
/* sampler ----------------------------------------------------------------- */
BM_EXPORT gfx_sampler_state_t*
gfx_sampler_state_new(struct gfx_sampler_desc* desc);
BM_EXPORT void gfx_sampler_state_free(gfx_sampler_state_t* sampler);
BM_EXPORT void gfx_sampler_state_init(gfx_sampler_state_t* sampler);
BM_EXPORT void gfx_bind_sampler_state(gfx_texture_t* texture, u32 slot);

/* rasterizer -------------------------------------------------------------- */
BM_EXPORT result gfx_init_rasterizer(const struct gfx_raster_desc* desc);
BM_EXPORT void gfx_bind_rasterizer(void);

/* blend ----------------------------------------------------------------- */
BM_EXPORT gfx_blend_state_t* gfx_blend_state_new();
BM_EXPORT void gfx_blend_state_free(gfx_blend_state_t* state);
BM_EXPORT void gfx_blend_state_init(gfx_blend_state_t* state);
BM_EXPORT result gfx_blend_configure(gfx_blend_state_t* state,
				     const struct gfx_blend_desc* cfg);
BM_EXPORT void gfx_bind_blend_state();

/* texture ----------------------------------------------------------------- */
BM_EXPORT result gfx_texture_create(const u8* data,
				    const struct gfx_texture_desc* desc,
				    gfx_texture_t** texture);
BM_EXPORT void gfx_texture_init(gfx_texture_t* texture);
BM_EXPORT void gfx_texture_destroy(gfx_texture_t* texture);
BM_EXPORT result gfx_texture_from_image(struct media_image* img,
					gfx_texture_t** tex);
BM_EXPORT result gfx_texture2d_create(const u8* data,
				      const struct gfx_texture_desc* desc,
				      gfx_texture_t** texture2d);
BM_EXPORT void gfx_texture2d_destroy(gfx_texture_t* texture);
BM_EXPORT result gfx_render_target_init(u32 width, u32 height,
					enum pixel_format pf);
BM_EXPORT void gfx_render_target_destroy(void);
BM_EXPORT void gfx_set_render_targets(gfx_texture_t* texture,
				      gfx_texture_t* depth);
BM_EXPORT result gfx_create_depth_state(bool enable,
					struct gfx_depth_state** state);
BM_EXPORT result gfx_init_depth(u32 width, u32 height,
				enum pixel_format pix_fmt, bool enabled);
BM_EXPORT void gfx_destroy_depth(void);
BM_EXPORT void gfx_bind_depth_state(const struct gfx_depth_state* state);
BM_EXPORT void gfx_toggle_depth(bool enabled);

/* sprite ------------------------------------------------------------------ */
BM_EXPORT struct gfx_sprite* gfx_sprite_new();
BM_EXPORT void gfx_sprite_free(struct gfx_sprite* sprite);
BM_EXPORT result gfx_sprite_make_texture(struct gfx_sprite* sprite);
BM_EXPORT struct gfx_sheet* gfx_sheet_new(u32 num_frames);
BM_EXPORT void gfx_sheet_free(struct gfx_sheet* sheet);

/* misc -------------------------------------------------------------------- */
BM_EXPORT u32 gfx_texture_get_width(gfx_texture_t* texture);
BM_EXPORT u32 gfx_texture_get_height(gfx_texture_t* texture);
BM_EXPORT void gfx_texture_get_size(gfx_texture_t* texture, vec2f_t* size);
BM_EXPORT u32 gfx_get_vertex_stride(enum gfx_vertex_type type);
// BM_EXPORT gfx_swapchain_t* gfx_swapchain_create(const struct gfx_config* cfg);

#ifdef __cplusplus
}
#endif
