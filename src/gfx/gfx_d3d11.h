#ifndef H_GFX_D3D11
#define H_GFX_D3D11

#include "gfx/gfx.h"
#include "gfx/win32_types.h"

#include "math/vec4.h"

#include <dxgiformat.h>

static const char* kDX11VertexShaderTarget = "vs_5_0";
static const char* kDX11PixelShaderTarget = "ps_5_0";

DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf);

void gfx_activate_d3d11_debug_info(gfx_system_t* gfx);

gfx_system_t* gfx_system_init(const struct gfx_config* cfg, s32 flags);
void gfx_system_shutdown(gfx_system_t* gfx);

result gfx_com_release_d3d11(gfx_system_t* gfx);

result gfx_create_swap_chain(gfx_system_t* gfx, const struct gfx_config* cfg);

result gfx_create_device_dependent_resources(gfx_system_t* gfx, s32 adapter);
void gfx_destroy_device_dependent_resources(gfx_system_t* gfx);

gfx_shader_t* gfx_compile_shader_from_file(const char* path,
    const char* entrypoint, const char* target,
    enum gfx_shader_type type);

void gfx_bind_vertex_buffer(gfx_system_t* gfx, gfx_buffer_t* vb, u32 stride, u32 offset);
void gfx_bind_primitive_topology(gfx_system_t* gfx, enum gfx_topology topo);

result gfx_build_shader(gfx_system_t* gfx, gfx_shader_t* shader);
result gfx_create_shader_input_layout(gfx_system_t* gfx, gfx_shader_t* vs, enum gfx_vertex_type vertex_type);
void gfx_bind_input_layout(gfx_system_t* gfx, const gfx_shader_t* vs);
void gfx_set_vertex_shader(gfx_system_t* gfx, gfx_shader_t* vs);
void gfx_set_pixel_shader(gfx_system_t* gfx, gfx_shader_t* ps);
void gfx_upload_shader_vars(gfx_system_t* gfx, const gfx_shader_var_t* vars, enum gfx_shader_type type);

result gfx_create_buffer(gfx_system_t* gfx, const void* data, size_t size, enum gfx_buffer_type type, enum gfx_buffer_usage usage, gfx_buffer_t** buffer);
result gfx_buffer_copy_data(gfx_system_t* gfx, gfx_buffer_t* buffer, const void* data, size_t size);

result gfx_init_sampler_state(gfx_system_t* gfx);
void gfx_bind_sampler_state(gfx_system_t* gfx, gfx_texture_t* texture, u32 slot);

result gfx_enumerate_adapters(gfx_system_t* gfx, u32 adapter_index, u32* count);
result gfx_enumerate_adapter_monitors(gfx_system_t* gfx);

result gfx_create_texture2d(gfx_system_t* gfx, const u8* data, const struct gfx_texture_desc* desc, gfx_texture_t** texture2d);
result gfx_create_texture(gfx_system_t* gfx, const u8* data, const struct gfx_texture_desc* desc, gfx_texture_t** texture);
void gfx_destroy_texture(gfx_texture_t* texture);
result gfx_init_render_target(gfx_system_t* gfx, u32 width, u32 height, enum gfx_pixel_format pf);
void gfx_set_render_target(gfx_system_t* gfx, gfx_texture_t* texture, gfx_texture_t* zstencil);

result gfx_create_zstencil_state(gfx_system_t* gfx, bool enable, struct gfx_zstencil_state** state);
result gfx_init_zstencil(gfx_system_t* gfx, u32 width, u32 height, bool enabled);
void gfx_destroy_zstencil(gfx_system_t* gfx);
void gfx_bind_zstencil_state(gfx_system_t* gfx, const struct gfx_zstencil_state* state);

result gfx_init_rasterizer(gfx_system_t* gfx, enum gfx_culling_mode culling, enum gfx_raster_flags flags);
void gfx_bind_rasterizer(gfx_system_t* gfx);

void gfx_set_viewport(gfx_system_t* gfx, u32 width, u32 height);

void gfx_render_clear(gfx_system_t* gfx, const rgba_t* color);

void gfx_render_begin(gfx_system_t* gfx);
void gfx_render_end(gfx_system_t* gfx, bool vsync, u32 flags);

void gfx_init_sprite(gfx_system_t* gfx);
void gfx_draw_sprite(gfx_system_t* gfx, gfx_texture_t* texture, u32 width, u32 height, u32 flags);

#endif
