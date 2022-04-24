#include "core/logger.h"
#include "core/memory.h"
#include "core/utils.h"
#include "core/vector.h"

#include "math/types.h"

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

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#define COM_RELEASE(obj)                          \
	if ((obj)) {                              \
		IUnknown_Release((IUnknown*)obj); \
		obj = NULL;                       \
	}

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionUV[2] = {
	{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionNormalUV[3] = {
	{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionColor[2] = {
	{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

static const D3D11_INPUT_ELEMENT_DESC kVertexDescPositionNormalColor[3] = {
	{"SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct gfx_buffer {
	u8* data;
	size_t size;
	ID3D11Buffer* buffer;
	enum gfx_buffer_type type;
	enum gfx_buffer_usage usage;
};

struct gfx_vertex_shader {
	enum gfx_vertex_type vertex_type;
	ID3DBlob* blob;
	ID3D11VertexShader* program;
	ID3D11InputLayout* input_layout;
};

struct gfx_pixel_shader {
	ID3DBlob* blob;
	ID3D11PixelShader* program;
};

struct gfx_texture2d {
	enum gfx_pixel_format pix_fmt;
	u32 width;
	u32 height;
	u32 mip_levels;
	u32 flags;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
};

struct gfx_zstencil_state {
	bool enabled;
	ID3D11DepthStencilState* dss;
};

struct gfx_texture {
	enum gfx_texture_type type;
	u8* data;
	size_t size;
	void* impl; // gfx_texture2d, etc.
};

struct gfx_system {
	/* dynamic libs */
	HMODULE dxgi_dll;
	HMODULE d3d11_dll;

	/* dxgi */
	IDXGIFactory2* dxgi_factory;
	IDXGIAdapter1* dxgi_adapter;
	IDXGIDevice1* dxgi_device;
	IDXGISwapChain2* dxgi_swap_chain;

	/* d3d11 device & context */
	ID3D11Device1* device;
	ID3D11DeviceContext1* ctx;

	/* render target */
	gfx_texture_t* render_target;

	/* z-buffer */
	gfx_texture_t* zstencil_target;
	struct gfx_zstencil_state* zstencil_state_enabled;
	struct gfx_zstencil_state* zstencil_state_disabled;

	/* sampler */
	ID3D11SamplerState* sampler_state;

	/* rasterizer */
	struct gfx_raster_opts raster_opts;
	ID3D11RasterizerState* raster_state;

	gfx_shader_t* vertex_shader;
	gfx_shader_t* pixel_shader;
	gfx_buffer_t* sprite_vb;
	gfx_buffer_t* cube_vb;

	DXGI_SWAP_EFFECT swap_effect;

	rect_t viewport;
};

typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY2)(UINT flags, REFIID riid,
						  void** ppFactory);

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

DXGI_FORMAT gfx_pixel_format_to_dxgi_format(enum gfx_pixel_format pf)
{
	switch (pf) {
	case GFX_FORMAT_BGRA:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case GFX_FORMAT_RGBA:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case GFX_FORMAT_NV12:
		return DXGI_FORMAT_NV12;
	case GFX_FORMAT_DEPTH_U16:
		return DXGI_FORMAT_D16_UNORM;
	case GFX_FORMAT_DEPTH_U24:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case GFX_FORMAT_DEPTH_F32:
		return DXGI_FORMAT_D32_FLOAT;
	default:
	case GFX_FORMAT_ARGB:
	case GFX_FORMAT_RGB24:
		return DXGI_FORMAT_UNKNOWN;
	}
	return DXGI_FORMAT_UNKNOWN;
}

D3D11_CULL_MODE
gfx_culling_mode_to_d3d11_cull_mode(enum gfx_culling_mode culling)
{
	switch (culling) {
	case GFX_CULLING_FRONT_FACE:
		return D3D11_CULL_FRONT;
	case GFX_CULLING_BACK_FACE:
		return D3D11_CULL_BACK;
	default:
	case GFX_CULLING_NONE:
		break;
	}
	return D3D11_CULL_NONE;
}

D3D11_PRIMITIVE_TOPOLOGY gfx_topology_to_d3d11_topology(enum gfx_topology topo)
{
	switch (topo) {
	case GFX_TOPOLOGY_POINT_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case GFX_TOPOLOGY_LINE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	case GFX_TOPOLOGY_LINE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case GFX_TOPOLOGY_TRIANGLE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case GFX_TOPOLOGY_TRIANGLE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	}

	return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
}

void gfx_activate_d3d11_debug_info(gfx_system_t* gfx)
{
	ID3D11InfoQueue* nfo_q;
	if (SUCCEEDED(IProvideClassInfo_QueryInterface(
		    gfx->device, &BM_IID_ID3D11InfoQueue, (void**)&nfo_q))) {
		// ID3D11InfoQueue_SetBreakOnSeverity(nfo_q, D3D11_MESSAGE_SEVERITY_INFO, TRUE);
		// ID3D11InfoQueue_SetBreakOnSeverity(nfo_q, D3D11_MESSAGE_SEVERITY_MESSAGE, TRUE);
		// ID3D11InfoQueue_SetBreakOnSeverity(nfo_q, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
		ID3D11InfoQueue_SetBreakOnSeverity(
			nfo_q, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		ID3D11InfoQueue_SetBreakOnSeverity(
			nfo_q, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
		ID3D11InfoQueue_Release(nfo_q);
	}
}

result gfx_com_release_d3d11(gfx_system_t* gfx)
{
	COM_RELEASE(gfx->sampler_state);
	COM_RELEASE(gfx->raster_state);
	COM_RELEASE(gfx->dxgi_swap_chain);
	COM_RELEASE(gfx->dxgi_device);
	COM_RELEASE(gfx->ctx);
	COM_RELEASE(gfx->device);
	COM_RELEASE(gfx->dxgi_adapter);
	COM_RELEASE(gfx->dxgi_factory);
	// COM_RELEASE(gfx->render_target);
	// COM_RELEASE(gfx->rtv);
	// COM_RELEASE(gfx->zstencil_target);
	// COM_RELEASE(gfx->dsv);
	// COM_RELEASE(gfx->dss);
	return RESULT_OK;
}

void gfx_bind_vertex_shader_input_layout(gfx_system_t* gfx)
{
	if (gfx && gfx->vertex_shader) {
		gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)gfx->vertex_shader->impl;
		if (vs)
			gfx_vertex_shader_bind_input_layout(gfx, vs);
	}
}

//
// gfx system
//
result gfx_enumerate_adapters(gfx_system_t* gfx, u32 adapter_index, u32* count)
{
	UINT i = 0;

	result res = RESULT_NOT_FOUND;

	IDXGIAdapter1* dxgi_adapter = NULL;
	for (i = 0; IDXGIFactory2_EnumAdapters1(gfx->dxgi_factory, i,
						&dxgi_adapter) == S_OK;
	     i++) {
		DXGI_ADAPTER_DESC desc;

		IDXGIAdapter1_GetDesc(dxgi_adapter, &desc);

		/* ignore Microsoft's 'basic' renderer */
		if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
			continue;

		char adapter_name[512] = "";

		os_wcs_to_utf8(desc.Description, 0, adapter_name,
			       sizeof adapter_name);
		logger(LOG_INFO, "\033[7mgfx\033[m\tDXGI Adapter %u: %s", i,
		       (const char*)&adapter_name[0]);
		logger(LOG_INFO, "     \tDedicated VRAM: %u",
		       desc.DedicatedVideoMemory);
		logger(LOG_INFO, "     \tShared VRAM:    %u",
		       desc.SharedSystemMemory);
		logger(LOG_INFO, "     \tPCI ID:         %x:%x", desc.VendorId,
		       desc.DeviceId);
		logger(LOG_INFO, "     \tRevision:       %u", desc.Revision);

		LARGE_INTEGER umd;
		if (SUCCEEDED(IDXGIAdapter1_CheckInterfaceSupport(
			    dxgi_adapter, &BM_IID_IDXGIDevice, &umd))) {
			const u64 version = umd.QuadPart;
			const u16 aa = (version >> 48) & 0xffff;
			const u16 bb = (version >> 32) & 0xffff;
			const u16 ccccc = (version >> 16) & 0xffff;
			const u64 ddddd = version & 0xffff;
			logger(LOG_INFO, "     \tDriver Version: %u.%u.%u.%u",
			       aa, bb, ccccc, ddddd);
		} else {
			logger(LOG_WARNING, "     \tDriver Version: Unknown");
		}

		if (adapter_index == i) {
			res = RESULT_OK;
			gfx->dxgi_adapter = dxgi_adapter;
			// break;
		}

		logger(LOG_INFO, "[gfx] Monitors");
		gfx_enumerate_adapter_monitors(gfx);

		(*count)++;
	}

	return res;
}

static bool gfx_get_monitor_target(const MONITORINFOEX* info,
				   DISPLAYCONFIG_TARGET_DEVICE_NAME* target)
{
	bool found = false;

	u32 num_paths = 0;
	u32 num_modes = 0;
	if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &num_paths,
					&num_modes) == ERROR_SUCCESS) {
		DISPLAYCONFIG_PATH_INFO* paths =
			(DISPLAYCONFIG_PATH_INFO*)mem_alloc(
				num_paths * (sizeof(DISPLAYCONFIG_PATH_INFO)));
		DISPLAYCONFIG_MODE_INFO* modes =
			(DISPLAYCONFIG_MODE_INFO*)mem_alloc(
				num_modes * (sizeof(DISPLAYCONFIG_MODE_INFO)));
		if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &num_paths, paths,
				       &num_modes, modes,
				       NULL) == ERROR_SUCCESS) {
			// realloc(paths, num_paths * sizeof(DISPLAYCONFIG_PATH_INFO));
			for (size_t i = 0; i < num_paths; ++i) {
				const DISPLAYCONFIG_PATH_INFO path = paths[i];
				DISPLAYCONFIG_SOURCE_DEVICE_NAME source;
				source.header.type =
					DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
				source.header.size = sizeof(source);
				source.header.adapterId =
					path.sourceInfo.adapterId;
				source.header.id = path.sourceInfo.id;
				if (DisplayConfigGetDeviceInfo(
					    &source.header) == ERROR_SUCCESS) {
					// wchar_t info_device[512] = L"";
					// os_utf8_to_wcs(info->szDevice, 0, info_device, 512);
					if (wcscmp(info->szDevice,
						   source.viewGdiDeviceName) ==
					    0) {
						target->header.type =
							DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
						target->header.size =
							sizeof(*target);
						target->header.adapterId =
							path.sourceInfo
								.adapterId;
						target->header.id =
							path.targetInfo.id;
						found = DisplayConfigGetDeviceInfo(
								&target->header) ==
							ERROR_SUCCESS;
						break;
					}
				}
			}
		}
		mem_free(paths);
		mem_free(modes);
		paths = NULL;
		modes = NULL;
	}
	return found;
}

result gfx_enumerate_adapter_monitors(gfx_system_t* gfx)
{
	if (!gfx && !gfx->dxgi_adapter)
		return RESULT_NULL;

	IDXGIOutput* dxgi_output = NULL;
	u32 output_index = 0;
	for (output_index = 0;
	     IDXGIAdapter1_EnumOutputs(gfx->dxgi_adapter, output_index,
				       &dxgi_output) == S_OK;
	     output_index++) {
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
			if (EnumDisplaySettings(nfo.szDevice,
						ENUM_CURRENT_SETTINGS, &mode)) {
				refresh_rate = mode.dmDisplayFrequency;
			}
			const RECT rect = desc.DesktopCoordinates;
			char device_name[512] = "";
			char friendly_name[512] = "";
			os_wcs_to_utf8(desc.DeviceName, 0, device_name,
				       sizeof desc.DeviceName);
			os_wcs_to_utf8(target.monitorFriendlyDeviceName, 0,
				       friendly_name,
				       sizeof target.monitorFriendlyDeviceName);
			logger(LOG_INFO, "     \tOutput %u:     %s",
			       output_index, device_name);
			logger(LOG_INFO, "     \tFriendly Name: %s",
			       friendly_name);
			logger(LOG_INFO, "     \tPosition:      %d, %d",
			       rect.left, rect.top);
			logger(LOG_INFO, "     \tSize:          %d, %d",
			       rect.right - rect.left, rect.bottom - rect.top);
			logger(LOG_INFO, "     \tAttached:      %s",
			       desc.AttachedToDesktop ? "true" : "false");
			logger(LOG_INFO, "     \tRefresh Rate:  %uhz",
			       refresh_rate);
		}
	}
	return RESULT_OK;
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

// TODO(paulh): Refactor to initialize D3D11 once, and allow instantiating multple swap chains
gfx_system_t* gfx_system_init(const struct gfx_config* cfg, s32 flags)
{
	gfx_system_t* gfx = (gfx_system_t*)mem_alloc(sizeof(gfx_system_t));
	memset(gfx, 0, sizeof(*gfx));

	if (gfx_create_device_dependent_resources(gfx, cfg->adapter) !=
	    RESULT_OK) {
		gfx_com_release_d3d11(gfx);
		mem_free((void*)gfx);
		gfx = NULL;
		return gfx;
	}
	logger(LOG_INFO,
	       "\033[7mgfx\033[m Created device dependent resources\n");

	gfx_activate_d3d11_debug_info(gfx);

	if (gfx_create_swap_chain(gfx, cfg) != RESULT_OK) {
		gfx_com_release_d3d11(gfx);
		mem_free((void*)gfx);
		gfx = NULL;
		return gfx;
	}
	logger(LOG_INFO, "\033[7mgfx\033[m Created swap chain\n");

	if (gfx_init_render_target(gfx, cfg->width, cfg->height,
				   cfg->pix_fmt) != RESULT_OK ||
	    gfx_init_zstencil(gfx, cfg->width, cfg->height,
			      GFX_FORMAT_DEPTH_U24,
			      flags & GFX_USE_ZBUFFER) != RESULT_OK) {
		gfx_com_release_d3d11(gfx);
		mem_free((void*)gfx);
		gfx = NULL;
		return gfx;
	}
	logger(LOG_INFO,
	       "\033[7mgfx\033[m Created render target and zbuffer\n");

	gfx_set_render_target(gfx, gfx->render_target, gfx->zstencil_target);
	gfx_set_viewport(gfx, cfg->width, cfg->height);
	gfx_init_sprite(gfx, gfx->sprite_vb);

	return gfx;
}

void gfx_system_shutdown(gfx_system_t* gfx)
{
	if (gfx) {
		gfx_destroy_render_target(gfx);
		gfx_destroy_zstencil(gfx);
		gfx_com_release_d3d11(gfx);
		gfx_destroy_device_dependent_resources(gfx);
		mem_free(gfx);
		gfx = NULL;
	}
}

void gfx_system_bind_render_target(gfx_system_t* gfx)
{
	gfx_set_render_target(gfx, gfx->render_target, gfx->zstencil_target);
}

result gfx_create_swap_chain(gfx_system_t* gfx, const struct gfx_config* cfg)
{
	result res = RESULT_OK;

	if (gfx->device && gfx->dxgi_factory) {
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
		DXGI_SWAP_CHAIN_DESC swap_desc = {
			.BufferDesc.Format =
				gfx_pixel_format_to_dxgi_format(cfg->pix_fmt),
			.BufferDesc.Width = cfg->width,
			.BufferDesc.Height = cfg->height,
			.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
			.BufferDesc.RefreshRate.Denominator =
				(UINT)cfg->fps_den,
			.BufferDesc.RefreshRate.Numerator = (UINT)cfg->fps_num,
			.BufferDesc.ScanlineOrdering =
				DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
			.SampleDesc.Count = 1,
			.SampleDesc.Quality = 0,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = 2,
			.OutputWindow = hwnd,
			.Windowed = (BOOL)!cfg->fullscreen,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.Flags =
				DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
		};

		IDXGISwapChain* dxgi_swap_chain = NULL;
		// if(SUCCEEDED(IDXGIFactory2_CreateSwapChainForHwnd(gfx->dxgi_factory, (IUnknown*)gfx->device, hwnd, &swap_desc1, NULL, NULL, &dxgi_swap_chain)))
		if (SUCCEEDED(IDXGIFactory2_CreateSwapChain(
			    gfx->dxgi_factory, (IUnknown*)gfx->device,
			    &swap_desc, &dxgi_swap_chain))) {
			if (SUCCEEDED(IDXGISwapChain1_QueryInterface(
				    dxgi_swap_chain, &BM_IID_IDXGISwapChain2,
				    (void**)&gfx->dxgi_swap_chain))) {
				if (cfg->fullscreen)
					IDXGIFactory2_MakeWindowAssociation(
						gfx->dxgi_factory, hwnd, 0);
				else
					IDXGIFactory2_MakeWindowAssociation(
						gfx->dxgi_factory, hwnd,
						DXGI_MWA_NO_ALT_ENTER |
							DXGI_MWA_NO_WINDOW_CHANGES);
			} else {
				res = RESULT_NULL;
			}

			IDXGISwapChain1_Release(dxgi_swap_chain);
		} else {
			res = RESULT_NULL;
		}
	}

	if (res == RESULT_NULL)
		logger(LOG_ERROR, "[gfx] Error creating swap chain!");

	return res;
}

result gfx_create_device_dependent_resources(gfx_system_t* gfx, s32 adapter)
{
	result res = RESULT_OK;
	HRESULT hr = S_OK;

	gfx->dxgi_dll = os_dlopen("dxgi.dll");
	if (!gfx->dxgi_dll) {
		res = RESULT_NULL;
		goto cleanup;
	}

	PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2Func =
		os_dlsym(gfx->dxgi_dll, "CreateDXGIFactory2");
	if (!CreateDXGIFactory2Func) {
		res = RESULT_NULL;
		goto cleanup;
	}

	UINT dxgi_flags = 0;
#if defined(BM_DEBUG)
	dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hr = CreateDXGIFactory2Func(dxgi_flags, &BM_IID_IDXGIFactory2,
				    (void**)&gfx->dxgi_factory);
	if (FAILED(hr)) {
		res = RESULT_ERROR;
		goto cleanup;
	}

	u32 adapter_count = 0;
	res = gfx_enumerate_adapters(gfx, adapter, &adapter_count);
	if (res == RESULT_NOT_FOUND)
		goto cleanup;

	gfx->d3d11_dll = os_dlopen("d3d11.dll");
	if (!gfx->d3d11_dll) {
		res = RESULT_NULL;
		goto cleanup;
	}

	PFN_D3D11_CREATE_DEVICE D3D11CreateDeviceFunc =
		os_dlsym(gfx->d3d11_dll, "D3D11CreateDevice");
	if (!D3D11CreateDeviceFunc) {
		res = RESULT_NULL;
		goto cleanup;
	}

	UINT create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(BM_DEBUG)
	create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL feature_levels[] = {
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

	hr = D3D11CreateDeviceFunc((IDXGIAdapter*)gfx->dxgi_adapter,
				   D3D_DRIVER_TYPE_UNKNOWN, NULL, create_flags,
				   feature_levels, num_feature_levels,
				   D3D11_SDK_VERSION,
				   (ID3D11Device**)&gfx->device, &feature_level,
				   (ID3D11DeviceContext**)&gfx->ctx);
	if (FAILED(hr)) {
		res = RESULT_ERROR;
		goto cleanup;
	}

	hr = ID3D11Device_QueryInterface(gfx->device, &BM_IID_ID3D11Device1,
					 (void**)&gfx->device);
	if (FAILED(hr)) {
		res = RESULT_ERROR;
		goto cleanup;
	}

	hr = ID3D11DeviceContext_QueryInterface(
		gfx->ctx, &BM_IID_ID3D11DeviceContext1, (void**)&gfx->ctx);
	if (FAILED(hr)) {
		res = RESULT_ERROR;
		goto cleanup;
	}

	hr = ID3D11Device_QueryInterface(gfx->device, &BM_IID_IDXGIDevice1,
					 (void**)&gfx->dxgi_device);
	if (FAILED(hr)) {
		res = RESULT_ERROR;
		goto cleanup;
	}

cleanup:
	if (res != RESULT_OK) {
		gfx_system_shutdown(gfx);
	}

	return res;
}

void gfx_destroy_device_dependent_resources(gfx_system_t* gfx)
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

void gfx_render_clear(gfx_system_t* gfx, const rgba_t* color)
{
	float clear_color[4];
	clear_color[0] = color->r / 255.f;
	clear_color[1] = color->g / 255.f;
	clear_color[2] = color->b / 255.f;
	clear_color[3] = color->a / 255.f;
	if (gfx && gfx->ctx && gfx->render_target) {
		struct gfx_texture2d* tex =
			(struct gfx_texture2d*)gfx->render_target->impl;
		if (tex && tex->rtv) {
			ID3D11DeviceContext1_ClearRenderTargetView(
				gfx->ctx, tex->rtv, (FLOAT*)clear_color);
		}

		struct gfx_texture2d* zs =
			(struct gfx_texture2d*)gfx->zstencil_target->impl;
		if (zs && zs->dsv) {
			u32 dsv_clear_flags = D3D11_CLEAR_DEPTH |
					      D3D11_CLEAR_STENCIL;
			u8 stencil = 0;
			ID3D11DeviceContext1_ClearDepthStencilView(
				gfx->ctx, zs->dsv, dsv_clear_flags, 1.f,
				stencil);
		}
	}
}

void gfx_render_begin(gfx_system_t* gfx)
{
	gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)gfx->vertex_shader->impl;
	gfx_pixel_shader_t* ps = (gfx_pixel_shader_t*)gfx->pixel_shader->impl;
	u32 vertex_count = BM_GFX_MAX_VERTICES;
	u32 start_vertex = 0;
	ID3D11DeviceContext1_VSSetShader(gfx->ctx, vs->program, NULL, 0);
	ID3D11DeviceContext1_PSSetShader(gfx->ctx, ps->program, NULL, 0);
	ID3D11DeviceContext1_PSSetSamplers(gfx->ctx, 0, 1, &gfx->sampler_state);
	ID3D11DeviceContext1_Draw(gfx->ctx, vertex_count, start_vertex);
}

void gfx_render_end(gfx_system_t* gfx, bool vsync, u32 flags)
{
	IDXGISwapChain1_Present(gfx->dxgi_swap_chain, (UINT)vsync, flags);
}

void gfx_set_vertex_shader(gfx_system_t* gfx, gfx_shader_t* vs)
{
	if (gfx != NULL)
		gfx->vertex_shader = vs;
}

void gfx_set_pixel_shader(gfx_system_t* gfx, gfx_shader_t* ps)
{
	if (gfx != NULL)
		gfx->pixel_shader = ps;
}

enum gfx_vertex_type gfx_get_vertex_type(gfx_system_t* gfx)
{
	if (gfx != NULL) {
		gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)gfx->vertex_shader->impl;
		if (vs != NULL)
			return vs->vertex_type;
	}
	return GFX_VERTEX_UNKNOWN;
}

// void gfx_init_sprite(gfx_system_t* gfx, gfx_buffer_t* vertex_buffer)
// {
// 	size_t sz = sizeof(struct gfx_vertex_data);
// 	struct gfx_vertex_data* vd = (struct gfx_vertex_data*)mem_alloc(sz);
// 	memset(vd, 0, sz);
// 	vd->num_vertices = 4;
// 	size_t sz_positions = sizeof(vec3f_t) * vd->num_vertices;
// 	vd->positions = (vec3f_t*)mem_alloc(sz_positions);
// 	vd->tex_verts = (struct texture_vertex*)mem_alloc(
// 		sizeof(struct texture_vertex));
// 	size_t sz_tex_verts = sizeof(vec2f_t) * 4;
// 	vd->tex_verts->data = mem_alloc(sz_tex_verts);
// 	vd->tex_verts->size = sizeof(vec2f_t);
// 	gfx_buffer_create(gfx, vd, sz_positions + sz_tex_verts,
// 			  GFX_BUFFER_VERTEX, GFX_BUFFER_USAGE_DYNAMIC,
// 			  &vertex_buffer);
// }

// void gfx_draw_sprite(gfx_system_t* gfx, struct gfx_texture* texture, u32 width,
// 		     u32 height, u32 flags)
// {
// }

//
// gfx buffer
//
result gfx_buffer_create(gfx_system_t* gfx, const void* data, size_t size,
			 enum gfx_buffer_type type, enum gfx_buffer_usage usage,
			 gfx_buffer_t** buffer)
{
	if (!gfx || !gfx->device)
		return RESULT_NULL;

	gfx_buffer_t* buf = mem_alloc(sizeof(gfx_buffer_t));
	buf->usage = usage;
	buf->type = type;
	buf->data = (u8*)data;
	buf->size = size;

	u32 cpu_access_flags = 0;
	if (usage == GFX_BUFFER_USAGE_DYNAMIC)
		cpu_access_flags = D3D11_CPU_ACCESS_WRITE;

	u32 bind_flags = 0;
	switch (type) {
	case GFX_BUFFER_VERTEX:
		bind_flags = D3D11_BIND_VERTEX_BUFFER;
		break;
	case GFX_BUFFER_INDEX:
		bind_flags = D3D11_BIND_INDEX_BUFFER;
		break;
	case GFX_BUFFER_CONSTANT:
		bind_flags = D3D11_BIND_CONSTANT_BUFFER;
		break;
	default:
	case GFX_BUFFER_UNKNOWN:
		return RESULT_UNKNOWN;
	}

	D3D11_BUFFER_DESC desc = {
		.Usage = gfx_buffer_usage_to_d3d11_usage(usage),
		.CPUAccessFlags = (UINT)cpu_access_flags,
		.ByteWidth = (UINT)size,
		.BindFlags = (UINT)bind_flags,
		.MiscFlags = 0,
		.StructureByteStride = 0};
	if (data != NULL) {
		D3D11_SUBRESOURCE_DATA srd = {.pSysMem = buf->data,
					      .SysMemPitch = 0,
					      .SysMemSlicePitch = 0};
		if (FAILED(ID3D11Device1_CreateBuffer(gfx->device, &desc, &srd,
						      &buf->buffer)))
			return RESULT_NULL;
	} else {
		if (FAILED(ID3D11Device1_CreateBuffer(gfx->device, &desc, NULL,
						      &buf->buffer)))
			return RESULT_NULL;
	}
	*buffer = buf;
	return RESULT_OK;
}

void gfx_buffer_free(gfx_buffer_t* buffer)
{
	if (buffer != NULL) {
		if (buffer->buffer != NULL) {
			ID3D11Buffer_Release(buffer->buffer);
			buffer->buffer = NULL;
		}
	}
}

result gfx_buffer_copy(gfx_system_t* gfx, gfx_buffer_t* buffer,
		       const void* data, size_t size)
{
	if (!gfx || !buffer || !data || size == 0)
		return RESULT_NULL;

	D3D11_MAPPED_SUBRESOURCE sr = {
		.DepthPitch = 0,
		.pData = NULL,
		.RowPitch = 0,
	};

	if (FAILED(ID3D11DeviceContext1_Map(gfx->ctx,
					    (ID3D11Resource*)buffer->buffer, 0,
					    D3D11_MAP_WRITE_DISCARD, 0, &sr)))
		return RESULT_ERROR;

	memcpy(sr.pData, data, size);

	ID3D11DeviceContext1_Unmap(gfx->ctx, (ID3D11Resource*)buffer->buffer,
				   0);

	return RESULT_OK;
}

void gfx_bind_vertex_buffer(gfx_system_t* gfx, gfx_buffer_t* vb, u32 stride,
			    u32 offset)
{
	if (gfx && gfx->ctx) {
		ID3D11DeviceContext1_IASetVertexBuffers(gfx->ctx, 0, 1,
							&vb->buffer,
							(UINT*)&stride,
							(UINT*)&offset);
	}
}

void gfx_buffer_upload_constants(gfx_system_t* gfx, const gfx_buffer_t* buffer,
				 enum gfx_shader_type type)
{
	if (gfx && buffer) {
		if (type == GFX_SHADER_VERTEX)
			ID3D11DeviceContext1_VSSetConstantBuffers(
				gfx->ctx, 0, 1, &buffer->buffer);
		else if (type == GFX_SHADER_PIXEL)
			ID3D11DeviceContext1_PSSetConstantBuffers(
				gfx->ctx, 0, 1, &buffer->buffer);
	}
}

/*
 * gfx shader
 */
void gfx_shader_init(gfx_shader_t* shader)
{
	if (shader != NULL) {
		shader->type = GFX_SHADER_UNKNOWN;
		shader->impl = NULL;
		shader->cbuffer = NULL;
		vec_init(shader->vars);
	}
}

gfx_shader_t* gfx_shader_create(enum gfx_shader_type type)
{
	gfx_shader_t* shader = (gfx_shader_t*)mem_alloc(sizeof(gfx_shader_t));
	gfx_shader_init(shader);
	shader->type = type;
	switch (shader->type) {
		case GFX_SHADER_VERTEX:
			shader->impl = (gfx_vertex_shader_t*)gfx_vertex_shader_create();
			break;
		case GFX_SHADER_PIXEL:
			shader->impl = (gfx_pixel_shader_t*)gfx_pixel_shader_create();
			break;
		case GFX_SHADER_GEOMETRY:
		case GFX_SHADER_COMPUTE:
		default:
			break;
	}
	return shader;
}

void gfx_shader_free(gfx_shader_t* shader)
{
	if (shader != NULL) {
		gfx_buffer_free(shader->cbuffer);
		switch (shader->type) {
			case GFX_SHADER_VERTEX:
				gfx_vertex_shader_free((gfx_vertex_shader_t*)shader->impl);
				break;
			case GFX_SHADER_PIXEL:
				gfx_pixel_shader_free((gfx_pixel_shader_t*)shader->impl);
				break;
			case GFX_SHADER_GEOMETRY:
			case GFX_SHADER_COMPUTE:
			default:
				logger(LOG_WARNING, "[gfx] shader type %s not implemented!", gfx_shader_type_to_string(shader->type));
				break;
		}
		vec_free(shader->vars);
		mem_free(shader);
		shader = NULL;
	}
}

void gfx_vertex_shader_init(gfx_vertex_shader_t* vs)
{
	if (vs != NULL) {
		vs->blob = NULL;
		vs->input_layout = NULL;
		vs->program = NULL;
		vs->vertex_type = GFX_VERTEX_UNKNOWN;
	}
}
gfx_vertex_shader_t* gfx_vertex_shader_create()
{
	gfx_vertex_shader_t* vs = mem_alloc(sizeof(gfx_vertex_shader_t));
	gfx_vertex_shader_init(vs);
	return vs;
}
void gfx_vertex_shader_free(gfx_vertex_shader_t* vs)
{
	if (vs != NULL) {
		ID3D10Blob_Release(vs->blob);
		vs->blob = NULL;
		ID3D11InputLayout_Release(vs->input_layout);
		vs->input_layout = NULL;
		ID3D11VertexShader_Release(vs->program);
		vs->program = NULL;
		vs->vertex_type = GFX_VERTEX_UNKNOWN;
		mem_free(vs);
		vs = NULL;
	}
}

enum gfx_vertex_type gfx_vertex_shader_get_vertex_type(gfx_vertex_shader_t* vs)
{
	return vs->vertex_type;
}

void gfx_vertex_shader_set_vertex_type(gfx_vertex_shader_t* vs, enum gfx_vertex_type type)
{
	vs->vertex_type = type;
}


void gfx_pixel_shader_init(gfx_pixel_shader_t* ps)
{
	if (ps != NULL) {
		ps->blob = NULL;
		ps->program = NULL;
	}
}
gfx_pixel_shader_t* gfx_pixel_shader_create()
{
	gfx_pixel_shader_t* ps = mem_alloc(sizeof(gfx_pixel_shader_t));
	// gfx_pixel_shader_init(ps);
	return ps;
}
void gfx_pixel_shader_free(gfx_pixel_shader_t* ps)
{
	if (ps != NULL) {
		ID3D10Blob_Release(ps->blob);
		ps->blob = NULL;
		ID3D11PixelShader_Release(ps->program);
		ps->program = NULL;
		mem_free(ps);
		ps = NULL;
	}
}

// original stuff -----------------------------------------------------------------------------------------
result gfx_shader_compile_from_file(gfx_system_t* gfx,
						    const char* path,
						    const char* entrypoint,
						    const char* target,
						    gfx_shader_t* shader)
{
	if (shader == NULL)
		return RESULT_NULL;
	if (!os_path_exists(path))
		return RESULT_NOT_FOUND;
	size_t file_size = os_get_file_size(path);
	if (file_size == 0)
		return RESULT_NO_DATA;
	const char* txt = os_quick_read_utf8_file(path);
	if (txt == NULL)
		return RESULT_NULL;

	ID3D10Blob** shader_blob = NULL;
	switch (shader->type) {
	case GFX_SHADER_VERTEX:
		{
			gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
			if (vs == NULL)
				return RESULT_NULL;
			shader_blob = &vs->blob;
			break;
		}
	case GFX_SHADER_PIXEL:
		{
			gfx_pixel_shader_t* ps = (gfx_pixel_shader_t*)shader->impl;
			if (ps == NULL)
				return RESULT_NULL;
			shader_blob = &ps->blob;
			break;
		}
	default:
		logger(LOG_ERROR, "[gfx] compilation of %s shaders not implemented!", gfx_shader_type_to_string(shader->type));
		return RESULT_NOT_IMPL;
	}

	ID3DBlob* blob_error = NULL;
	if (FAILED(D3DCompile(txt, file_size, path, NULL, NULL, entrypoint,
				target, 0, 0, shader_blob, &blob_error))) {
		if (blob_error) {
			const char* err_msg =
				(const char*)ID3D10Blob_GetBufferPointer(
					blob_error);
			logger(LOG_ERROR,
				"[gfx] Failed to compile shader %s. Error: %s",
				path, err_msg);
			ID3D10Blob_Release(blob_error);
		}
		return RESULT_ERROR;
	}

	return RESULT_OK;
}

result gfx_shader_build_program(gfx_system_t* gfx, gfx_shader_t* shader)
{
	if (gfx == NULL || gfx->device == NULL || shader == NULL)
		return RESULT_NULL;

	switch (shader->type) {
	case GFX_SHADER_VERTEX:
		{
			gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
			if (vs == NULL)
				return RESULT_NULL;
			if (FAILED(ID3D11Device1_CreateVertexShader(
					gfx->device,
					ID3D10Blob_GetBufferPointer(vs->blob),
					ID3D10Blob_GetBufferSize(vs->blob),
					NULL,
					(ID3D11VertexShader**)&vs->program))) {
				logger(LOG_DEBUG,
						"[gfx] Failed to build vertex shader!");
				ID3D11VertexShader_Release(vs->program);
				vs->program = NULL;
			}
			break;
		}
	case GFX_SHADER_PIXEL:
		{
			gfx_pixel_shader_t* ps = (gfx_pixel_shader_t*)shader->impl;
			if (ps == NULL)
				return RESULT_NULL;
			if (FAILED(ID3D11Device1_CreatePixelShader(
					gfx->device,
					ID3D10Blob_GetBufferPointer(ps->blob),
					ID3D10Blob_GetBufferSize(ps->blob),
					NULL,
					(ID3D11PixelShader**)&ps->program))) {
				logger(LOG_DEBUG,
						"[gfx] Failed to build pixel shader!");
				ID3D11PixelShader_Release(ps->program);
				ps->program = NULL;
			}
			break;
		}
	default:
		logger(LOG_ERROR, "[gfx] building of %s shaders not implemented!", gfx_shader_type_to_string(shader->type));
		return RESULT_NOT_IMPL;
	}

	return RESULT_OK;
}

result gfx_shader_create_input_layout(gfx_system_t* gfx, gfx_shader_t* shader)
{
	if (!gfx || !shader)
		return RESULT_NULL;

	gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
	if (!vs)
		return RESULT_NULL;

	const D3D11_INPUT_ELEMENT_DESC* descs = NULL;
	size_t num_elems = 0;
	switch (vs->vertex_type) {
	case GFX_VERTEX_POS_UV:
		descs = &kVertexDescPositionUV[0];
		num_elems = 2;
		break;
	case GFX_VERTEX_POS_NORM_UV:
		descs = &kVertexDescPositionNormalUV[0];
		num_elems = 3;
		break;
	case GFX_VERTEX_POS_COLOR:
		descs = &kVertexDescPositionColor[0];
		num_elems = 2;
		break;
	case GFX_VERTEX_POS_NORM_COLOR:
		descs = &kVertexDescPositionNormalColor[0];
		num_elems = 3;
		break;
	case GFX_VERTEX_UNKNOWN:
		break;
	}

	if (descs && vs->blob) {
		LPVOID data = ID3D10Blob_GetBufferPointer(vs->blob);
		size_t size = ID3D10Blob_GetBufferSize(vs->blob);
		if (FAILED(ID3D11Device1_CreateInputLayout(
			    gfx->device, descs, (UINT)num_elems, data, size,
			    &vs->input_layout))) {
			ID3D11InputLayout_Release(vs->input_layout);
			return RESULT_NULL;
		}
	}

	return RESULT_OK;
}

void gfx_bind_primitive_topology(gfx_system_t* gfx, enum gfx_topology topo)
{
	if (gfx && gfx->ctx) {
		ID3D11DeviceContext1_IASetPrimitiveTopology(
			gfx->ctx, gfx_topology_to_d3d11_topology(topo));
	}
}

void gfx_vertex_shader_bind_input_layout(gfx_system_t* gfx,
					 const gfx_vertex_shader_t* vs)
{
	if (gfx && gfx->ctx && vs) {
		ID3D11DeviceContext1_IASetInputLayout(gfx->ctx,
						      vs->input_layout);
	}
}

bool gfx_shader_add_var(gfx_shader_t* shader, gfx_shader_var_t* var)
{
	if (shader != NULL && var != NULL) {
		if (vec_find(shader->vars, var, 0) == VEC_NOT_FOUND) {
			vec_push_back(shader->vars, var);
			return true;
		}
	}
	return false;
}

bool gfx_shader_set_var_by_name(gfx_shader_t* shader, const char* name,
				const void* value, size_t size)
{
	for (size_t i = 0; i < shader->vars.num_elems; i++) {
		gfx_shader_var_t* var =
			(gfx_shader_var_t*)&shader->vars.elems[i];
		if (var != NULL && !strcmp(var->name, name) &&
		    gfx_get_shader_var_size(var->type) == size) {
			memcpy(var->data, value, size);
			return true;
		}
	}
	return false;
}

gfx_shader_var_t* gfx_shader_get_var_by_name(gfx_shader_t* shader,
					     const char* name)
{
	gfx_shader_var_t* found = NULL;
	for (size_t i = 0; i < shader->vars.num_elems; i++) {
		gfx_shader_var_t* var =
			(gfx_shader_var_t*)&shader->vars.elems[i];
		if (var != NULL && !strcmp(var->name, name)) {
			found = var;
		}
	}
	return found;
}

//
// gfx sampler
//
result gfx_init_sampler_state(gfx_system_t* gfx)
{
	if (!gfx || !gfx->device)
		return RESULT_NULL;

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
		.BorderColor = {0.f, 0.f, 0.f, 0.f},
	};

	if (FAILED(ID3D11Device1_CreateSamplerState(gfx->device, &sd,
						    &gfx->sampler_state))) {
		logger(LOG_DEBUG, "[gfx] Failed to create sampler state!");

		ID3D11SamplerState_Release(gfx->sampler_state);
		gfx->sampler_state = NULL;

		return RESULT_ERROR;
	}

	return RESULT_OK;
}

void gfx_bind_sampler_state(gfx_system_t* gfx, gfx_texture_t* texture, u32 slot)
{
	if (gfx && gfx->ctx && texture) {
		struct gfx_texture2d* impl =
			(struct gfx_texture2d*)(texture->impl);
		if (impl) {
			UINT views = 1;
			ID3D11DeviceContext1_PSSetShaderResources(
				gfx->ctx, slot, views, &impl->srv);
			ID3D11DeviceContext1_PSSetSamplers(gfx->ctx, slot, 1,
							   &gfx->sampler_state);
		}
	}
}

//
// gfx rasterizer
//
result gfx_init_rasterizer(gfx_system_t* gfx, enum gfx_culling_mode culling,
			   enum gfx_raster_flags flags)
{
	bool changed = false;
	if (gfx->raster_opts.culling_mode != culling ||
	    gfx->raster_opts.raster_flags != flags) {
		changed = true;
	}

	if (gfx->raster_state && changed) {
		ID3D11RasterizerState1_Release(gfx->raster_state);
		gfx->raster_state = NULL;
	}

	if (!gfx->raster_state) {
		D3D11_RASTERIZER_DESC desc = {
			.FillMode = (flags & GFX_RASTER_WIREFRAME)
					    ? D3D11_FILL_WIREFRAME
					    : D3D11_FILL_SOLID,
			.CullMode =
				gfx_culling_mode_to_d3d11_cull_mode(culling),
			.FrontCounterClockwise =
				(BOOL)(flags & GFX_RASTER_WINDING_CCW),
			.DepthBias = 0,
			.DepthBiasClamp = 0.f,
			.SlopeScaledDepthBias = 0.f,
			.DepthClipEnable = TRUE,
			.ScissorEnable = (flags & GFX_RASTER_SCISSOR),
			.MultisampleEnable = (flags & GFX_RASTER_MULTI_SAMPLE),
			.AntialiasedLineEnable =
				(flags & GFX_RASTER_ANTIALIAS_LINES),
		};
		if (FAILED(ID3D11Device1_CreateRasterizerState(
			    gfx->device, &desc, &gfx->raster_state))) {
			return RESULT_NULL;
		}
	}

	return RESULT_OK;
}

void gfx_bind_rasterizer(gfx_system_t* gfx)
{
	if (gfx && gfx->device) {
		ID3D11DeviceContext1_RSSetState(gfx->ctx, gfx->raster_state);
	}
}

//
// gfx texture
//
result gfx_create_texture2d(gfx_system_t* gfx, const u8* data,
			    const struct gfx_texture_desc* desc,
			    gfx_texture_t** texture)
{
	if (!texture || !*texture)
		return RESULT_NULL;

	gfx_texture_t* tex = *texture;
	tex->impl = mem_alloc(sizeof(struct gfx_texture2d));
	struct gfx_texture2d* tex2d = (struct gfx_texture2d*)tex->impl;

	DXGI_FORMAT dxgi_format =
		gfx_pixel_format_to_dxgi_format(desc->pix_fmt);

	DXGI_SAMPLE_DESC sample_desc = {
		.Count = 1,
		.Quality = 0,
	};

	u32 bind_flags = D3D11_BIND_SHADER_RESOURCE;
	if (desc->flags & GFX_TEXTURE_IS_RENDER_TARGET)
		bind_flags |= D3D11_BIND_RENDER_TARGET;
	if (desc->flags & GFX_TEXTURE_IS_ZSTENCIL) {
		bind_flags |= D3D11_BIND_DEPTH_STENCIL;
		bind_flags ^= D3D11_BIND_SHADER_RESOURCE;
	}

	u32 misc_flags = 0;
	if (desc->flags & GFX_TEXTURE_SHARED &&
	    !(desc->flags & GFX_TEXTURE_KEYED_MUTEX))
		misc_flags |= D3D11_RESOURCE_MISC_SHARED;
	else if (!(desc->flags & GFX_TEXTURE_SHARED) &&
		 desc->flags & GFX_TEXTURE_KEYED_MUTEX)
		misc_flags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	bool is_dynamic = desc->flags & GFX_TEXTURE_DYNAMIC;

	D3D11_TEXTURE2D_DESC tex_desc = {
		.Width = desc->width,
		.Height = desc->height,
		.MipLevels = desc->flags & GFX_TEXTURE_GENERATE_MIPMAPS
				     ? 0UL
				     : desc->mip_levels,
		.ArraySize = 1UL,
		.Format = dxgi_format,
		.SampleDesc = sample_desc,
		.Usage = is_dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT,
		.BindFlags = bind_flags,
		.CPUAccessFlags = is_dynamic ? D3D11_CPU_ACCESS_WRITE : 0UL,
		.MiscFlags = misc_flags,
	};

	// cache texture data
	u32 stride = desc->width * (gfx_get_bits_per_pixel(desc->pix_fmt) / 8);
	tex->size = desc->height * stride;
	if (data) {
		tex->data = (u8*)mem_alloc(tex->size);
		memcpy(tex->data, data, tex->size);

		D3D11_SUBRESOURCE_DATA srd = {
			.pSysMem = (void*)tex->data,
			.SysMemPitch = stride,
			.SysMemSlicePitch = (UINT)tex->size,
		};

		if (FAILED(ID3D11Device1_CreateTexture2D(
			    gfx->device, &tex_desc, &srd, &tex2d->texture)))
			return RESULT_NULL;
	} else {
		if (FAILED(ID3D11Device1_CreateTexture2D(
			    gfx->device, &tex_desc, NULL, &tex2d->texture)))
			return RESULT_NULL;
	}

	// z-buffer
	if (desc->flags & GFX_TEXTURE_IS_ZSTENCIL) {
		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {
			.Format = dxgi_format,
			.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
			.Texture2D.MipSlice = 0,
		};

		if (FAILED(ID3D11Device_CreateDepthStencilView(
			    gfx->device, (ID3D11Resource*)tex2d->texture,
			    &dsv_desc, &tex2d->dsv)))
			return RESULT_NULL;
		// shader texture
	} else {
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {
			.Format = dxgi_format,
			.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
			.Texture2D.MipLevels = desc->mip_levels,
			.Texture2D.MostDetailedMip = desc->mip_levels - 1,
		};

		if (FAILED(ID3D11Device1_CreateShaderResourceView(
			    gfx->device, (ID3D11Resource*)tex2d->texture,
			    &srv_desc, &tex2d->srv)))
			return RESULT_NULL;
	}

	return RESULT_OK;
}

result gfx_create_texture(gfx_system_t* gfx, const u8* data,
			  const struct gfx_texture_desc* desc,
			  gfx_texture_t** texture)
{
	result res = RESULT_OK;

	gfx_texture_t* tex = (gfx_texture_t*)mem_alloc(sizeof(*tex));

	switch (desc->type) {
	case GFX_TEXTURE_2D:
		res = gfx_create_texture2d(gfx, data, desc, &tex);
		break;
	case GFX_TEXTURE_1D:
	case GFX_TEXTURE_3D:
	case GFX_TEXTURE_CUBE:
	default:
		res = RESULT_NOT_IMPL;
		break;
	}

	if (res == RESULT_OK) {
		*texture = tex;
	} else {
		mem_free(tex);
		tex = NULL;
	}

	return res;
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_init_render_target(gfx_system_t* gfx, u32 width, u32 height,
			      enum gfx_pixel_format pf)
{
	if (!gfx || !gfx->device || !gfx->dxgi_swap_chain)
		return RESULT_NULL;

	gfx->render_target =
		(gfx_texture_t*)mem_alloc(sizeof(*gfx->render_target));

	struct gfx_texture_desc desc = {
		.type = GFX_TEXTURE_2D,
		.pix_fmt = pf,
		.width = width,
		.height = height,
		.mip_levels = 1,
		.flags = GFX_TEXTURE_IS_RENDER_TARGET,
	};

	if (gfx_create_texture2d(gfx, NULL, &desc, &gfx->render_target) ==
	    RESULT_OK) {
		struct gfx_texture2d* tex =
			(struct gfx_texture2d*)gfx->render_target->impl;
		HRESULT hr = IDXGISwapChain2_GetBuffer(gfx->dxgi_swap_chain, 0,
						       &BM_IID_ID3D11Texture2D,
						       (void**)&tex->texture);
		if (FAILED(hr))
			return RESULT_NULL;

		// D3D11_RENDER_TARGET_VIEW_DESC rtv_desc; // TODO?
		hr = ID3D11Device1_CreateRenderTargetView(
			gfx->device, (ID3D11Resource*)tex->texture, NULL,
			&tex->rtv);
		if (FAILED(hr))
			return RESULT_NULL;
	}

	return RESULT_OK;
}

void gfx_destroy_render_target(gfx_system_t* gfx)
{
	if (gfx && gfx->render_target) {
		if (gfx->render_target->impl) {
			mem_free(gfx->render_target->impl);
			gfx->render_target->impl = NULL;
		}
		mem_free(gfx->render_target);
		gfx->render_target = NULL;
	}
}

void gfx_set_render_target(gfx_system_t* gfx, gfx_texture_t* texture,
			   gfx_texture_t* zstencil)
{
	if (gfx && gfx->ctx) {
		ID3D11RenderTargetView* rtv = NULL;
		ID3D11DepthStencilView* dsv = NULL;
		if (texture && texture->impl) {
			struct gfx_texture2d* tex =
				(struct gfx_texture2d*)texture->impl;
			rtv = tex->rtv;
		}
		if (zstencil && zstencil->impl) {
			struct gfx_texture2d* tex =
				(struct gfx_texture2d*)zstencil->impl;
			dsv = tex->dsv;
		}
		ID3D11DeviceContext1_OMSetRenderTargets(gfx->ctx, 1, &rtv, dsv);
	}
}

result gfx_create_zstencil_state(gfx_system_t* gfx, bool enable,
				 struct gfx_zstencil_state** state)
{
	if (!gfx || !gfx->device || !state)
		return RESULT_NULL;

	struct gfx_zstencil_state* zss = *state;
	if (!zss)
		return RESULT_NULL;

	zss->enabled = enable;

	// TODO(paulh): Port the other aspects of this config to gfx_zstencil_state
	D3D11_DEPTH_STENCIL_DESC desc = {
		.DepthEnable = enable,
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS,
		.StencilEnable = enable,
		.StencilReadMask = 0xFF,
		.StencilWriteMask = 0xFF,
		.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
		.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
		.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
		.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
		.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP,
		.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
		.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP,
		.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS,
	};

	if (FAILED(ID3D11Device1_CreateDepthStencilState(gfx->device, &desc,
							 &zss->dss)))
		return RESULT_NULL;

	return RESULT_OK;
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_init_zstencil(gfx_system_t* gfx, u32 width, u32 height,
			 enum gfx_pixel_format pix_fmt, bool enabled)
{
	if (!gfx || !gfx->device)
		return RESULT_NULL;

	gfx->zstencil_target =
		(gfx_texture_t*)mem_alloc(sizeof(*gfx->zstencil_target));

	struct gfx_texture_desc desc = {
		.type = GFX_TEXTURE_2D,
		.pix_fmt = pix_fmt,
		.width = width,
		.height = height,
		.mip_levels = 1,
		.flags = GFX_TEXTURE_IS_ZSTENCIL,
	};
	gfx_create_texture2d(gfx, NULL, &desc, &gfx->zstencil_target);

	// TODO(paulh): Add more zstencil state options eventually
	gfx->zstencil_state_enabled =
		mem_alloc(sizeof(*gfx->zstencil_state_enabled));
	gfx->zstencil_state_disabled =
		mem_alloc(sizeof(*gfx->zstencil_state_disabled));
	gfx_create_zstencil_state(gfx, false, &gfx->zstencil_state_disabled);
	gfx_create_zstencil_state(gfx, true, &gfx->zstencil_state_enabled);
	gfx_toggle_zstencil(gfx, enabled);

	return RESULT_OK;
}

void gfx_destroy_zstencil(gfx_system_t* gfx)
{
	if (gfx && gfx->zstencil_state_enabled &&
	    gfx->zstencil_state_disabled && gfx->zstencil_target) {
		mem_free(gfx->zstencil_state_enabled);
		gfx->zstencil_state_enabled = NULL;
		mem_free(gfx->zstencil_state_disabled);
		gfx->zstencil_state_disabled = NULL;

		mem_free(gfx->zstencil_target->impl);
		mem_free(gfx->zstencil_target);
		gfx->zstencil_target->impl = NULL;
		gfx->zstencil_target = NULL;
	}
}

void gfx_bind_zstencil_state(gfx_system_t* gfx,
			     const struct gfx_zstencil_state* state)
{
	if (gfx && state) {
		ID3D11DeviceContext1_OMSetDepthStencilState(gfx->ctx,
							    state->dss, 1);
	}
}

void gfx_toggle_zstencil(gfx_system_t* gfx, bool enabled)
{
	if (gfx) {
		if (enabled)
			gfx_bind_zstencil_state(gfx,
						gfx->zstencil_state_enabled);
		else
			gfx_bind_zstencil_state(gfx,
						gfx->zstencil_state_disabled);
	}
}

u32 gfx_texture_get_width(gfx_texture_t* texture)
{
	if (texture && texture->type == GFX_TEXTURE_2D) {
		struct gfx_texture2d* impl =
			(struct gfx_texture2d*)texture->impl;
		if (impl)
			return impl->width;
	}
	return 0;
}

u32 gfx_texture_get_height(gfx_texture_t* texture)
{
	if (texture && texture->type == GFX_TEXTURE_2D) {
		struct gfx_texture2d* impl =
			(struct gfx_texture2d*)texture->impl;
		if (impl)
			return impl->height;
	}
	return 0;
}

void gfx_texture_get_size(gfx_texture_t* texture, vec2f_t* size)
{
	if (texture && texture->type == GFX_TEXTURE_2D) {
		struct gfx_texture2d* impl =
			(struct gfx_texture2d*)texture->impl;
		if (impl) {
			size->x = (f32)impl->width;
			size->y = (f32)impl->height;
		}
	}
}
