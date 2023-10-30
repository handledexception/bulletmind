#include "platform/platform.h"

#include "gfx/gfx_d3d12.h"
#include "gfx/gfx.h"
#include "gfx/dxguids.h"
#include "gfx/win32_types.h"
#define COBJMACROS
#define CINTERFACE
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <dxgiformat.h>
#include <d3d12.h>

typedef HRESULT(WINAPI* PFN_CREATE_DXGI_FACTORY1)(REFIID riid, _COM_Outptr_ void **ppFactory);
static PFN_CREATE_DXGI_FACTORY1 CreateDXGIFactory1Func = NULL;
static PFN_D3D12_CREATE_DEVICE D3D12CreateDeviceFunc = NULL;

struct gfx_module {
    enum gfx_module_type type;
    HMODULE dxgi_dll;
    HMODULE d3d12_dll;
    gui_window_t* window;
    HWND hwnd;

    IDXGIFactory1* dxgi_factory;

    ID3D12Device* device;
};

result gfx_load_dx12_dlls()
{
	if (gfx != NULL && gfx->module != NULL) {
		gfx->module->dxgi_dll = os_dlopen("dxgi.dll");
		if (!gfx->module->dxgi_dll) {
			return RESULT_NULL;
		}
		logger(LOG_INFO, "[gfx] Loaded dxgi.dll");

		gfx->module->d3d12_dll = os_dlopen("d3d12.dll");
		if (!gfx->module->d3d12_dll) {
			return RESULT_NULL;
		}
		logger(LOG_INFO, "[gfx] Loaded d3d12.dll");
	}
	return RESULT_OK;
}

result gfx_load_dx12_functions()
{
	if (gfx->module->dxgi_dll) {
		CreateDXGIFactory1Func =
			os_dlsym(gfx->module->dxgi_dll, "CreateDXGIFactory1");
		if (!CreateDXGIFactory1Func) {
			return RESULT_NULL;
		}
	}
	if (gfx->module->d3d12_dll) {
		D3D12CreateDeviceFunc =
			os_dlsym(gfx->module->d3d12_dll, "D3D12CreateDevice");
		if (!D3D12CreateDeviceFunc) {
			return RESULT_NULL;
		}
	}
	return RESULT_OK;
}

result gfx_create_device(s32 adapter)
{
    result res = RESULT_OK;
    HRESULT hr = S_OK;

    ENSURE_OK(gfx_load_dx12_dlls());
    ENSURE_OK(gfx_load_dx12_functions());

	UINT dxgi_flags = 0;
#if defined(BM_DEBUG)
	dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    hr = CreateDXGIFactory1Func(dxgi_flags, &BM_IID_IDXGIFactory1,
				    (void**)&gfx->module->dxgi_factory);
	if (FAILED(hr)) {
		return RESULT_ERROR;
	}
    return res;
}
