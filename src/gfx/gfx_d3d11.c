#include "core/logger.h"
#include "core/utils.h"

#include "platform/platform.h"

#include "gfx/gfx_d3d11.h"
#include "gfx/dxguids.h"

#define COBJMACROS
#define CINTERFACE
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment( lib, "dxgi" )
#pragma comment( lib, "d3d11" )
#pragma comment( lib, "d3dcompiler" )

#define COM_RELEASE(obj) if ((obj)) { IUnknown_Release((IUnknown*)obj); obj = NULL; }

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionUV[2] = {
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionNormalUV[3] = {
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionColor[2] = {
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionNormalColor[3] = {
    { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

struct gfx_buffer {
    u8* data;
    size_t size;
    ID3D11Buffer* buffer;
    enum gfx_usage usage;
};

struct gfx_shader {
    void* data;
    ID3DBlob* blob;
    enum gfx_shader_type type;
};

struct gfx_texture {
    u8* data;
    size_t size;
    ID3D11Texture2D* texture;
    ID3D11ShaderResourceView* srv;
};

struct gfx_zstencil {
    u8* data;
};

struct gfx_swapchain {
    struct gfx_window window;
    struct gfx_texture target;
    struct gfx_zstencil zs;
    IDXGISwapChain1* swap;
    DXGI_SWAP_CHAIN_DESC1 swap_desc;
};

struct gfx_system {
    HMODULE dxgi_dll;
    HMODULE d3d11_dll;

    IDXGIFactory2* dxgi_factory;
    IDXGIAdapter1* dxgi_adapter;
    IDXGIDevice1* dxgi_device;
    IDXGISwapChain2* dxgi_swap_chain;

    // render target
    ID3D11Texture2D* render_target;
    ID3D11RenderTargetView* rtv;

    // zbuffer
    ID3D11Texture2D* zstencil_target;
    ID3D11DepthStencilView* dsv;
    ID3D11DepthStencilState* dss;

    ID3D11Device1* device;
    ID3D11DeviceContext1* ctx;
    ID3D11SamplerState* sampler;

    DXGI_SWAP_EFFECT swap_effect;

    rect_t viewport;
};

typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY2)(UINT flags, REFIID riid, void** ppFactory);

DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf)
{
    switch (pf) {
    case GFX_FORMAT_BGRA:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case GFX_FORMAT_RGBA:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case GFX_FORMAT_NV12:
        return DXGI_FORMAT_NV12;
    default:
    case GFX_FORMAT_ARGB:
    case GFX_FORMAT_RGB24:
        return DXGI_FORMAT_UNKNOWN;
    }
    return DXGI_FORMAT_UNKNOWN;
}

void activate_d3d11_debug_info(gfx_system_t* gfx)
{
    ID3D11InfoQueue* nfo_q;
    if(SUCCEEDED(IProvideClassInfo_QueryInterface(gfx->device, &BM_IID_ID3D11InfoQueue, (void**)&nfo_q)))
    {
        ID3D11InfoQueue_SetBreakOnSeverity(nfo_q, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(nfo_q, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);

        ID3D11InfoQueue_Release(nfo_q);
    }
}

gfx_system_t* gfx_system_init(const struct gfx_config* cfg, s32 flags)
{
    (void)flags;

    gfx_system_t* gfx = (gfx_system_t*)malloc(sizeof(gfx_system_t));
    memset(gfx, 0, sizeof(*gfx));

    if (create_device_dependent_resources(gfx, cfg->adapter) != kResultOk) {
        release_d3d11_resources(gfx);
        free((void*)gfx);
        gfx = NULL;
    }

    activate_d3d11_debug_info(gfx);

    if (acquire_swap_chain(gfx, cfg) != kResultOk) {
        release_d3d11_resources(gfx);
        free((void*)gfx);
        gfx = NULL;
    }

    if (gfx_init_render_target(gfx, cfg->width, cfg->height, cfg->pix_fmt) != kResultOk ||
        gfx_init_depth_stencil(gfx, cfg->width, cfg->height, false) != kResultOk) {
        release_d3d11_resources(gfx);
        free((void*)gfx);
        gfx = NULL;
    }

    gfx_set_viewport(gfx, cfg->width, cfg->height);

    return gfx;
}

void gfx_system_shutdown(gfx_system_t* gfx)
{
    if (gfx) {
       destroy_device_dependent_resources(gfx);
        release_d3d11_resources(gfx);
        free(gfx);
        gfx = NULL;
    }
}

result enumerate_dxgi_adapters(gfx_system_t* gfx)
{
    return kResultOk;
}

result release_d3d11_resources(gfx_system_t* gfx)
{
    COM_RELEASE(gfx->dxgi_factory);
    COM_RELEASE(gfx->device);
    COM_RELEASE(gfx->ctx);
    COM_RELEASE(gfx->dxgi_swap_chain);
    COM_RELEASE(gfx->render_target);
    COM_RELEASE(gfx->rtv);
    COM_RELEASE(gfx->dsv);
    COM_RELEASE(gfx->dss);
    COM_RELEASE(gfx->sampler);

    return kResultOk;
}

result acquire_swap_chain(gfx_system_t* gfx, const struct gfx_config* cfg)
{
    result res = kResultOk;

    if(gfx->device && gfx->dxgi_factory)
    {
        HWND hwnd = (HWND)cfg->window.hwnd;
        // DXGI_SWAP_CHAIN_DESC1 swap_desc1 = {
        //     .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        //     .BufferCount = 1,
        //     .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        //     .Flags = 0,
        //     .Format = gfx_pixel_format_to_dxgi_format(cfg->pix_fmt),
        //     .Width = cfg->width,
        //     .Height = cfg->height,
        //     .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        //     .Scaling = DXGI_SCALING_NONE,
        //     .SampleDesc = { 1, 0 }
        // };
        DXGI_SWAP_CHAIN_DESC swap_desc =
        {
            .BufferDesc.Format = gfx_pixel_format_to_dxgi_format(cfg->pix_fmt),
            .BufferDesc.Width = cfg->width,
            .BufferDesc.Height = cfg->height,
            .BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
            .BufferDesc.RefreshRate.Denominator = (UINT)cfg->fps_den,
            .BufferDesc.RefreshRate.Numerator = (UINT)cfg->fps_num,
            .BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
            .SampleDesc.Count = 1,
            .SampleDesc.Quality = 0,
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .OutputWindow = hwnd,
            .Windowed = (BOOL)!cfg->fullscreen,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
        };

        IDXGISwapChain* dxgi_swap_chain = NULL;
        // if(SUCCEEDED(IDXGIFactory2_CreateSwapChainForHwnd(gfx->dxgi_factory, (IUnknown*)gfx->device, hwnd, &swap_desc1, NULL, NULL, &dxgi_swap_chain)))
        if (SUCCEEDED(IDXGIFactory2_CreateSwapChain(gfx->dxgi_factory, (IUnknown*)gfx->device, &swap_desc, &dxgi_swap_chain)))
        {
            if(SUCCEEDED(IDXGISwapChain1_QueryInterface(dxgi_swap_chain, &BM_IID_IDXGISwapChain2, (void **)&gfx->dxgi_swap_chain)))
            {
                if (cfg->fullscreen)
                    IDXGIFactory2_MakeWindowAssociation(gfx->dxgi_factory, hwnd, 0);
                else
                    IDXGIFactory2_MakeWindowAssociation(gfx->dxgi_factory, hwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
            }
            else {
                res = kResultNull;
            }

            IDXGISwapChain1_Release(dxgi_swap_chain);
        }
        else {
            res = kResultNull;
        }
    }

    if (res == kResultNull)
        logger(LOG_ERROR, "[gfx] Error creating swap chain!");

    return res;
}

result create_device_dependent_resources(gfx_system_t* gfx, s32 adapter)
{
    result res = kResultOk;
    HRESULT hr = S_OK;

    gfx->dxgi_dll = os_dlopen("dxgi.dll");
    if (!gfx->dxgi_dll) {
        res = kResultNull;
        goto cleanup;
    }

    PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2Func =
                    os_dlsym(gfx->dxgi_dll, "CreateDXGIFactory2");
    if (!CreateDXGIFactory2Func) {
        res =  kResultNull;
        goto cleanup;
    }

    UINT dxgi_flags = 0;
#if defined(BM_DEBUG)
    dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG ;
#endif

    hr = CreateDXGIFactory2Func(dxgi_flags, &BM_IID_IDXGIFactory2, (void**)&gfx->dxgi_factory);
    if (FAILED(hr)) {
        res = kResultError;
        goto cleanup;
    }

    u32 adapter_count = 0;
    res = gfx_enumerate_adapters(gfx, adapter, &adapter_count);
    if (res == kResultNotFound)
        goto cleanup;

    gfx->d3d11_dll = os_dlopen("d3d11.dll");
    if (!gfx->d3d11_dll) {
        res = kResultNull;
        goto cleanup;
    }

    PFN_D3D11_CREATE_DEVICE D3D11CreateDeviceFunc =
                    os_dlsym(gfx->d3d11_dll, "D3D11CreateDevice");
    if (!D3D11CreateDeviceFunc) {
        res = kResultNull;
        goto cleanup;
    }

    UINT create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(BM_DEBUG)
    create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL feature_levels[] =
    {
        // D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        // D3D_FEATURE_LEVEL_10_1,
        // D3D_FEATURE_LEVEL_10_0,
        // D3D_FEATURE_LEVEL_9_3,
        // D3D_FEATURE_LEVEL_9_2,
        // D3D_FEATURE_LEVEL_9_1
    };

    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
    UINT num_feature_levels = ARRAY_SIZE(feature_levels);

    hr = D3D11CreateDeviceFunc(
        (IDXGIAdapter*)gfx->dxgi_adapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        NULL,
        create_flags,
        feature_levels,
        num_feature_levels,
        D3D11_SDK_VERSION,
        (ID3D11Device**)&gfx->device,
        &feature_level,
        (ID3D11DeviceContext**)&gfx->ctx);

    if (FAILED(hr)) {
        res = kResultError;
        goto cleanup;
    }

    hr = ID3D11Device_QueryInterface(gfx->device, &BM_IID_ID3D11Device1, (void**)&gfx->device);
    if (FAILED(hr)) {
        res = kResultError;
        goto cleanup;
    }

    hr = ID3D11DeviceContext_QueryInterface(gfx->ctx, &BM_IID_ID3D11DeviceContext1, (void**)&gfx->ctx);
    if (FAILED(hr)) {
        res = kResultError;
        goto cleanup;
    }

    hr = ID3D11Device_QueryInterface(gfx->device, &BM_IID_IDXGIDevice1, (void**)&gfx->dxgi_device);
    if (FAILED(hr)) {
        res = kResultError;
        goto cleanup;
    }

cleanup:
    if (res != kResultOk) {
        COM_RELEASE(gfx->device);
        COM_RELEASE(gfx->ctx);
        COM_RELEASE(gfx->dxgi_device);
    }

    return res;
}

void destroy_device_dependent_resources(gfx_system_t* gfx)
{
    if (gfx) {
        if (gfx->d3d11_dll) {
            os_dlclose(gfx->d3d11_dll);
        }
        if (gfx->dxgi_dll) {
            os_dlclose(gfx->dxgi_dll);
        }
    }
}

gfx_shader_t* gfx_compile_shader_from_file(const char* path, const char* entrypoint, const char* target, enum gfx_shader_type type)
{
    gfx_shader_t* shader = NULL;

    size_t file_size = os_get_file_size(path);

    const char* txt = os_quick_read_utf8_file(path);

    ID3DBlob* blob = NULL;
    ID3DBlob* blob_error = NULL;
    HRESULT hr = D3DCompile(
        txt, file_size, path,
        NULL, NULL,
        entrypoint, target,
        0, 0,
        &blob, &blob_error);

    if (SUCCEEDED(hr)) {
        shader = (gfx_shader_t*)malloc(sizeof(gfx_shader_t));
        shader->blob = blob;
        shader->type = type;
    }
    else {
        if (!blob || blob_error) {
            const char* err_msg = (const char*)ID3D10Blob_GetBufferPointer(blob_error);
            logger(LOG_ERROR, "[gfx] Failed to compile shader %s. Error: %s", path, err_msg);
        }

        ID3D10Blob_Release(shader->blob);
        ID3D10Blob_Release(blob_error);

        free(shader);
    }

    return shader;
}

D3D11_USAGE gfx_buffer_usage_to_d3d11_usage(enum gfx_buffer_usage usage)
{
    D3D11_USAGE d3d11_usage = D3D11_USAGE_DEFAULT;
    switch (usage) {
    case GFX_BUFFER_USAGE_DEFAULT:
        d3d11_usage = D3D11_USAGE_DEFAULT;
        break;
    case GFX_BUFFER_USAGE_DYNAMIC:
        d3d11_usage = D3D11_USAGE_DYNAMIC;
        break;
    case GFX_BUFFER_USAGE_IMMUTABLE:
        d3d11_usage = D3D11_USAGE_IMMUTABLE;
        break;
    case GFX_BUFFER_USAGE_STAGING:
        d3d11_usage = D3D11_USAGE_STAGING;
        break;
    case GFX_BUFFER_USAGE_UNKNOWN:
    default:
        break;
    }
    return d3d11_usage;
}

result gfx_create_buffer(gfx_system_t* gfx, gfx_buffer_t* buffer, u8* data, size_t size, enum gfx_buffer_type type, enum gfx_buffer_usage usage)
{
    u32 cpu_access_flags = 0;
    if (usage == GFX_BUFFER_USAGE_DYNAMIC)
        cpu_access_flags = D3D11_CPU_ACCESS_WRITE;
    
    u32 bind_flags = 0;
    switch(type) {
    case GFX_BUFFER_VERTEX:
        bind_flags = D3D11_BIND_VERTEX_BUFFER;
        break;
    case GFX_BUFFER_INDEX:
        bind_flags = D3D11_BIND_INDEX_BUFFER;
        break;
    case GFX_BUFFER_CONSTANT:
        bind_flags = D3D11_BIND_CONSTANT_BUFFER;
        break;
    }

    D3D11_BUFFER_DESC desc = {
        .Usage = gfx_buffer_usage_to_d3d11_usage(usage),
        .CPUAccessFlags = (UINT)cpu_access_flags,
        .ByteWidth = (UINT)size,
        .BindFlags = (UINT)bind_flags,
        .MiscFlags = 0,
        .StructureByteStride = 0
    };

    HRESULT hr = S_OK;
    if (data && size) {
        D3D11_SUBRESOURCE_DATA srd = {
            .pSysMem = data,
            .SysMemPitch = 0,
            .SysMemSlicePitch = 0
        };
        hr = ID3D11Device1_CreateBuffer(gfx->device, &desc, &srd, &buffer->buffer);
    } else {
        hr = ID3D11Device1_CreateBuffer(gfx->device, &desc, NULL, &buffer->buffer);
    }

    return kResultOk;
}

result gfx_buffer_copy_data(gfx_system_t* gfx, gfx_buffer_t* buffer, u8* data, size_t size)
{
    D3D11_MAPPED_SUBRESOURCE sr;
    ID3D11DeviceContext1_Map(gfx->ctx, buffer->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sr);
    memcpy(sr.pData, data, size);
    ID3D11DeviceContext1_Unmap(gfx->ctx, buffer->buffer, 0);
}

result gfx_create_input_layout(gfx_system_t* gfx, gfx_shader_t* vs)
{
    D3D11_INPUT_ELEMENT_DESC* desc_list = NULL;
    size_t num_elems = 0;
    
    ID3D11InputLayout* input_layout = NULL;
    
    LPVOID data = ID3D10Blob_GetBufferPointer(vs->blob);
    size_t size = ID3D10Blob_GetBufferSize(vs->blob);
    
    if(FAILED(ID3D11Device1_CreateInputLayout(gfx->device, desc_list, (UINT)num_elems, data, size, &input_layout))) {
        return kResultNull;
    }

    return kResultOk;
}

result gfx_build_shader(gfx_system_t* gfx, gfx_shader_t* shader)
{
    if (!gfx || !gfx->device)
        return kResultNull;

    if (shader->blob) {
        if (shader->type == GFX_SHADER_VERTEX) {
            if (FAILED(ID3D11Device1_CreateVertexShader(
                gfx->device,
                ID3D10Blob_GetBufferPointer(shader->blob),
                ID3D10Blob_GetBufferSize(shader->blob),
                NULL, (ID3D11VertexShader**)&shader->data))) {
                logger(LOG_DEBUG, "[gfx] Failed to build vertex shader!");

                ID3D11VertexShader_Release((ID3D11VertexShader*)shader->data);
                shader->data = NULL;
            }
        } else if (shader->type == GFX_SHADER_PIXEL) {
            if (FAILED(ID3D11Device1_CreatePixelShader(
                gfx->device,
                ID3D10Blob_GetBufferPointer(shader->blob),
                ID3D10Blob_GetBufferSize(shader->blob),
                NULL, (ID3D11PixelShader**)&shader->data))) {
                logger(LOG_DEBUG, "[gfx] Failed to build pixel shader!");

                ID3D11PixelShader_Release((ID3D11PixelShader*)shader->data);
                shader->data = NULL;
            }
        }
    }

    return kResultOk;
}

result gfx_init_sampler_state(gfx_system_t* gfx)
{
    if (!gfx || !gfx->device)
        return kResultNull;

    D3D11_SAMPLER_DESC sd = {
        .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
        .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
        .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
        .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
        .MipLODBias = 0.f,
        .MaxAnisotropy = 1,
        .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
        .MinLOD = 0,
        .MaxLOD = D3D11_FLOAT32_MAX,
        .BorderColor = { 0.f, 0.f, 0.f, 0.f },
    };

    ID3D11SamplerState* ss = NULL;

    if (FAILED(ID3D11Device1_CreateSamplerState(gfx->device, &sd, &ss))) {
        logger(LOG_DEBUG, "[gfx] Failed to create sampler state!");

        ID3D11SamplerState_Release(ss);
        ss = NULL;

        return kResultError;
    }

    return kResultOk;
}

static bool gfx_get_monitor_target(const MONITORINFOEX* info, DISPLAYCONFIG_TARGET_DEVICE_NAME* target)
{
	bool found = false;

	u32 num_paths = 0;
    u32 num_modes = 0;
	if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &num_paths, &num_modes) == ERROR_SUCCESS) {
        DISPLAYCONFIG_PATH_INFO* paths = (DISPLAYCONFIG_PATH_INFO*)malloc(num_paths * (sizeof(DISPLAYCONFIG_PATH_INFO)));
        DISPLAYCONFIG_MODE_INFO* modes = (DISPLAYCONFIG_MODE_INFO*)malloc(num_modes * (sizeof(DISPLAYCONFIG_MODE_INFO)));
        if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &num_paths, paths, &num_modes, modes, NULL) == ERROR_SUCCESS) {
			// realloc(paths, num_paths * sizeof(DISPLAYCONFIG_PATH_INFO));
			for (size_t i = 0; i < num_paths; ++i) {
				const DISPLAYCONFIG_PATH_INFO path = paths[i];
				DISPLAYCONFIG_SOURCE_DEVICE_NAME source;
                source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                source.header.size = sizeof(source);
                source.header.adapterId = path.sourceInfo.adapterId;
				source.header.id = path.sourceInfo.id;
				if (DisplayConfigGetDeviceInfo(&source.header) == ERROR_SUCCESS) {
                    // wchar_t info_device[512] = L"";
                    // os_utf8_to_wcs(info->szDevice, 0, info_device, 512);
                    if (wcscmp(info->szDevice, source.viewGdiDeviceName) == 0) {
                        target->header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
                        target->header.size = sizeof(*target);
                        target->header.adapterId = path.sourceInfo.adapterId;
                        target->header.id = path.targetInfo.id;
                        found = DisplayConfigGetDeviceInfo(&target->header) == ERROR_SUCCESS;
                        break;
                    }
				}
			}
		}
        free(paths);
        free(modes);
        paths = NULL;
        modes = NULL;
	}
	return found;
}

result gfx_enumerate_adapters(gfx_system_t* gfx, u32 adapter_index, u32* count)
{
    UINT i = 0;

    result res = kResultNotFound;

    for (i = 0; IDXGIFactory2_EnumAdapters1(gfx->dxgi_factory, i, &gfx->dxgi_adapter) == S_OK; i++) {
        DXGI_ADAPTER_DESC desc;

        IDXGIAdapter1_GetDesc(gfx->dxgi_adapter, &desc);

        /* ignore Microsoft's 'basic' renderer */
		if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
			continue;

        char adapter_name[512] = "";

        os_wcs_to_utf8(desc.Description, 0, adapter_name, sizeof adapter_name);

        logger(LOG_INFO, "[gfx]\tDXGI Adapter %u: %s", i, (const char*)&adapter_name[0]);
        logger(LOG_INFO, "     \tDedicated VRAM: %u", desc.DedicatedVideoMemory);
        logger(LOG_INFO, "     \tShared VRAM:    %u", desc.SharedSystemMemory);
        logger(LOG_INFO, "     \tPCI ID:         %x:%x", desc.VendorId, desc.DeviceId);
        logger(LOG_INFO, "     \tRevision:       %u", desc.Revision);

        LARGE_INTEGER umd;
        if (SUCCEEDED(IDXGIAdapter1_CheckInterfaceSupport(gfx->dxgi_adapter, &BM_IID_IDXGIDevice, &umd))) {
            const u64 version = umd.QuadPart;
            const u16 aa = (version >> 48) & 0xffff;
            const u16 bb = (version >> 32) & 0xffff;
            const u16 ccccc = (version >> 16) &0xffff;
            const u64 ddddd = version & 0xffff;
            logger(LOG_INFO, "     \tDriver Version: %u.%u.%u.%u", aa, bb, ccccc, ddddd);
        }
        else {
            logger(LOG_WARNING, "     \tDriver Version: Unknown");
        }

        if (adapter_index == i) {
            res = kResultOk;
            break;
        }

        logger(LOG_INFO, "[gfx] Monitors");
        gfx_enumerate_adapter_monitors(gfx);

        (*count)++;
    }

    return res;
}

result gfx_enumerate_adapter_monitors(gfx_system_t* gfx)
{
    if (!gfx && !gfx->dxgi_adapter)
        return kResultNull;

    IDXGIOutput* dxgi_output = NULL;
    u32 output_index = 0;
    for (output_index = 0; IDXGIAdapter1_EnumOutputs(gfx->dxgi_adapter, output_index, &dxgi_output) == S_OK; output_index++) {
        DXGI_OUTPUT_DESC desc;
        if (FAILED(IDXGIOutput_GetDesc(dxgi_output, &desc)))
            continue;

        bool target_found = false;
        DISPLAYCONFIG_TARGET_DEVICE_NAME target;

        MONITORINFOEX nfo;
        nfo.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(desc.Monitor, (LPMONITORINFO)&nfo)) {
            target_found = gfx_get_monitor_target(&nfo, &target);

            DEVMODE mode;
            mode.dmSize = sizeof(mode);
            mode.dmDriverExtra = 0;

            u32 refresh_rate = 0;
            if (EnumDisplaySettings(nfo.szDevice, ENUM_CURRENT_SETTINGS, &mode)) {
                refresh_rate = mode.dmDisplayFrequency;
            }
            const RECT rect = desc.DesktopCoordinates;
            char device_name[512] = "";
            char friendly_name[512] = "";
            os_wcs_to_utf8(desc.DeviceName, 0, device_name, sizeof desc.DeviceName);
            os_wcs_to_utf8(target.monitorFriendlyDeviceName, 0, friendly_name, sizeof target.monitorFriendlyDeviceName);
            logger(LOG_INFO, "     \tOutput %u:     %s", output_index, device_name);
            logger(LOG_INFO, "     \tFriendly Name: %s", friendly_name);
            logger(LOG_INFO, "     \tPosition:      %d, %d", rect.left, rect.top);
            logger(LOG_INFO, "     \tSize:          %d, %d", rect.right - rect.left, rect.bottom - rect.top);
            logger(LOG_INFO, "     \tAttached:      %s", desc.AttachedToDesktop ? "true" : "false");
            logger(LOG_INFO, "     \tRefresh Rate:  %uhz", refresh_rate);
        }
    }
    return kResultOk;
}

