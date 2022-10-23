#include "core/logger.h"
#include "core/memory.h"
#include "core/utils.h"
#include "core/vector.h"

#include "math/types.h"

#include "platform/platform.h"

#include "gfx/gfx.h"
#include "gfx/gfx_d3d11.h"
#include "gfx/dxguids.h"

#define COBJMACROS
#define CINTERFACE
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgiformat.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include "gfx/gfx_d3d11_input_layout.h"

#define COM_RELEASE(obj)                          \
	if ((obj)) {                              \
		IUnknown_Release((IUnknown*)obj); \
		obj = NULL;                       \
	}

typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY2)(UINT flags, REFIID riid,
						  void** ppFactory);
static PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2Func = NULL;
static PFN_D3D11_CREATE_DEVICE D3D11CreateDeviceFunc = NULL;

struct gfx_display {
	char name[128];
	char friendly_name[128];
	u32 index;
	u32 refresh_rate;
	u32 width;
	u32 height;
	bool has_desktop;
	enum gfx_display_orientation orientation;
	rect_t desktop_coords;
	IDXGIOutput* dxgi_output;
};

struct gfx_adapter {
	char name[128];
	char description[128];
	u32 index;
	u32 vendor_id;
	u32 device_id;
	u32 subsystem_id;
	u32 revision;
	u64 driver_version;
	size_t vram;
	size_t sys_mem;
	size_t sys_mem_shared;
	IDXGIAdapter1* dxgi_adapter;
	VECTOR(struct gfx_display) displays;
};

struct gfx_buffer {
	u8* data;
	size_t size;
	ID3D11Buffer* buffer;
	enum gfx_buffer_type type;
	enum gfx_buffer_usage usage;
	bool own_data;
};

struct gfx_vertex_shader {
	enum gfx_vertex_type vertex_type;
	ID3DBlob* blob;
	ID3D11VertexShader* program;
	ID3D11InputLayout* input_layout;
	s32 ref_count;
};

struct gfx_pixel_shader {
	ID3DBlob* blob;
	ID3D11PixelShader* program;
	s32 ref_count;
};

struct gfx_texture2d {
	enum pixel_format pix_fmt;
	u32 width;
	u32 height;
	u32 mip_levels;
	u32 flags;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtv;
	ID3D11DepthStencilView* dsv;
};

struct gfx_depth_state {
	bool enabled;
	ID3D11DepthStencilState* dss;
};

struct gfx_module {
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
	gfx_texture_t* depth_target;
	struct gfx_depth_state* depth_state_enabled;
	struct gfx_depth_state* depth_state_disabled;

	/* sampler */
	ID3D11SamplerState* sampler_state;

	/* rasterizer */
	struct gfx_raster_state_desc raster_desc;
	ID3D11RasterizerState* raster_state;

	gfx_shader_t* vertex_shader;
	gfx_shader_t* pixel_shader;
	gfx_buffer_t* sprite_vb;
	gfx_buffer_t* cube_vb;

	DXGI_SWAP_EFFECT swap_effect;

	rect_t viewport;
};

