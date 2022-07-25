#pragma once

#include "gfx/enums.h"
#include "gfx/win32_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DXGI_FORMAT DXGI_FORMAT;
typedef enum D3D11_BLEND D3D11_BLEND;
typedef enum D3D11_BLEND_OP D3D11_BLEND_OP;

static const char* kDX11VertexShaderTarget = "vs_5_0";
static const char* kDX11PixelShaderTarget = "ps_5_0";

BM_EXPORT DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf);
BM_EXPORT void gfx_activate_d3d11_debug_info(void);
BM_EXPORT result gfx_com_release_d3d11(void);
BM_EXPORT D3D11_BLEND gfx_blend_mode_to_d3d11_blend(enum gfx_blend_mode mode);
BM_EXPORT D3D11_BLEND_OP gfx_blend_op_to_d3d11_blend_op(enum gfx_blend_op op);

#ifdef __cplusplus
}
#endif