result gfx_create_texture(gfx_system_t* gfx, u8* data, const struct gfx_texture_desc* desc, struct gfx_texture* texture)
{
    DXGI_FORMAT dxgi_format = gfx_pixel_format_to_dxgi_format(desc->pix_fmt);

    u32 bits_per_pixel = gfx_get_bits_per_pixel(desc->pix_fmt);

    DXGI_SAMPLE_DESC sample_desc = {
        .Count = 1,
        .Quality = 0,
    };

    u32 bind_flags = D3D11_BIND_SHADER_RESOURCE;
    if (desc->flags & GFX_TEXTURE_IS_RENDER_TARGET)
        bind_flags |= D3D11_BIND_RENDER_TARGET;

    u32 misc_flags = 0;
    if (desc->flags & GFX_TEXTURE_SHARED && !(desc->flags & GFX_TEXTURE_KEYED_MUTEX))
        misc_flags |= D3D11_RESOURCE_MISC_SHARED;
    else if (!(desc->flags & GFX_TEXTURE_SHARED) && desc->flags & GFX_TEXTURE_KEYED_MUTEX)
        misc_flags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    bool is_dynamic = desc->flags & GFX_TEXTURE_DYNAMIC;

    D3D11_TEXTURE2D_DESC tex_desc = {
        .Width = desc->width,
        .Height = desc->height,
        .MipLevels = desc->flags & GFX_TEXTURE_GENERATE_MIPMAPS ? 0UL : desc->mip_levels,
        .ArraySize = 1UL,
        .Format = dxgi_format,
        .SampleDesc = sample_desc,
        .Usage = is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
        .BindFlags = bind_flags,
        .CPUAccessFlags = is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0UL,
        .MiscFlags = misc_flags,
    };

    u32 stride = desc->width * (bits_per_pixel / 8);
    u32 data_size = desc->height * stride;

    D3D11_SUBRESOURCE_DATA srd = {
        .pSysMem = (void*)data,
        .SysMemPitch = stride,
        .SysMemSlicePitch = data_size,
    };

    texture->data = data;
    texture->size = data_size;

    if (FAILED(ID3D11Device1_CreateTexture2D(gfx->device, &tex_desc, &srd, &texture->texture)))
        return kResultNull;

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
        .Format = dxgi_format,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D.MipLevels = desc->mip_levels,
        .Texture2D.MostDetailedMip = desc->mip_levels - 1,
    };

    if (FAILED(ID3D11Device1_CreateShaderResourceView(gfx->device, (ID3D11Resource*)texture->texture, &srv_desc, &texture->srv)))
        return kResultNull;

    return kResultOk;
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_init_render_target(gfx_system_t* gfx, u32 width, u32 height, enum gfx_pixel_format pf)
{
    HRESULT hr = IDXGISwapChain2_GetBuffer(gfx->dxgi_swap_chain, 0, &BM_IID_ID3D11Texture2D, (void**)&gfx->render_target);
    if (FAILED(hr))
        return kResultNull;

    // D3D11_RENDER_TARGET_VIEW_DESC rtv_desc; // TODO?
    hr = ID3D11Device1_CreateRenderTargetView(gfx->device, (ID3D11Resource*)gfx->render_target, NULL, &gfx->rtv);
    if (FAILED(hr))
        return kResultNull;

    return kResultOk;
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_init_depth_stencil(gfx_system_t* gfx, u32 width, u32 height, bool enabled)
{
    DXGI_FORMAT zs_format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    D3D11_DEPTH_STENCIL_DESC zs_desc = {
        .DepthEnable = enabled,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D11_COMPARISON_LESS,
        .StencilEnable = true,
        .StencilReadMask = 0xFF,
        .StencilWriteMask = 0xFF,
        .FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
        .FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR,
        .FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
        .FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
        .BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
        .BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR,
        .BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
        .BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
    };

    if (FAILED(ID3D11Device1_CreateDepthStencilState(gfx->device, &zs_desc, &gfx->dss)))
        return kResultNull;

    if (enabled) {
        D3D11_TEXTURE2D_DESC tex_desc = {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = zs_format,
            .SampleDesc.Count = 1,
            .SampleDesc.Quality = 0,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0,
            .MiscFlags = 0,
        };

        if (FAILED(ID3D11Device_CreateTexture2D(gfx->device, &tex_desc, NULL, &gfx->zstencil_target)))
            return kResultNull;

        D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
            .Format = zs_format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D.MipSlice = 0,
        };

        if (FAILED(ID3D11Device_CreateDepthStencilView(gfx->device, (ID3D11Resource*)gfx->zstencil_target, &dsv_desc, &gfx->dsv)))
            return kResultNull;
    }

    return kResultOk;
}

