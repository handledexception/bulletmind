#ifndef H_GFX_D3D11
#define H_GFX_D3D11

#include "core/types.h"

#include "gfx/gfx.h"
#include "gfx/win32_types.h"

#include "math/vec2.h"

#include <dxgiformat.h>

static const char* kDX11VertexShaderTarget = "vs_5_0";
static const char* kDX11PixelShaderTarget = "ps_5_0";

DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf);

void activate_d3d11_debug_info(gfx_system_t* gfx);

gfx_system_t* gfx_system_init(const struct gfx_config* cfg, s32 flags);
void gfx_system_shutdown(gfx_system_t* gfx);

result enumerate_dxgi_adapters(gfx_system_t* gfx);

result release_d3d11_resources(gfx_system_t* gfx);

result acquire_swap_chain(gfx_system_t* gfx, const struct gfx_config* cfg);

result create_device_dependent_resources(gfx_system_t* gfx, s32 adapter);
void destroy_device_dependent_resources(gfx_system_t* gfx);

gfx_shader_t* gfx_compile_shader_from_file(const char* path,
    const char* entrypoint, const char* target,
    enum gfx_shader_type type);

result gfx_create_buffer(gfx_system_t* gfx, gfx_buffer_t* buffer, u8* data, size_t size, enum gfx_buffer_type type, enum gfx_buffer_usage usage);
result gfx_buffer_copy_data(gfx_system_t* gfx, gfx_buffer_t* buffer, u8* data, size_t size);

result gfx_create_input_layout(gfx_system_t* gfx, gfx_shader_t* vs);

result gfx_build_shader(gfx_system_t* gfx, gfx_shader_t* shader);

result gfx_init_sampler_state(gfx_system_t* gfx);

result gfx_enumerate_adapters(gfx_system_t* gfx, u32 adapter_index, u32* count);
result gfx_enumerate_adapter_monitors(gfx_system_t* gfx);

result gfx_create_texture(gfx_system_t* gfx, u8* data, const struct gfx_texture_desc* desc, struct gfx_texture* texture);
result gfx_init_render_target(gfx_system_t* gfx, u32 width, u32 height, enum gfx_pixel_format pf);

result gfx_init_depth_stencil(gfx_system_t* gfx, u32 width, u32 height, bool enabled);
void gfx_bind_zstencil(gfx_system_t* gfx);

result gfx_init_rasterizer(gfx_system_t* gfx, enum gfx_culling_mode culling, enum gfx_raster_flags flags);

void gfx_set_viewport(gfx_system_t* gfx, u32 width, u32 height);

void gfx_render_clear(gfx_system_t* gfx, const rgba_t* color);

void gfx_render_begin(gfx_system_t* gfx);
void gfx_render_end(gfx_system_t* gfx, bool vsync, u32 flags);

#endif
