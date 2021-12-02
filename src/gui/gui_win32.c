#include "core/types.h"
#include "platform/platform.h"
#include "gui/gui.h"

#include <Windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define BM_WIN32_CLASS_STYLE CS_DBLCLKS
#define BM_WIN32_WINDOW_STYLE (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)
#define BM_WIN32_WINDOW_CHILD_STYLE \
	(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
#define BM_WIN32_WINDOW_STYLE_EX                     \
	(WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW) & \
		~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

struct gui_window_data {
	gui_window_t* window;
	HWND hwnd;
	HDC hdc;
	HINSTANCE instance;
	WNDPROC wndproc;
};

static HINSTANCE g_hinstance = NULL;
static ATOM g_atom = 0;
static const wchar_t* g_classname = L"BMAppClass";

LRESULT gui_process_keyboard_msg_win32(UINT msg, WPARAM wp, LPARAM lp)
{
	LRESULT result = 0;
	bool key_down = false;
	int vk = (int)wp;

	switch (msg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		key_down = true;
		result = 1;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	default:
		break;
	}

	switch (vk) {
	case VK_SHIFT:
		vk = (int)MapVirtualKey(((UINT)lp & 0x00ff0000) >> 16u,
					MAPVK_VSC_TO_VK_EX);
		if (!key_down) {
		}
	}

	return result;
}

HMODULE get_module_from_wndproc(WNDPROC wp)
{
	HMODULE instance = NULL;
	void* address = (void*)wp;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
				  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			  (LPCWSTR)address, &instance);
	return instance;
}

HWND get_window_to_parent_to(bool get_real_hwnd)
{
	return get_real_hwnd ? GetDesktopWindow() : HWND_DESKTOP;
}

void* set_window_user_data(HWND hwnd, void* user_data)
{
	return (void*)SetWindowLongPtr(hwnd, GWLP_USERDATA,
				       (LONG_PTR)user_data);
}

void* get_window_user_data(HWND hwnd)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid != GetCurrentProcessId())
		return NULL;
	return (void*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
}

LRESULT CALLBACK gui_win32_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	gui_window_data_t* data = NULL;
	// LRESULT result = 0;

	switch (msg) {
	case WM_NCCREATE: {
		CREATESTRUCT* cs = (CREATESTRUCT*)lp;
		if (cs) {
			data = (gui_window_data_t*)cs->lpCreateParams;
			if (data) {
				data->hwnd = hwnd;
				set_window_user_data(hwnd, data);
			}
		}
	} break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		return gui_process_keyboard_msg_win32(msg, wp, lp);
	default:
		data = (gui_window_data_t*)get_window_user_data(hwnd);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

bool gui_create_window_win32(gui_platform_t* gs, gui_window_t* window)
{
	if (!window)
		return false;

	HWND hwnd = NULL;
	HWND parent_hwnd = NULL;
	HMENU menu = NULL;

	DWORD style = BM_WIN32_WINDOW_STYLE;
	DWORD style_ex = BM_WIN32_WINDOW_STYLE_EX;

	if (window->parent && window->parent->data) {
		style = BM_WIN32_WINDOW_CHILD_STYLE;
		parent_hwnd = window->parent->data->hwnd;
	}

	if (parent_hwnd == HWND_DESKTOP)
		parent_hwnd = get_window_to_parent_to(false);
	else if (parent_hwnd == GetDesktopWindow())
		parent_hwnd = get_window_to_parent_to(true);

	wchar_t window_title[512];
	memset(&window_title, 0, sizeof(window_title));
	os_utf8_to_wcs(window->title, 0, window_title, sizeof(window_title));

	window->data = (gui_window_data_t*)calloc(1, sizeof(gui_window_data_t));
	window->data->instance = g_hinstance;
	window->data->window = window;
	window->data->wndproc = NULL;

	hwnd = CreateWindowEx(style_ex, (LPCWSTR)g_atom, (LPCWSTR)window_title,
			      style, window->x, window->y, window->w, window->h,
			      parent_hwnd, menu, window->data->instance,
			      window->data);

	gs->show_window(gs, window, true);

	return true;
}

void gui_destroy_window_win32(gui_platform_t* gs, gui_window_t* window)
{
	if (window) {
		if (window->data) {
			free((gui_window_data_t*)window->data);
			window->data = NULL;
		}
		gui_destroy_window(window);
	}
}

void gui_show_window_win32(gui_platform_t* gs, gui_window_t* window, bool shown)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		ShowWindow(data->hwnd, (int)shown);
	}
}

void* gui_get_window_handle_win32(gui_platform_t* gs, gui_window_t* window)
{
	return (void*)window->data->hwnd;
}

bool gui_init_win32(gui_platform_t* gs)
{
	g_hinstance = get_module_from_wndproc(gui_win32_wndproc);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hCursor = NULL;
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_classname;
	wcex.style = BM_WIN32_CLASS_STYLE;
	wcex.hbrBackground = NULL;
	wcex.lpfnWndProc = gui_win32_wndproc;
	wcex.hInstance = g_hinstance;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	g_atom = RegisterClassEx(&wcex);

	/* function pointerz */
	gs->create_window = gui_create_window_win32;
	gs->destroy_window = gui_destroy_window_win32;
	gs->show_window = gui_show_window_win32;
	gs->get_handle = gui_get_window_handle_win32;

	return true;
}
