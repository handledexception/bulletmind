#pragma once

#include "core/types.h"

#include "gfx/gfx.h"

#define COBJMACROS
#define CINTERFACE
#include <d3d11_1.h>

#define COM_RELEASE(obj) if ((obj)) { IUnknown_Release((IUnknown*)obj); obj = NULL; }

struct gfx_system {
    HMODULE dxgi_dll;
    HMODULE d3d11_dll;

    IDXGIFactory2* dxgi_factory;
    IDXGIDevice1* dxgi_device;
    IDXGISwapChain1* swap_chain;

    IDXGIAdapter* adapter;
    ID3D11Device1* device;
    ID3D11DeviceContext1* ctx;
    ID3D11RenderTargetView* current_rtv;
    ID3D11RenderTargetView* offscreen_rtv;
    ID3D11SamplerState* sampler;

    DXGI_SWAP_EFFECT swap_effect;

    rect_t viewport;
};

result release_d3d11_resources(gfx_system_t* gfx);

result create_device_dependent_resources(gfx_system_t* gfx);