enum gfx_display_orientation
gfx_dxgi_rotation_to_orientation(DXGI_MODE_ROTATION rotation)
{
	switch (rotation) {
	case DXGI_MODE_ROTATION_ROTATE90:
		return GFX_DISPLAY_ORIENTATION_90;
	case DXGI_MODE_ROTATION_ROTATE180:
		return GFX_DISPLAY_ORIENTATION_180;
	case DXGI_MODE_ROTATION_ROTATE270:
		return GFX_DISPLAY_ORIENTATION_270;
	default:
	case DXGI_MODE_ROTATION_UNSPECIFIED:
	case DXGI_MODE_ROTATION_IDENTITY:
		return GFX_DISPLAY_ORIENTATION_0;
	}
	return GFX_DISPLAY_ORIENTATION_0;
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

DXGI_FORMAT pixel_format_to_dxgi_format(enum pixel_format pf)
{
	switch (pf) {
	case PIX_FMT_BGRA32:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case PIX_FMT_RGBA32:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case PIX_FMT_NV12:
		return DXGI_FORMAT_NV12;
	case PIX_FMT_DEPTH_U16:
		return DXGI_FORMAT_D16_UNORM;
	case PIX_FMT_DEPTH_U24_S8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case PIX_FMT_DEPTH_F32:
		return DXGI_FORMAT_D32_FLOAT;
	default:
	case PIX_FMT_ARGB32:
	case PIX_FMT_RGB24:
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

void gfx_activate_d3d11_debug_info(void)
{
	if (gfx && gfx->module) {
		ID3D11InfoQueue* nfo_q;
		if (SUCCEEDED(IProvideClassInfo_QueryInterface(
			    gfx->module->device, &BM_IID_ID3D11InfoQueue,
			    (void**)&nfo_q))) {
			ID3D11InfoQueue_SetBreakOnSeverity(
				nfo_q, D3D11_MESSAGE_SEVERITY_INFO, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(
				nfo_q, D3D11_MESSAGE_SEVERITY_MESSAGE, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(
				nfo_q, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(
				nfo_q, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			ID3D11InfoQueue_SetBreakOnSeverity(
				nfo_q, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
			ID3D11InfoQueue_Release(nfo_q);
		}
	}
}

result gfx_com_release_d3d11(void)
{
	COM_RELEASE(gfx->module->sampler_state);
	COM_RELEASE(gfx->module->raster_state);
	COM_RELEASE(gfx->module->dxgi_swap_chain);
	COM_RELEASE(gfx->module->dxgi_device);
	COM_RELEASE(gfx->module->ctx);
	COM_RELEASE(gfx->module->device);
	COM_RELEASE(gfx->module->dxgi_adapter);
	COM_RELEASE(gfx->module->dxgi_factory);
	// COM_RELEASE(gfx->module->render_target);
	// COM_RELEASE(gfx->module->rtv);
	// COM_RELEASE(gfx->module->depth_target);
	// COM_RELEASE(gfx->module->dsv);
	// COM_RELEASE(gfx->module->dss);
	return RESULT_OK;
}

//
// gfx system
//
void unpack_driver_version(u64 ver, u16* aa, u16* bb, u16* ccccc, u64* ddddd)
{
	*aa = (ver >> 48) & 0xffff;
	*bb = (ver >> 32) & 0xffff;
	*ccccc = (ver >> 16) & 0xffff;
	*ddddd = ver & 0xffff;
}

result gfx_enumerate_adapters(struct vector* adapters, bool enum_displays)
{
	if (gfx == NULL || gfx->module == NULL ||
	    gfx->module->dxgi_factory == NULL) {
		return RESULT_NULL;
	}

	HRESULT hr = S_OK;
	for (UINT i = 0; hr == S_OK; i++) {
		struct gfx_adapter ga;
		memset(&ga, 0, sizeof(struct gfx_adapter));

		hr = IDXGIFactory2_EnumAdapters1(gfx->module->dxgi_factory, i,
						 &ga.dxgi_adapter);
		if (FAILED(hr))
			continue;

		DXGI_ADAPTER_DESC desc;
		IDXGIAdapter1_GetDesc(ga.dxgi_adapter, &desc);
		// ignore Microsoft's 'basic' renderer
		if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
			continue;

		os_wcs_to_utf8(desc.Description, 0, ga.description,
			       sizeof(ga.description));
		logger(LOG_INFO, "\033[7mgfx\033[m\tDXGI Adapter %u: %s", i,
		       (const char*)&ga.description[0]);
		logger(LOG_INFO, "     \tDedicated VRAM: %u",
		       desc.DedicatedVideoMemory);
		logger(LOG_INFO, "     \tShared VRAM:    %u",
		       desc.SharedSystemMemory);
		logger(LOG_INFO, "     \tPCI ID:         %x:%x", desc.VendorId,
		       desc.DeviceId);
		logger(LOG_INFO, "     \tRevision:       %u", desc.Revision);

		LARGE_INTEGER umd;
		if (SUCCEEDED(IDXGIAdapter1_CheckInterfaceSupport(
			    ga.dxgi_adapter, &BM_IID_IDXGIDevice, &umd))) {
			u16 aa = 0;
			u16 bb = 0;
			u16 ccccc = 0;
			u64 ddddd = 0;
			unpack_driver_version((u64)umd.QuadPart, &aa, &bb,
					      &ccccc, &ddddd);
			logger(LOG_INFO, "     \tDriver Version: %u.%u.%u.%u",
			       aa, bb, ccccc, ddddd);
			ga.driver_version = umd.QuadPart;
		} else {
			logger(LOG_WARNING, "     \tDriver Version: Unknown");
		}

		ga.index = i;
		ga.vram = desc.DedicatedVideoMemory;
		ga.sys_mem = desc.DedicatedSystemMemory;
		ga.sys_mem_shared = desc.SharedSystemMemory;
		ga.vendor_id = desc.VendorId;
		ga.device_id = desc.DeviceId;
		ga.subsystem_id = desc.SubSysId;
		ga.revision = desc.Revision;

		logger(LOG_INFO, "[gfx] Monitors");
		vec_init(ga.displays);
		gfx_enumerate_displays(&ga, &ga.displays.vec);

		vector_push_back(adapters, &ga, sizeof(struct gfx_adapter));
	}

	return RESULT_OK;
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
			(DISPLAYCONFIG_PATH_INFO*)BM_ALLOC(
				num_paths * (sizeof(DISPLAYCONFIG_PATH_INFO)));
		DISPLAYCONFIG_MODE_INFO* modes =
			(DISPLAYCONFIG_MODE_INFO*)BM_ALLOC(
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
		BM_FREE(paths);
		BM_FREE(modes);
		paths = NULL;
		modes = NULL;
	}
	return found;
}

result gfx_enumerate_displays(const gfx_adapter_t* adapter,
			      struct vector* displays)
{
	if (adapter->dxgi_adapter == NULL)
		return RESULT_NULL;

	struct gfx_display gd;
	UINT output_index = 0;
	if (FAILED(IDXGIAdapter1_EnumOutputs(adapter->dxgi_adapter,
					     output_index, &gd.dxgi_output)))
		return RESULT_NOT_FOUND;

	for (output_index = 0;
	     IDXGIAdapter1_EnumOutputs(adapter->dxgi_adapter, output_index,
				       &gd.dxgi_output) == S_OK;
	     output_index++) {
		DXGI_OUTPUT_DESC desc;
		if (FAILED(IDXGIOutput_GetDesc(gd.dxgi_output, &desc)))
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
			os_wcs_to_utf8(desc.DeviceName, 0, gd.name,
				       sizeof desc.DeviceName);
			os_wcs_to_utf8(target.monitorFriendlyDeviceName, 0,
				       gd.friendly_name,
				       sizeof target.monitorFriendlyDeviceName);
			logger(LOG_INFO, "     \tOutput %u:     %s",
			       output_index, gd.name);
			logger(LOG_INFO, "     \tFriendly Name: %s",
			       gd.friendly_name);
			logger(LOG_INFO, "     \tPosition:      %d, %d",
			       rect.left, rect.top);
			logger(LOG_INFO, "     \tSize:          %d, %d",
			       rect.right - rect.left, rect.bottom - rect.top);
			logger(LOG_INFO, "     \tAttached:      %s",
			       desc.AttachedToDesktop ? "true" : "false");
			logger(LOG_INFO, "     \tRefresh Rate:  %uhz",
			       refresh_rate);
			gd.desktop_coords.x = rect.left;
			gd.desktop_coords.y = rect.top;
			gd.desktop_coords.w = rect.right;
			gd.desktop_coords.h = rect.bottom;
			gd.width = rect.right - rect.left;
			gd.height = rect.bottom - rect.top;
			gd.orientation =
				gfx_dxgi_rotation_to_orientation(desc.Rotation);
			gd.has_desktop = desc.AttachedToDesktop;
			gd.refresh_rate = refresh_rate;
			DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY output_tect =
				target.outputTechnology;
			vector_push_back(displays, &gd,
					 sizeof(struct gfx_display));
		}
	}
	return RESULT_OK;
}

void gfx_set_viewport(u32 width, u32 height)
{
	D3D11_VIEWPORT vp = {
		.Width = (FLOAT)width,
		.Height = (FLOAT)height,
		.MaxDepth = 1.f,
		.MinDepth = 0.f,
		.TopLeftX = 0.f,
		.TopLeftY = 0.f,
	};
	ID3D11DeviceContext1_RSSetViewports(gfx->module->ctx, 1, &vp);
	logger(LOG_INFO,
	       "\033[7mgfx\033[m Set D3D11 Viewport:\n"
	       "Width: %f, Height: %f\n"
	       "Min Depth: %f, Max Depth: %f\n"
	       "Top Left X: %f, Top Left Y: %f",
	       vp.Width, vp.Height, vp.MinDepth, vp.MaxDepth, vp.TopLeftX,
	       vp.TopLeftY);
}

// TODO(paulh): Refactor to initialize D3D11 once, and allow instantiating multple swap chains
result gfx_init_dx11(const struct gfx_config* cfg, s32 flags)
{
	gfx_hardware_ready = false;

	if (gfx == NULL)
		return RESULT_NULL;

	if (gfx->module != NULL) {
		BM_FREE(gfx->module);
		gfx->module = NULL;
	}
	gfx->module = BM_ALLOC(sizeof(*gfx->module));
	memset(gfx->module, 0, sizeof(*gfx->module));

	if (gfx_create_device(cfg->adapter) != RESULT_OK) {
		gfx_shutdown();
		return RESULT_ERROR;
	}
	gfx_activate_d3d11_debug_info();

	if (gfx_create_swap_chain(cfg) != RESULT_OK) {
		gfx_com_release_d3d11();
		return RESULT_ERROR;
	}

	gfx_hardware_ready = true;

	return RESULT_OK;
}

void gfx_shutdown_dx11(void)
{
	for (size_t i = 0; i < gfx->adapters.num_elems; i++) {
		gfx_adapter_t* adap = vec_elem(gfx->adapters, i);
		if (adap) {
			for (size_t j = 0; j < adap->displays.num_elems; j++) {
				gfx_display_t* disp =
					vec_elem(adap->displays, j);
				if (disp) {
					IDXGIOutput_Release(disp->dxgi_output);
				}
			}
		}
		vec_free(adap->displays);
		IDXGIAdapter1_Release(adap->dxgi_adapter);
	}
	vec_free(gfx->adapters);

	if (gfx->module != NULL) {
		gfx_com_release_d3d11();
		BM_FREE(gfx->module);
		gfx->module = NULL;
	}
}

result gfx_init_renderer(const struct gfx_config* cfg, s32 flags)
{
	if (!gfx_hardware_ok())
		return RESULT_ERROR;

	result res = RESULT_OK;
	ENSURE_OK(
		gfx_render_target_init(cfg->width, cfg->height, cfg->pix_fmt));
	ENSURE_OK(gfx_init_depth(cfg->width, cfg->height, PIX_FMT_DEPTH_U24_S8,
				 flags & GFX_USE_ZBUFFER));

	logger(LOG_INFO,
	       "\033[7mgfx\033[m Created render target and zbuffer\n");

	gfx_set_render_targets(gfx->module->render_target,
			       gfx->module->depth_target);
	gfx_set_viewport(cfg->width, cfg->height);
	// gfx_init_sprite(gfx->module->sprite_vb);

	gfx_system_ready = true;

	return RESULT_OK;
}

gfx_shader_t* gfx_system_get_vertex_shader()
{
	if (gfx_ok())
		return gfx->module->vertex_shader;
	return NULL;
}

gfx_shader_t* gfx_system_get_pixel_shader()
{
	if (gfx_ok())
		return gfx->module->pixel_shader;
	return NULL;
}

void gfx_system_bind_render_target(void)
{
	gfx_set_render_targets(gfx->module->render_target,
			       gfx->module->depth_target);
}

void gfx_system_bind_input_layout(gfx_shader_t* shader)
{
	if (gfx_ok() && shader != NULL) {
		gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
		if (vs != NULL)
			gfx_vertex_shader_bind_input_layout(vs);
	}
}

result gfx_create_swap_chain(const struct gfx_config* cfg)
{
	if (gfx == NULL || gfx->module == NULL ||
	    gfx->module->dxgi_factory == NULL || cfg == NULL ||
	    cfg->window.hwnd == NULL) {
		return RESULT_NULL;
	}

	result res = RESULT_OK;
	HRESULT hr = S_OK;
	HWND hwnd = (HWND)cfg->window.hwnd;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsd = {
		.Scaling = DXGI_SCALING_NONE,
		.RefreshRate.Denominator = (UINT)cfg->fps_den,
		.RefreshRate.Numerator = (UINT)cfg->fps_num,
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
		.Windowed = (BOOL)!cfg->fullscreen,
	};
	DXGI_SWAP_CHAIN_DESC1 swap_desc1 = {
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.BufferCount = 2,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.Flags = 0,
		.Format = pixel_format_to_dxgi_format(cfg->pix_fmt),
		.Width = cfg->width,
		.Height = cfg->height,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.Scaling = DXGI_SCALING_NONE,
		.SampleDesc = {1, 0},
	};
	// DXGI_SWAP_CHAIN_DESC swap_desc = {
	// 	.BufferDesc.Format = pixel_format_to_dxgi_format(cfg->pix_fmt),
	// 	.BufferDesc.Width = cfg->width,
	// 	.BufferDesc.Height = cfg->height,
	// 	.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
	// 	.BufferDesc.RefreshRate.Denominator = (UINT)cfg->fps_den,
	// 	.BufferDesc.RefreshRate.Numerator = (UINT)cfg->fps_num,
	// 	.BufferDesc.ScanlineOrdering =
	// 		DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE,
	// 	.SampleDesc.Count = 1,
	// 	.SampleDesc.Quality = 0,
	// 	.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	// 	.BufferCount = 2,
	// 	.OutputWindow = hwnd,
	// 	.Windowed = (BOOL)!cfg->fullscreen,
	// 	.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
	// 	.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
	// };
	IDXGISwapChain1* sc1;
	hr = IDXGIFactory2_CreateSwapChainForHwnd(
		gfx->module->dxgi_factory, (IUnknown*)gfx->module->device, hwnd,
		&swap_desc1, &fsd, NULL, &sc1);
	if (FAILED(hr)) {
		logger(LOG_ERROR, "Error creating Swap Chain!");
		goto cleanup;
	}

	// IDXGISwapChain* sc = NULL;
	// hr = IDXGIFactory2_CreateSwapChain(gfx->module->dxgi_factory,
	// 				   (IUnknown*)gfx->module->device,
	// 				   &swap_desc, &sc);
	// if (FAILED(hr)) {
	// 	logger(LOG_ERROR,
	// 	       "\033[7mgfx\033[m IDXGIFactory2 CreateSwapChain failed!");
	// 	goto cleanup;
	// }
	hr = IDXGISwapChain1_QueryInterface(
		sc1, &BM_IID_IDXGISwapChain2,
		(void**)&gfx->module->dxgi_swap_chain);
	if (FAILED(hr)) {
		logger(LOG_ERROR,
		       "\033[7mgfx\033[m IDXGISwapChain1 QI failed!");
		goto cleanup;
	}
	if (cfg->fullscreen) {
		hr = IDXGIFactory2_MakeWindowAssociation(
			gfx->module->dxgi_factory, hwnd, 0);
		if (FAILED(hr)) {
			logger(LOG_ERROR,
			       "\033[7mgfx\033[m IDXGIFactory2 MakeWindowAssociation (fullscreen) failed!");
			goto cleanup;
		}
	} else {
		hr = IDXGIFactory2_MakeWindowAssociation(
			gfx->module->dxgi_factory, hwnd,
			DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
		if (FAILED(hr)) {
			logger(LOG_ERROR,
			       "\033[7mgfx\033[m IDXGIFactory2 MakeWindowAssociation (windowed) failed!");
			goto cleanup;
		}
	}

cleanup:
	if (hr != S_OK) {
		// if (sc != NULL) {
		// 	IDXGISwapChain1_Release(sc);
		// 	sc = NULL;
		// }
		if (gfx->module->dxgi_swap_chain != NULL) {
			IDXGISwapChain1_Release(gfx->module->dxgi_swap_chain);
			gfx->module->dxgi_swap_chain = NULL;
		}
		res = RESULT_ERROR;
	}

	if (res == RESULT_OK)
		logger(LOG_INFO, "\033[7mgfx\033[m Created D3D11 Swap Chain\n");

	return res;
}

result gfx_load_dx11_dlls()
{
	if (gfx != NULL && gfx->module != NULL) {
		gfx->module->dxgi_dll = os_dlopen("dxgi.dll");
		if (!gfx->module->dxgi_dll) {
			return RESULT_NULL;
		}
		logger(LOG_INFO, "\033[7mgfx\033[m Loaded dxgi.dll");

		gfx->module->d3d11_dll = os_dlopen("d3d11.dll");
		if (!gfx->module->d3d11_dll) {
			return RESULT_NULL;
		}
		logger(LOG_INFO, "\033[7mgfx\033[m Loaded d3d11.dll");
	}
	return RESULT_OK;
}

void gfx_unload_dx11_dlls()
{
	if (gfx != NULL) {
		if (gfx->module->d3d11_dll != NULL) {
			os_dlclose(gfx->module->d3d11_dll);
			gfx->module->d3d11_dll = NULL;
		}
		if (gfx->module->dxgi_dll != NULL) {
			os_dlclose(gfx->module->dxgi_dll);
			gfx->module->dxgi_dll = NULL;
		}
	}
}

result gfx_load_dx11_functions()
{
	if (gfx->module->dxgi_dll) {
		CreateDXGIFactory2Func =
			os_dlsym(gfx->module->dxgi_dll, "CreateDXGIFactory2");
		if (!CreateDXGIFactory2Func) {
			return RESULT_NULL;
		}
	}
	if (gfx->module->d3d11_dll) {
		D3D11CreateDeviceFunc =
			os_dlsym(gfx->module->d3d11_dll, "D3D11CreateDevice");
		if (!D3D11CreateDeviceFunc) {
			return RESULT_NULL;
		}
	}
	return RESULT_OK;
}

result gfx_create_device(s32 adapter)
{
	result res = RESULT_OK;
	HRESULT hr = S_OK;

	ENSURE_OK(gfx_load_dx11_dlls());
	ENSURE_OK(gfx_load_dx11_functions());

	UINT dxgi_flags = 0;
#if defined(BM_DEBUG)
	dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hr = CreateDXGIFactory2Func(dxgi_flags, &BM_IID_IDXGIFactory2,
				    (void**)&gfx->module->dxgi_factory);
	if (FAILED(hr)) {
		return RESULT_ERROR;
	}

	vec_init(gfx->adapters);
	ENSURE_OK(gfx_enumerate_adapters(&gfx->adapters.vec, true));

	gfx_adapter_t* adap = vec_elem(gfx->adapters, adapter);
	if (adap != NULL) {
		gfx->module->dxgi_adapter = adap->dxgi_adapter;
	}

	UINT create_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(BM_DEBUG)
	create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL feature_levels[] = {
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1};

	UINT num_feature_levels = ARRAY_SIZE(feature_levels);

	D3D_FEATURE_LEVEL feature_level;
	hr = D3D11CreateDeviceFunc((IDXGIAdapter*)gfx->module->dxgi_adapter,
				   D3D_DRIVER_TYPE_UNKNOWN, NULL, create_flags,
				   feature_levels, num_feature_levels,
				   D3D11_SDK_VERSION,
				   (ID3D11Device**)&gfx->module->device,
				   &feature_level,
				   (ID3D11DeviceContext**)&gfx->module->ctx);
	if (FAILED(hr)) {
		logger(LOG_ERROR, "\033[7mgfx\033[m D3D11CreateDevice failed!");
		return RESULT_ERROR;
	}

	hr = ID3D11Device_QueryInterface(gfx->module->device,
					 &BM_IID_ID3D11Device1,
					 (void**)&gfx->module->device);
	if (FAILED(hr)) {
		logger(LOG_ERROR, "\033[7mgfx\033[m ID3D11Device1 QI failed!");
		return RESULT_ERROR;
	}

	logger(LOG_INFO, "\033[7mgfx\033[m Created D3D11 Device");

	hr = ID3D11DeviceContext_QueryInterface(gfx->module->ctx,
						&BM_IID_ID3D11DeviceContext1,
						(void**)&gfx->module->ctx);
	if (FAILED(hr)) {
		logger(LOG_ERROR,
		       "\033[7mgfx\033[m ID3D11DeviceContext1 QI failed!");
		return RESULT_ERROR;
	}

	logger(LOG_INFO, "\033[7mgfx\033[m Created D3D11 Device Context");

	hr = ID3D11Device_QueryInterface(gfx->module->device,
					 &BM_IID_IDXGIDevice1,
					 (void**)&gfx->module->dxgi_device);
	if (FAILED(hr)) {
		logger(LOG_ERROR, "\033[7mgfx\033[m IDXGIDevice1 QI failed!");
		return RESULT_ERROR;
	}

	logger(LOG_INFO, "\033[7mgfx\033[m Created DXGI Device");

	return res;
}

void gfx_destroy_device(void)
{
	if (gfx) {
		gfx_unload_dx11_dlls();
	}
}

void gfx_render_clear(const rgba_t* color)
{
	if (!gfx_ok())
		return;
	float clear_color[4];
	clear_color[0] = color->r / 255.f;
	clear_color[1] = color->g / 255.f;
	clear_color[2] = color->b / 255.f;
	clear_color[3] = color->a / 255.f;
	if (gfx && gfx->module->ctx && gfx->module->render_target) {
		struct gfx_texture2d* tex =
			(struct gfx_texture2d*)gfx->module->render_target->impl;
		if (tex && tex->rtv) {
			ID3D11DeviceContext1_ClearRenderTargetView(
				gfx->module->ctx, tex->rtv,
				(FLOAT*)clear_color);
		}

		struct gfx_texture2d* zs =
			(struct gfx_texture2d*)gfx->module->depth_target->impl;
		if (zs && zs->dsv) {
			u32 dsv_clear_flags = D3D11_CLEAR_DEPTH |
					      D3D11_CLEAR_STENCIL;
			u8 stencil = 0;
			ID3D11DeviceContext1_ClearDepthStencilView(
				gfx->module->ctx, zs->dsv, dsv_clear_flags, 1.f,
				stencil);
		}
	}
}

void gfx_render_begin(bool draw_indexed)
{
	if (!gfx_ok())
		return;

	gfx_vertex_shader_t* vs =
		(gfx_vertex_shader_t*)gfx->module->vertex_shader->impl;
	gfx_pixel_shader_t* ps =
		(gfx_pixel_shader_t*)gfx->module->pixel_shader->impl;

	ID3D11DeviceContext1* ctx = gfx->module->ctx;
	ID3D11DeviceContext1_VSSetShader(ctx, vs->program, NULL, 0);
	ID3D11DeviceContext1_PSSetShader(ctx, ps->program, NULL, 0);
	ID3D11DeviceContext1_PSSetSamplers(ctx, 0, 1,
					   &gfx->module->sampler_state);
	u32 start = 0;
	if (draw_indexed) {
		u32 base_vertex = 0;
		u32 index_count = BM_GFX_MAX_INDICES;
		ID3D11DeviceContext1_DrawIndexed(ctx, index_count, start, 0);
	} else {
		u32 vertex_count = BM_GFX_MAX_VERTICES;
		u32 start_vertex = 0;
		ID3D11DeviceContext1_Draw(ctx, vertex_count, start);
	}
}

void gfx_render_end(bool vsync, u32 flags)
{
	if (!gfx_ok())
		return;
	IDXGISwapChain1_Present(gfx->module->dxgi_swap_chain, (UINT)vsync,
				flags);
}

void gfx_set_vertex_shader(gfx_shader_t* vs)
{
	if (gfx != NULL)
		gfx->module->vertex_shader = vs;
}

void gfx_set_pixel_shader(gfx_shader_t* ps)
{
	if (gfx != NULL)
		gfx->module->pixel_shader = ps;
}

enum gfx_vertex_type gfx_get_vertex_type(void)
{
	if (gfx != NULL) {
		gfx_vertex_shader_t* vs =
			(gfx_vertex_shader_t*)gfx->module->vertex_shader->impl;
		if (vs != NULL)
			return vs->vertex_type;
	}
	return GFX_VERTEX_UNKNOWN;
}

// void gfx_init_sprite(gfx_buffer_t* vertex_buffer)
// {
// 	size_t sz = sizeof(struct gfx_vertex_data);
// 	struct gfx_vertex_data* vd = (struct gfx_vertex_data*)BM_ALLOC(sz);
// 	memset(vd, 0, sz);
// 	vd->num_vertices = 4;
// 	size_t sz_positions = sizeof(vec3f_t) * vd->num_vertices;
// 	vd->positions = (vec3f_t*)BM_ALLOC(sz_positions);
// 	vd->tex_verts = (struct texture_vertex*)BM_ALLOC(
// 		sizeof(struct texture_vertex));
// 	size_t sz_tex_verts = sizeof(vec2f_t) * 4;
// 	vd->tex_verts->data = BM_ALLOC(sz_tex_verts);
// 	vd->tex_verts->size = sizeof(vec2f_t);
// 	gfx_buffer_new(gfx, vd, sz_positions + sz_tex_verts,
// 			  GFX_BUFFER_VERTEX, GFX_BUFFER_USAGE_DYNAMIC,
// 			  &vertex_buffer);
// }

// void gfx_draw_sprite(struct gfx_texture* texture, u32 width,
// 		     u32 height, u32 flags)
// {
// }

//
// gfx buffer
//
u8* gfx_buffer_get_data(gfx_buffer_t* buf)
{
	if (buf != NULL) {
		return buf->data;
	}
	return NULL;
}

result gfx_buffer_make(gfx_buffer_t* buf, const void* data, size_t size)
{
	if (buf == NULL)
		return RESULT_NULL;

	if (data != NULL && size > 0) {
		buf->data = BM_ALLOC(size);
		buf->size = size;
		memcpy(buf->data, data, size);
	}

	u32 cpu_access_flags = 0;
	if (buf->usage == GFX_BUFFER_USAGE_DYNAMIC)
		cpu_access_flags = D3D11_CPU_ACCESS_WRITE;

	u32 bind_flags = 0;
	switch (buf->type) {
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
		.Usage = gfx_buffer_usage_to_d3d11_usage(buf->usage),
		.CPUAccessFlags = (UINT)cpu_access_flags,
		.ByteWidth = (UINT)size,
		.BindFlags = (UINT)bind_flags,
		.MiscFlags = 0,
		.StructureByteStride = 0};
	if (data != NULL) {
		D3D11_SUBRESOURCE_DATA srd = {.pSysMem = buf->data,
					      .SysMemPitch = 0,
					      .SysMemSlicePitch = 0};
		if (FAILED(ID3D11Device1_CreateBuffer(
			    gfx->module->device, &desc, &srd, &buf->buffer)))
			return RESULT_NULL;
	} else {
		if (FAILED(ID3D11Device1_CreateBuffer(
			    gfx->module->device, &desc, NULL, &buf->buffer)))
			return RESULT_NULL;
	}

	return RESULT_OK;
}

result gfx_buffer_new(const void* data, size_t size, enum gfx_buffer_type type,
		      enum gfx_buffer_usage usage, gfx_buffer_t** buf)
{
	if (!gfx_ok() || buf == NULL)
		return RESULT_NULL;

	if (type >= GFX_BUFFER_UNKNOWN) {
		logger(LOG_ERROR, "gfx_buffer_new: invalid type specified!");
		return RESULT_UNKNOWN;
	}
	if (usage >= GFX_BUFFER_USAGE_UNKNOWN) {
		logger(LOG_ERROR, "gfx_buffer_new: invalid usage specified!");
		return RESULT_UNKNOWN;
	}

	gfx_buffer_t* buffer = BM_ALLOC(sizeof(*buffer));
	buffer->buffer = NULL;
	buffer->data = NULL;
	buffer->own_data = false;
	buffer->size = size;
	buffer->type = type;
	buffer->usage = usage;
	if (data != NULL && size > 0) {
		buffer->data = BM_ALLOC(size);
		memcpy(buffer->data, data, size);
		buffer->own_data = true;
	} else if (data == NULL && size > 0) {
		buffer->data = BM_ALLOC(size);
		memset(buffer->data, 0, size);
		buffer->own_data = true;
	} else {
		buffer->data = (u8*)data;
		buffer->own_data = false;
	}

	u32 cpu_access_flags = 0;
	if (buffer->usage == GFX_BUFFER_USAGE_DYNAMIC)
		cpu_access_flags = D3D11_CPU_ACCESS_WRITE;

	u32 bind_flags = 0;
	switch (buffer->type) {
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
		.Usage = gfx_buffer_usage_to_d3d11_usage(buffer->usage),
		.CPUAccessFlags = (UINT)cpu_access_flags,
		.ByteWidth = (UINT)size,
		.BindFlags = (UINT)bind_flags,
		.MiscFlags = 0,
		.StructureByteStride = 0};
	if (buffer->data != NULL) {
		D3D11_SUBRESOURCE_DATA srd = {.pSysMem = buffer->data,
					      .SysMemPitch = 0,
					      .SysMemSlicePitch = 0};
		if (FAILED(ID3D11Device1_CreateBuffer(
			    gfx->module->device, &desc, &srd, &buffer->buffer)))
			return RESULT_NULL;
	} else {
		if (FAILED(ID3D11Device1_CreateBuffer(
			    gfx->module->device, &desc, NULL, &buffer->buffer)))
			return RESULT_NULL;
	}

	*buf = buffer;

	return RESULT_OK;
}

void gfx_buffer_free(gfx_buffer_t* buffer)
{
	if (buffer != NULL) {
		if (buffer->buffer != NULL) {
			ID3D11Buffer_Release(buffer->buffer);
			buffer->buffer = NULL;
		}
		if (buffer->data != NULL && buffer->own_data) {
			BM_FREE(buffer->data);
			buffer->data = NULL;
		}
		BM_FREE(buffer);
		buffer = NULL;
	}
}

size_t gfx_buffer_get_size(gfx_buffer_t* buf)
{
	if (buf != NULL) {
		return buf->size;
	}
	return 0;
}

result gfx_buffer_copy(gfx_buffer_t* buf, const void* data, size_t size)
{
	if (!gfx_ok() || !buf || !data || size == 0)
		return RESULT_NULL;

	D3D11_MAPPED_SUBRESOURCE sr = {
		.DepthPitch = 0,
		.pData = NULL,
		.RowPitch = 0,
	};

	if (FAILED(ID3D11DeviceContext1_Map(gfx->module->ctx,
					    (ID3D11Resource*)buf->buffer, 0,
					    D3D11_MAP_WRITE_DISCARD, 0, &sr)))
		return RESULT_ERROR;

	memcpy(sr.pData, data, size);

	ID3D11DeviceContext1_Unmap(gfx->module->ctx,
				   (ID3D11Resource*)buf->buffer, 0);

	return RESULT_OK;
}

void gfx_bind_vertex_buffer(gfx_buffer_t* vb, u32 stride, u32 offset)
{
	if (gfx_ok()) {
		ID3D11DeviceContext1_IASetVertexBuffers(gfx->module->ctx, 0, 1,
							&vb->buffer,
							(UINT*)&stride,
							(UINT*)&offset);
	}
}

void gfx_bind_index_buffer(gfx_buffer_t* ib, u32 offset)
{
	if (gfx_ok()) {
		ID3D11DeviceContext1_IASetIndexBuffer(gfx->module->ctx,
						      ib->buffer,
						      DXGI_FORMAT_R32_UINT,
						      offset);
	}
}

void gfx_buffer_upload_constants(const gfx_shader_t* shader)
{
	if (gfx_ok() && shader != NULL) {
		gfx_buffer_t* buf = shader->cbuffer;
		if (shader->type == GFX_SHADER_VERTEX)
			ID3D11DeviceContext1_VSSetConstantBuffers(
				gfx->module->ctx, 0, 1, &buf->buffer);
		else if (shader->type == GFX_SHADER_PIXEL)
			ID3D11DeviceContext1_PSSetConstantBuffers(
				gfx->module->ctx, 0, 1, &buf->buffer);
	}
}

/*
 * gfx shader
 */
void gfx_shader_free(gfx_shader_t* shader)
{
	if (shader != NULL) {
		gfx_buffer_free(shader->cbuffer);
		switch (shader->type) {
		case GFX_SHADER_VERTEX: {
			if (!gfx_vertex_shader_free(
				    (gfx_vertex_shader_t*)shader->impl)) {
				gfx_vertex_shader_release(shader->impl);
			}
			break;
		}
		case GFX_SHADER_PIXEL: {
			if (!gfx_pixel_shader_free(
				    (gfx_pixel_shader_t*)shader->impl)) {
				gfx_pixel_shader_release(shader->impl);
			}
			break;
		}
		case GFX_SHADER_GEOMETRY:
		case GFX_SHADER_COMPUTE:
		default:
			logger(LOG_WARNING,
			       "[gfx] shader type %s not implemented!",
			       gfx_shader_type_to_string(shader->type));
			break;
		}
		for (size_t i = 0; i < shader->vars.num_elems; i++) {
			gfx_shader_var_free(&shader->vars.elems[i]);
		}
		vec_free(shader->vars);
		BM_FREE(shader);
		shader = NULL;
	}
}

size_t gfx_shader_cbuffer_fill(gfx_shader_t* shader)
{
	/* Copy shader vars into the constant buffer */
	if (shader == NULL || shader->cbuffer == NULL ||
	    shader->cbuffer->data == NULL) {
		return 0;
	}

	size_t num_vars = shader->vars.num_elems;
	if (num_vars == 0)
		return 0;

	size_t cbuf_size = gfx_shader_get_vars_size(shader);
	if (cbuf_size > 0) {
		u8* data = shader->cbuffer->data;
		size_t offset = 0;
		for (size_t vdx = 0; vdx < num_vars; vdx++) {
			gfx_shader_var_t* var = &shader->vars.elems[vdx];
			size_t var_size = gfx_shader_var_size(var->type);
			if (var->type == GFX_SHADER_VAR_TEX) {
				// bind sampler state here?
			} else {
				memcpy(&data[offset], (const void*)var->data,
				       var_size);
				offset += (var_size + 15) & ~15;
			}
		}
	}

	gfx_buffer_copy(shader->cbuffer, shader->cbuffer->data, cbuf_size);

	return cbuf_size;
}

void gfx_vertex_shader_init(gfx_vertex_shader_t* vs)
{
	if (vs != NULL) {
		vs->blob = NULL;
		vs->input_layout = NULL;
		vs->program = NULL;
		vs->vertex_type = GFX_VERTEX_UNKNOWN;
		os_atomic_set_s32(&vs->ref_count, 0);
	}
}

gfx_vertex_shader_t* gfx_vertex_shader_new()
{
	gfx_vertex_shader_t* vs = BM_ALLOC(sizeof(gfx_vertex_shader_t));
	gfx_vertex_shader_init(vs);
	return vs;
}

bool gfx_vertex_shader_addref(gfx_vertex_shader_t* vs)
{
	if (vs) {
		os_atomic_inc_s32(&vs->ref_count);
		return true;
	}
	return false;
}

bool gfx_vertex_shader_release(gfx_vertex_shader_t* vs)
{
	if (vs) {
		os_atomic_dec_s32(&vs->ref_count);
		return true;
	}
	return false;
}

bool gfx_vertex_shader_free(gfx_vertex_shader_t* vs)
{
	if (vs != NULL) {
		s32 refs = os_atomic_get_s32(&vs->ref_count);
		if (refs <= 0) {
			ID3D10Blob_Release(vs->blob);
			vs->blob = NULL;
			ID3D11InputLayout_Release(vs->input_layout);
			vs->input_layout = NULL;
			ID3D11VertexShader_Release(vs->program);
			vs->program = NULL;
			vs->vertex_type = GFX_VERTEX_UNKNOWN;
			BM_FREE(vs);
			vs = NULL;
			return true;
		}
	}
	return false;
}

enum gfx_vertex_type gfx_vertex_shader_get_vertex_type(gfx_vertex_shader_t* vs)
{
	return vs->vertex_type;
}

void gfx_vertex_shader_set_vertex_type(gfx_vertex_shader_t* vs,
				       enum gfx_vertex_type type)
{
	vs->vertex_type = type;
}

void gfx_pixel_shader_init(gfx_pixel_shader_t* ps)
{
	if (ps != NULL) {
		ps->blob = NULL;
		ps->program = NULL;
		os_atomic_set_s32(&ps->ref_count, 0);
	}
}

gfx_pixel_shader_t* gfx_pixel_shader_new()
{
	gfx_pixel_shader_t* ps = BM_ALLOC(sizeof(gfx_pixel_shader_t));
	gfx_pixel_shader_init(ps);
	return ps;
}

bool gfx_pixel_shader_addref(gfx_pixel_shader_t* ps)
{
	if (ps) {
		os_atomic_inc_s32(&ps->ref_count);
		return true;
	}
	return false;
}

bool gfx_pixel_shader_release(gfx_pixel_shader_t* ps)
{
	if (ps) {
		os_atomic_dec_s32(&ps->ref_count);
		return true;
	}
	return false;
}

bool gfx_pixel_shader_free(gfx_pixel_shader_t* ps)
{
	if (ps != NULL) {
		s32 refs = os_atomic_get_s32(&ps->ref_count);
		if (refs <= 0) {
			ID3D10Blob_Release(ps->blob);
			ps->blob = NULL;
			ID3D11PixelShader_Release(ps->program);
			ps->program = NULL;
			BM_FREE(ps);
			ps = NULL;
			return true;
		}
	}
	return false;
}

// original stuff -----------------------------------------------------------------------------------------
result gfx_shader_compile_from_file(const char* path, const char* entrypoint,
				    const char* target, gfx_shader_t* shader)
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
	case GFX_SHADER_VERTEX: {
		gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
		if (vs == NULL)
			return RESULT_NULL;
		shader_blob = &vs->blob;
		break;
	}
	case GFX_SHADER_PIXEL: {
		gfx_pixel_shader_t* ps = (gfx_pixel_shader_t*)shader->impl;
		if (ps == NULL)
			return RESULT_NULL;
		shader_blob = &ps->blob;
		break;
	}
	default:
		logger(LOG_ERROR,
		       "[gfx] compilation of %s shaders not implemented!",
		       gfx_shader_type_to_string(shader->type));
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

result gfx_shader_build_program(gfx_shader_t* shader)
{
	if (gfx == NULL || gfx->module->device == NULL || shader == NULL)
		return RESULT_NULL;

	switch (shader->type) {
	case GFX_SHADER_VERTEX: {
		gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
		if (vs == NULL)
			return RESULT_NULL;
		if (FAILED(ID3D11Device1_CreateVertexShader(
			    gfx->module->device,
			    ID3D10Blob_GetBufferPointer(vs->blob),
			    ID3D10Blob_GetBufferSize(vs->blob), NULL,
			    (ID3D11VertexShader**)&vs->program))) {
			logger(LOG_DEBUG,
			       "[gfx] Failed to build vertex shader!");
			ID3D11VertexShader_Release(vs->program);
			vs->program = NULL;
		}
		break;
	}
	case GFX_SHADER_PIXEL: {
		gfx_pixel_shader_t* ps = (gfx_pixel_shader_t*)shader->impl;
		if (ps == NULL)
			return RESULT_NULL;
		if (FAILED(ID3D11Device1_CreatePixelShader(
			    gfx->module->device,
			    ID3D10Blob_GetBufferPointer(ps->blob),
			    ID3D10Blob_GetBufferSize(ps->blob), NULL,
			    (ID3D11PixelShader**)&ps->program))) {
			logger(LOG_DEBUG,
			       "[gfx] Failed to build pixel shader!");
			ID3D11PixelShader_Release(ps->program);
			ps->program = NULL;
		}
		break;
	}
	default:
		logger(LOG_ERROR,
		       "[gfx] building of %s shaders not implemented!",
		       gfx_shader_type_to_string(shader->type));
		return RESULT_NOT_IMPL;
	}

	return RESULT_OK;
}

result gfx_shader_new_input_layout(gfx_shader_t* shader)
{
	if (!gfx_ok() || !shader)
		return RESULT_NULL;

	gfx_vertex_shader_t* vs = (gfx_vertex_shader_t*)shader->impl;
	if (!vs)
		return RESULT_NULL;

	const D3D11_INPUT_ELEMENT_DESC* descs = NULL;
	size_t num_elems = 0;
	switch (vs->vertex_type) {
	case GFX_VERTEX_ID:
		descs = &kVertexDescID[0];
		num_elems = 1;
		break;
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
			    gfx->module->device, descs, (UINT)num_elems, data,
			    size, &vs->input_layout))) {
			ID3D11InputLayout_Release(vs->input_layout);
			return RESULT_NULL;
		}
	}

	return RESULT_OK;
}

void gfx_bind_primitive_topology(enum gfx_topology topo)
{
	if (gfx && gfx->module->ctx) {
		ID3D11DeviceContext1_IASetPrimitiveTopology(
			gfx->module->ctx, gfx_topology_to_d3d11_topology(topo));
	}
}

void gfx_vertex_shader_bind_input_layout(const gfx_vertex_shader_t* vs)
{
	if (gfx && gfx->module->ctx && vs) {
		ID3D11DeviceContext1_IASetInputLayout(gfx->module->ctx,
						      vs->input_layout);
	}
}

//
// gfx sampler
//
result gfx_init_sampler_state(void)
{
	if (!gfx_ok() || !gfx->module->device)
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

	if (FAILED(ID3D11Device1_CreateSamplerState(
		    gfx->module->device, &sd, &gfx->module->sampler_state))) {
		logger(LOG_DEBUG, "[gfx] Failed to create sampler state!");

		ID3D11SamplerState_Release(gfx->module->sampler_state);
		gfx->module->sampler_state = NULL;

		return RESULT_ERROR;
	}

	return RESULT_OK;
}

void gfx_bind_sampler_state(gfx_texture_t* texture, u32 slot)
{
	if (gfx && gfx->module->ctx && texture) {
		struct gfx_texture2d* impl =
			(struct gfx_texture2d*)(texture->impl);
		if (impl) {
			UINT views = 1;
			ID3D11DeviceContext1_PSSetShaderResources(
				gfx->module->ctx, slot, views, &impl->srv);
			ID3D11DeviceContext1_PSSetSamplers(
				gfx->module->ctx, slot, 1,
				&gfx->module->sampler_state);
		}
	}
}

//
// gfx rasterizer
//
result gfx_init_rasterizer(const struct gfx_raster_state_desc* desc)
{
	if (!desc)
		return RESULT_NULL;

	bool changed = false;
	if (gfx->module->raster_desc.culling_mode != desc->culling_mode ||
	    gfx->module->raster_desc.winding_order != desc->winding_order ||
	    gfx->module->raster_desc.raster_flags != desc->raster_flags) {
		changed = true;
	}

	if (gfx->module->raster_state && changed) {
		ID3D11RasterizerState1_Release(gfx->module->raster_state);
		gfx->module->raster_state = NULL;
	}

	if (!gfx->module->raster_state) {
		D3D11_RASTERIZER_DESC rd = {
			.FillMode = (desc->raster_flags & GFX_RASTER_WIREFRAME)
					    ? D3D11_FILL_WIREFRAME
					    : D3D11_FILL_SOLID,
			.CullMode = gfx_culling_mode_to_d3d11_cull_mode(
				desc->culling_mode),
			.FrontCounterClockwise =
				(BOOL)(desc->winding_order == GFX_WINDING_CCW),
			.DepthBias = 0,
			.DepthBiasClamp = 0.f,
			.SlopeScaledDepthBias = 0.f,
			.DepthClipEnable = TRUE,
			.ScissorEnable =
				(desc->raster_flags & GFX_RASTER_SCISSOR),
			.MultisampleEnable =
				(desc->raster_flags & GFX_RASTER_MULTI_SAMPLE),
			.AntialiasedLineEnable = (desc->raster_flags &
						  GFX_RASTER_ANTIALIAS_LINES),
		};
		if (FAILED(ID3D11Device1_CreateRasterizerState(
			    gfx->module->device, &rd,
			    &gfx->module->raster_state))) {
			return RESULT_NULL;
		}
	}

	return RESULT_OK;
}

void gfx_bind_rasterizer(void)
{
	if (gfx && gfx->module->device) {
		ID3D11DeviceContext1_RSSetState(gfx->module->ctx,
						gfx->module->raster_state);
	}
}

//
// gfx blend
//
result gfx_init_blend_state()
{
	return RESULT_OK;
}

void gfx_bind_blend_state() {}

//
// gfx texture
//
void gfx_texture_init2d(struct gfx_texture2d* tex2d)
{
	if (tex2d != NULL) {
		tex2d->pix_fmt = PIX_FMT_UNKNOWN;
		tex2d->width = 0;
		tex2d->height = 0;
		tex2d->mip_levels = 0;
		tex2d->flags = 0;
		tex2d->texture = NULL;
		tex2d->srv = NULL;
		tex2d->rtv = NULL;
		tex2d->dsv = NULL;
	}
}

result gfx_texture2d_create(const u8* data, const struct gfx_texture_desc* desc,
			    gfx_texture_t** texture)
{
	if (!gfx_hardware_ok())
		return RESULT_ERROR;

	if (!texture || !*texture)
		return RESULT_NULL;

	gfx_texture_t* tex = *texture;
	tex->impl = BM_ALLOC(sizeof(struct gfx_texture2d));
	struct gfx_texture2d* tex2d = (struct gfx_texture2d*)tex->impl;
	gfx_texture_init2d(tex2d);
	tex2d->width = desc->width;
	tex2d->height = desc->height;
	tex2d->pix_fmt = desc->pix_fmt;
	tex2d->mip_levels = desc->mip_levels;
	DXGI_FORMAT dxgi_format = pixel_format_to_dxgi_format(desc->pix_fmt);

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
	u32 stride = desc->width * (pix_fmt_bits_per_pixel(desc->pix_fmt) / 8);
	tex->size = desc->height * stride;
	if (data) {
		tex->data = (u8*)BM_ALLOC(tex->size);
		memcpy(tex->data, data, tex->size);

		D3D11_SUBRESOURCE_DATA srd = {
			.pSysMem = (void*)tex->data,
			.SysMemPitch = stride,
			.SysMemSlicePitch = (UINT)tex->size,
		};

		if (FAILED(ID3D11Device1_CreateTexture2D(gfx->module->device,
							 &tex_desc, &srd,
							 &tex2d->texture)))
			return RESULT_NULL;
	} else {
		if (FAILED(ID3D11Device1_CreateTexture2D(gfx->module->device,
							 &tex_desc, NULL,
							 &tex2d->texture)))
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
			    gfx->module->device,
			    (ID3D11Resource*)tex2d->texture, &dsv_desc,
			    &tex2d->dsv)))
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
			    gfx->module->device,
			    (ID3D11Resource*)tex2d->texture, &srv_desc,
			    &tex2d->srv)))
			return RESULT_NULL;
	}

	return RESULT_OK;
}