void gfx_bind_zstencil(gfx_system_t* gfx)
{
    ID3D11DeviceContext1_OMSetDepthStencilState(gfx->ctx, gfx->dss, 1);
}

result gfx_init_rasterizer(gfx_system_t* gfx, enum gfx_culling_mode culling, enum gfx_raster_flags flags)
{
    return kResultOk;
}

void gfx_set_viewport(gfx_system_t* gfx, u32 width, u32 height)
{
    D3D11_VIEWPORT viewport = {
        .Width = (FLOAT)width,
        .Height = (FLOAT)height,
        .MaxDepth = 1.f,
        .MinDepth = 0.f,
        .TopLeftX = 0.f,
        .TopLeftY = 0.f,
    };

    ID3D11DeviceContext1_RSSetViewports(gfx->ctx, 1, &viewport);
}

void gfx_render_clear(gfx_system_t* gfx, const rgba_t* color)
{
    if (gfx && gfx->ctx) {
        if (gfx->rtv) {
            ID3D11DeviceContext1_ClearRenderTargetView(gfx->ctx, gfx->rtv, (FLOAT*)color);
        }

        if (gfx->dsv) {
            u32 dsv_clear_flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
            u8 stencil = 0;
            ID3D11DeviceContext1_ClearDepthStencilView(gfx->ctx, gfx->dsv, dsv_clear_flags, 1.f, stencil);
        }
    }
}

void gfx_render_begin(gfx_system_t* gfx)
{
    return;
}

void gfx_render_end(gfx_system_t* gfx, bool vsync, u32 flags)
{
    IDXGISwapChain1_Present(gfx->dxgi_swap_chain, (UINT)vsync, flags);
}
