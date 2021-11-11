#ifndef H_GFX_D3D11
#define H_GFX_D3D11

#include "gfx/gfx.h"
#include "gfx/win32_types.h"

#include <dxgiformat.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char* kDX11VertexShaderTarget = "vs_5_0";
static const char* kDX11PixelShaderTarget = "ps_5_0";

BM_EXPORT DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf);
BM_EXPORT void gfx_activate_d3d11_debug_info(gfx_system_t* gfx);
BM_EXPORT result gfx_com_release_d3d11(gfx_system_t* gfx);

#ifdef __cplusplus
}
#endif

#endif // H_GFX_D3D11