void gfx_texture2d_destroy(gfx_texture_t* texture)
{
	if (texture != NULL) {
		struct gfx_texture2d* tex2d =
			(struct gfx_texture2d*)texture->impl;
		if (tex2d->texture != NULL) {
			ID3D11Texture2D_Release(tex2d->texture);
		}
		if (tex2d->rtv) {
			ID3D11RenderTargetView_Release(tex2d->rtv);
		}
		if (tex2d->dsv) {
			ID3D11DepthStencilView_Release(tex2d->dsv);
		}
		if (tex2d->srv) {
			ID3D11ShaderResourceView_Release(tex2d->srv);
		}
	}
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_render_target_init(u32 width, u32 height, enum pixel_format pf)
{
	if (!gfx_hardware_ok())
		return RESULT_NULL;

	struct gfx_texture_desc desc = {
		.type = GFX_TEXTURE_2D,
		.pix_fmt = pf,
		.width = width,
		.height = height,
		.mip_levels = 1,
		.flags = GFX_TEXTURE_IS_RENDER_TARGET,
	};

	ENSURE_OK(gfx_texture_create(NULL, &desc, &gfx->module->render_target));
	struct gfx_texture2d* tex =
		(struct gfx_texture2d*)gfx->module->render_target->impl;
	HRESULT hr = IDXGISwapChain2_GetBuffer(gfx->module->dxgi_swap_chain, 0,
					       &BM_IID_ID3D11Texture2D,
					       (void**)&tex->texture);
	if (FAILED(hr))
		return RESULT_NULL;

	// D3D11_RENDER_TARGET_VIEW_DESC rtv_desc; // TODO?
	hr = ID3D11Device1_CreateRenderTargetView(gfx->module->device,
						  (ID3D11Resource*)tex->texture,
						  NULL, &tex->rtv);
	if (FAILED(hr))
		return RESULT_NULL;

	return RESULT_OK;
}

void gfx_render_target_destroy(void)
{
	if (gfx && gfx->module->render_target) {
		gfx_texture_destroy(gfx->module->render_target);
	}
}

void gfx_set_render_targets(gfx_texture_t* texture, gfx_texture_t* depth)
{
	if (gfx_hardware_ok()) {
		ID3D11RenderTargetView* rtv = NULL;
		ID3D11DepthStencilView* dsv = NULL;
		if (texture && texture->impl) {
			struct gfx_texture2d* tex =
				(struct gfx_texture2d*)texture->impl;
			rtv = tex->rtv;
		}
		if (depth && depth->impl) {
			struct gfx_texture2d* tex =
				(struct gfx_texture2d*)depth->impl;
			dsv = tex->dsv;
		}
		ID3D11DeviceContext1_OMSetRenderTargets(gfx->module->ctx, 1,
							&rtv, dsv);
	}
}

result gfx_create_depth_state(bool enable, struct gfx_depth_state** state)
{
	if (!gfx_hardware_ok() || state == NULL)
		return RESULT_NULL;

	struct gfx_depth_state* zss = *state;
	if (!zss)
		return RESULT_NULL;

	zss->enabled = enable;

	// TODO(paulh): Port the other aspects of this config to gfx_depth_state
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

	if (FAILED(ID3D11Device1_CreateDepthStencilState(gfx->module->device,
							 &desc, &zss->dss)))
		return RESULT_NULL;

	return RESULT_OK;
}

// TODO(paulh): Release stuff if failed!!!!!
result gfx_init_depth(u32 width, u32 height, enum pixel_format pix_fmt,
		      bool enabled)
{
	if (!gfx_hardware_ok())
		return RESULT_NULL;

	struct gfx_texture_desc desc = {
		.type = GFX_TEXTURE_2D,
		.pix_fmt = pix_fmt,
		.width = width,
		.height = height,
		.mip_levels = 1,
		.flags = GFX_TEXTURE_IS_ZSTENCIL,
	};

	ENSURE_OK(gfx_texture_create(NULL, &desc, &gfx->module->depth_target));
	// TODO(paulh): Add more depth state options eventually
	gfx->module->depth_state_enabled =
		BM_ALLOC(sizeof(*gfx->module->depth_state_enabled));
	gfx->module->depth_state_disabled =
		BM_ALLOC(sizeof(*gfx->module->depth_state_disabled));
	gfx_create_depth_state(false, &gfx->module->depth_state_disabled);
	gfx_create_depth_state(true, &gfx->module->depth_state_enabled);
	gfx_toggle_depth(enabled);

	return RESULT_OK;
}

void gfx_destroy_depth(void)
{
	if (gfx && gfx->module->depth_state_enabled &&
	    gfx->module->depth_state_disabled && gfx->module->depth_target) {
		ID3D11DepthStencilState_Release(
			gfx->module->depth_state_enabled->dss);
		BM_FREE(gfx->module->depth_state_enabled);
		gfx->module->depth_state_enabled = NULL;

		ID3D11DepthStencilState_Release(
			gfx->module->depth_state_disabled->dss);
		BM_FREE(gfx->module->depth_state_disabled);
		gfx->module->depth_state_disabled = NULL;

		gfx_texture_destroy(gfx->module->depth_target);
	}
}

void gfx_bind_depth_state(const struct gfx_depth_state* state)
{
	if (gfx != NULL && gfx->module != NULL && gfx->module->ctx != NULL &&
	    state != NULL && state->dss != NULL) {
		ID3D11DeviceContext1_OMSetDepthStencilState(gfx->module->ctx,
							    state->dss, 1);
	}
}

void gfx_toggle_depth(bool enabled)
{
	if (gfx != NULL && gfx->module != NULL) {
		if (enabled) {
			gfx_bind_depth_state(gfx->module->depth_state_enabled);
		} else {
			gfx_bind_depth_state(gfx->module->depth_state_disabled);
		}
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

D3D11_BLEND gfx_blend_mode_to_d3d11_blend(enum gfx_blend_mode mode)
{
	switch (mode) {
	case GFX_BLEND_ZERO:
		return D3D11_BLEND_ZERO;
	case GFX_BLEND_ONE:
		return D3D11_BLEND_ONE;
	case GFX_BLEND_SRCCOLOR:
		return D3D11_BLEND_SRC_COLOR;
	case GFX_BLEND_INVSRCCOLOR:
		return D3D11_BLEND_INV_SRC_COLOR;
	case GFX_BLEND_SRCALPHA:
		return D3D11_BLEND_SRC_ALPHA;
	case GFX_BLEND_INVSRCALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case GFX_BLEND_DSTCOLOR:
		return D3D11_BLEND_DEST_COLOR;
	case GFX_BLEND_INVDSTCOLOR:
		return D3D11_BLEND_INV_DEST_COLOR;
	case GFX_BLEND_DSTALPHA:
		return D3D11_BLEND_DEST_ALPHA;
	case GFX_BLEND_INVDSTALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case GFX_BLEND_SRCALPHASAT:
		return D3D11_BLEND_SRC_ALPHA_SAT;
	}

	return D3D11_BLEND_ONE;
}

D3D11_BLEND_OP gfx_blend_op_to_d3d11_blend_op(enum gfx_blend_op op)
{
	switch (op) {
	case GFX_BLEND_OP_ADD:
		return D3D11_BLEND_OP_ADD;
	case GFX_BLEND_OP_SUBTRACT:
		return D3D11_BLEND_OP_SUBTRACT;
	case GFX_BLEND_OP_REVERSE_SUBTRACT:
		return D3D11_BLEND_OP_REV_SUBTRACT;
	case GFX_BLEND_OP_MIN:
		return D3D11_BLEND_OP_MIN;
	case GFX_BLEND_OP_MAX:
		return D3D11_BLEND_OP_MAX;
	}
	return D3D11_BLEND_OP_ADD;
}
