#include "core/utils.h"

#include "platform/platform.h"

#include "gfx/gfx_d3d11.h"

static const GUID BM_IID_IDXGIFactory2 = { 0x50c83a1c, 0xe072, 0x4c48, { 0x87, 0xb0, 0x36, 0x30, 0xfa, 0x36, 0xa6, 0xd0 } };
static const GUID BM_IID_IDXGIDevice1 = { 0x77db970f, 0x6276, 0x48ba, { 0xba, 0x28, 0x07, 0x01, 0x43, 0xb4, 0x39, 0x2c } };
static const GUID BM_IID_IDXGIDevice3 = { 0x6007896c, 0x3244, 0x4afd, { 0xbf, 0x18, 0xa6, 0xd3, 0xbe, 0xda, 0x50, 0x23 } };
static const GUID BM_IID_ID3D11Texture2D = { 0x6f15aaf2, 0xd208, 0x4e89, { 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c } };
static const GUID BM_IID_ID3D11Device1 = { 0xa04bfb29, 0x08ef, 0x43d6, { 0xa4, 0x9c, 0xa9, 0xbd, 0xbd, 0xcb, 0xe6, 0x86 } };
static const GUID BM_IID_ID3D11DeviceContext1 = { 0xbb2c6faa, 0xb5fb, 0x4082, { 0x8e, 0x6b, 0x38, 0x8b, 0x8c, 0xfa, 0x90, 0xe1 } };
static const GUID BM_IID_ID3D11Debug = { 0x79cf2233, 0x7536, 0x4948, { 0x9d, 0x36, 0x1e, 0x46, 0x92, 0xdc, 0x57, 0x60 } };

typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY)(REFIID riid, void** ppFactory);

result release_d3d11_resources(gfx_system_t* gfx)
{
    COM_RELEASE(gfx->dxgi_factory);
    COM_RELEASE(gfx->adapter);
    COM_RELEASE(gfx->device);
    COM_RELEASE(gfx->ctx);
    COM_RELEASE(gfx->swap_chain);
    COM_RELEASE(gfx->current_rtv);
    COM_RELEASE(gfx->offscreen_rtv);
    COM_RELEASE(gfx->sampler);

    return kResultOk;
}

result create_device_dependent_resources(gfx_system_t* gfx)
{
    result res = kResultOk;
    HRESULT hr = S_OK;

    gfx->dxgi_dll = os_dlopen("dxgi.dll");
    if (!gfx->dxgi_dll) {
        res = kResultNullPointer;
        goto done;
    }

    PFN_CREATE_DXGI_FACTORY CreateDXGIFactoryFunc =
                    os_dlsym(gfx->dxgi_dll, "CreateDXGIFactory");
    if (!CreateDXGIFactoryFunc) {
        res =  kResultNullPointer;
        goto done;
    }

    hr = CreateDXGIFactoryFunc(&BM_IID_IDXGIFactory2, (void**)&gfx->dxgi_factory);
    if (FAILED(hr)) {
        res = kResultError;
        goto done;
    }

    hr = IDXGIFactory2_EnumAdapters(gfx->dxgi_factory, 0, &gfx->adapter);
    if (FAILED(hr)) {
        res = kResultError;
        goto done;
    }

    gfx->d3d11_dll = os_dlopen("d3d11.dll");
    if (!gfx->d3d11_dll) {
        res = kResultNullPointer;
        goto done;
    }

    PFN_D3D11_CREATE_DEVICE D3D11CreateDeviceFunc =
                    os_dlsym(gfx->d3d11_dll, "D3D11CreateDevice");
    if (!D3D11CreateDeviceFunc) {
        res = kResultNullPointer;
        goto done;
    }

    UINT create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(BM_DEBUG)
    create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL feature_levels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
    UINT num_feature_levels = ARRAY_SIZE(feature_levels);

    hr = D3D11CreateDeviceFunc(
        gfx->adapter,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        create_flags,
        feature_levels,
        num_feature_levels,
        D3D11_SDK_VERSION,
        &gfx->device,
        &feature_level,
        &gfx->ctx);

    if (FAILED(hr)) {
        res = kResultError;
        goto done;
    }

done:
    COM_RELEASE(gfx->device);
    COM_RELEASE(gfx->ctx);
    COM_RELEASE(gfx->dxgi_device);

    return res;
}
