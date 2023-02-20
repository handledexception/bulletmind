#include "core/types.h"
#include "core/memory.h"
#include "core/logger.h"

#include "platform/platform.h"

#include "gui/gui.h"
#include "platform/keyboard-vkey-win32.h"

#include <Windows.h>
#include <dwmapi.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(BM_USE_CIMGUI)
#include "cimgui.h"
#include "cimgui_impl.h"
#endif

// #define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
//                              WS_CAPTION        | \
//                              WS_SYSMENU        | \
//                              WS_THICKFRAME     | \
//                              WS_MINIMIZEBOX    | \
//                              WS_MAXIMIZEBOX)

#define GUI_WIN32_CLASS_STYLE CS_DBLCLKS
#define GUI_WIN32_WINDOW_STYLE (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)
#define GUI_WIN32_WINDOW_CHILD_STYLE \
	(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
#define GUI_WIN32_WINDOW_STYLE_EX                    \
	(WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW) & \
		~(WS_EX_DLGMODALFRAME | WS_EX_STATICEDGE | WS_EX_CLIENTEDGE)

static HINSTANCE g_hinstance = NULL;
static ATOM g_atom = 0;
static const wchar_t* g_classname = L"BMAppClass";

struct gui_window_data {
	gui_window_t* window;
	HWND hwnd;
	HDC hdc;
	HINSTANCE instance;
	WNDPROC wndproc;
	DWORD style;
	DWORD style_ex;
};

/* utils */
keyboard_scancode_t win32_virtual_key_to_scancode(WPARAM wp, LPARAM lp);
void* set_window_user_data(HWND hwnd, void* user_data);
void* get_window_user_data(HWND hwnd);
HMODULE get_module_from_wndproc(WNDPROC wp);

/* window proc */
LRESULT CALLBACK gui_win32_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

bool gui_create_window_win32(gui_window_t* window);
void gui_destroy_window_win32(gui_window_t* window);
bool gui_is_valid_window_win32(const gui_window_t* window);
void gui_show_window_win32(gui_window_t* window, bool shown);
void gui_set_window_pos_win32(gui_window_t* window, const rect_t* rect);
bool gui_set_focused_window_win32(const gui_window_t* window);
void gui_center_window_win32(gui_window_t* window);
bool gui_get_window_size_win32(const gui_window_t* window, s32* w, s32* h, bool client);
bool gui_get_window_rect_win32(const gui_window_t* window, rect_t* rect, bool client);
bool gui_get_window_centerpoint_win32(const gui_window_t* window, vec2i_t* p, bool client);
void* gui_get_window_handle_win32(gui_window_t* window);
void gui_read_mouse_state_win32(mouse_t* mouse);
bool gui_set_mouse_mode_win32(enum mouse_mode mode);
bool gui_capture_mouse_win32(gui_window_t* window, bool captured);
bool gui_move_mouse_win32(s32 x, s32 y);
bool gui_show_mouse_win32(bool shown);

LRESULT gui_process_mouse_move_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT gui_process_mouse_button_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT gui_process_mouse_capture_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT gui_process_keyboard_win32(UINT msg, WPARAM wp, LPARAM lp);
LRESULT gui_process_window_focus_lost_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT gui_process_window_activate_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

result gui_init_win32(gui_system_t* gp)
{
	g_hinstance = get_module_from_wndproc(gui_win32_wndproc);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_classname;
	wcex.style = GUI_WIN32_CLASS_STYLE;
	wcex.hbrBackground = NULL;
	wcex.lpfnWndProc = gui_win32_wndproc;
	wcex.hInstance = g_hinstance;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	g_atom = RegisterClassEx(&wcex);

	/* function pointerz */
	gp->create_window = gui_create_window_win32;
	gp->destroy_window = gui_destroy_window_win32;
	gp->is_valid_window = gui_is_valid_window_win32;
	gp->show_window = gui_show_window_win32;
	gp->set_window_pos = gui_set_window_pos_win32;
	gp->focus_window = gui_set_focused_window_win32;
	gp->center_window = gui_center_window_win32;
	gp->get_window_size = gui_get_window_size_win32;
	gp->get_window_rect = gui_get_window_rect_win32;
	gp->get_window_centerpoint = gui_get_window_centerpoint_win32;
	gp->get_handle = gui_get_window_handle_win32;
	gp->read_mouse_state = gui_read_mouse_state_win32;
	gp->set_mouse_mode = gui_set_mouse_mode_win32;
	gp->capture_mouse = gui_capture_mouse_win32;
	gp->move_mouse = gui_move_mouse_win32;
	gp->show_mouse = gui_show_mouse_win32;

	return RESULT_OK;
}

LRESULT CALLBACK gui_win32_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
#if defined(BM_USE_CIMGUI)
	// Allow DearIMGUI to handle Win32 events
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
		return 0;
#endif
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
	case WM_ACTIVATEAPP:
		logger(LOG_DEBUG, "WM_ACTIVATEAPP");
		gui_clear_key_state();
		break;
	// case WM_MOUSELEAVE:
	// 	return gui_process_mouse_leave_win32(hwnd, msg, wp, lp);
	case WM_KILLFOCUS:
		return gui_process_window_focus_lost_win32(hwnd, msg, wp, lp);
	case WM_ACTIVATE:
		logger(LOG_DEBUG, "WM_ACTIVATE");
		return gui_process_window_activate_win32(hwnd, msg, wp, lp);
	case WM_MOUSEACTIVATE:
		logger(LOG_DEBUG, "WM_MOUSEACTIVATE");
		return gui_process_window_activate_win32(hwnd, msg, wp, lp);
	case WM_MOUSEMOVE:
		return gui_process_mouse_move_win32(hwnd, msg, wp, lp);
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		return gui_process_mouse_button_win32(hwnd, msg, wp, lp);
	case WM_CAPTURECHANGED:
		logger(LOG_DEBUG, "WM_CAPTURECHANGED");
		return gui_process_mouse_capture_win32(hwnd, msg, wp, lp);
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		return gui_process_keyboard_win32(msg, wp, lp);
	default:
		data = (gui_window_data_t*)get_window_user_data(hwnd);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

void gui_refresh_win32(gui_system_t* gp)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT gui_process_mouse_move_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	gui_event_t evt;
	memset(&evt, 0, sizeof(gui_event_t));
	evt.timestamp = os_get_time_ns();
	evt.type = GUI_EVENT_MOUSE_MOTION;
	evt.mouse.mouse.screen_pos   = vec2i_copy(gui->mouse.screen_pos);
	evt.mouse.mouse.screen_delta = vec2i_copy(gui->mouse.screen_delta);
	evt.mouse.mouse.window_pos   = vec2i_copy(gui->mouse.window_pos);
	evt.mouse.mouse.window_delta = vec2i_copy(gui->mouse.window_delta);

	// constrain mouse to window
	rect_t r = { 0 };
	gui_get_window_rect(gui->window_focused, &r, false);
	// if (gui->mouse.is_captured) {
	// 	s32 w = (r.w-r.x);
	// 	s32 h = (r.h-r.y);
	// 	s32 cx = r.x+(w/2);
	// 	s32 cy = r.y+(h/2);
	// 	gui_move_mouse(cx, cy);
	// } else {
	if (gui->mouse.is_captured) {
		gui_constrain_mouse(gui->mouse.window_pos.x, gui->mouse.window_pos.y, &r);
	}
	// }

	// evt.mouse.button.state = MOUSE_BUTTON_NON
	// evt.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
	// evt.mouse.buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
	// evt.mouse.buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
	// evt.mouse.buttons[MOUSE_BUTTON_X1].button = MOUSE_BUTTON_X1;
	// evt.mouse.buttons[MOUSE_BUTTON_X2].button = MOUSE_BUTTON_X2;
	// evt.mouse.buttons[MOUSE_BUTTON_LEFT].state =
	// 	GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	// evt.mouse.buttons[MOUSE_BUTTON_RIGHT].state =
	// 	GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	// evt.mouse.buttons[MOUSE_BUTTON_MIDDLE].state =
	// 	GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;
	// evt.mouse.buttons[MOUSE_BUTTON_X1].state =
	// 	GetAsyncKeyState(VK_XBUTTON1) & 0x8000 ? 1 : 0;
	// evt.mouse.buttons[MOUSE_BUTTON_X2].state =
	// 	GetAsyncKeyState(VK_XBUTTON2) & 0x8000 ? 1 : 0;

	gui_event_push(&evt);

	return 0;
}

LRESULT gui_process_mouse_button_win32(HWND hwnd, UINT msg, WPARAM wp,
				       LPARAM lp)
{
	gui_event_t evt;
	memset(&evt, 0, sizeof(gui_event_t));
	evt.timestamp = os_get_time_ns();
	switch (msg) {
	case WM_LBUTTONDOWN:
		evt.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
		evt.mouse.button.button = MOUSE_BUTTON_LEFT;
		evt.mouse.button.state = MOUSE_BUTTON_DOWN;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_LEFT].state = MOUSE_BUTTON_DOWN;
		break;
	case WM_LBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.button.button = MOUSE_BUTTON_LEFT;
		evt.mouse.button.state = MOUSE_BUTTON_UP;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_LEFT].state = MOUSE_BUTTON_UP;
		break;
	case WM_RBUTTONDOWN:
		evt.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
		evt.mouse.button.button = MOUSE_BUTTON_RIGHT;
		evt.mouse.button.state = MOUSE_BUTTON_DOWN;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_RIGHT].state = MOUSE_BUTTON_DOWN;
		break;
	case WM_RBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.button.button = MOUSE_BUTTON_RIGHT;
		evt.mouse.button.state = MOUSE_BUTTON_UP;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_RIGHT].state = MOUSE_BUTTON_UP;
		break;
	case WM_MBUTTONDOWN:
		evt.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
		evt.mouse.button.button = MOUSE_BUTTON_MIDDLE;
		evt.mouse.button.state = MOUSE_BUTTON_DOWN;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_MIDDLE].state = MOUSE_BUTTON_DOWN;
		break;
	case WM_MBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.button.button = MOUSE_BUTTON_MIDDLE;
		evt.mouse.button.state = MOUSE_BUTTON_UP;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
		evt.mouse.mouse.buttons[MOUSE_BUTTON_MIDDLE].state = MOUSE_BUTTON_UP;
		break;
	}

	gui_event_push(&evt);

	return 0;
}

LRESULT gui_process_window_focus_lost_win32(HWND hwnd, UINT msg, WPARAM wp,
				       LPARAM lp)
{
	gui_window_t* focused = gui_get_focused_window();
	if (!gui_is_valid_window(focused))
		return 1;

	gui_window_data_t* wd = (gui_window_data_t*)get_window_user_data(hwnd);
	if (focused == wd->window)
		gui_set_focused_window(NULL);

	return 0;
}

LRESULT gui_process_window_activate_win32(HWND hwnd, UINT msg, WPARAM wp,
				       LPARAM lp)
{
	gui_window_data_t* wd = (gui_window_data_t*)get_window_user_data(hwnd);
	if (wd == NULL)
		return 1;

	if (!gui_set_focused_window(wd->window))
		return 1;

	// gui_event_t evt;
	// memset(&evt, 0, sizeof(gui_event_t));
	// evt.type = GUI_EVENT_WINDOW_MOUSE_ACTIVATE;
	// evt.timestamp = os_get_time_ns();
	// gui_event_push(&evt);

	gui_event_t evt;
	memset(&evt, 0, sizeof(gui_event_t));
	evt.type = GUI_EVENT_WINDOW_ACTIVATE;
	evt.timestamp = os_get_time_ns();
	gui_event_push(&evt);

	return 0;
}

LRESULT gui_process_mouse_capture_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (!gui || !lp) {
		logger(LOG_DEBUG, "gui_process_mouse_capture_win32: lp == NULL");
		return 1;
	}

	logger(LOG_DEBUG, "gui_process_mouse_capture_win32: lp != NULL");

	return 0;
}

LRESULT gui_process_keyboard_win32(UINT msg, WPARAM wp, LPARAM lp)
{
	if (!gui)
		return RESULT_NULL;

	LRESULT result = 0;
	u8 key_state = 0;
	gui_event_t evt;
	memset(&evt, 0, sizeof(gui_event_t));
	evt.timestamp = os_get_time_ns();

	switch (msg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		result = 1;
		evt.type = GUI_EVENT_KEY_DOWN;
		key_state = KEY_DOWN;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
	default:
		evt.type = GUI_EVENT_KEY_UP;
		key_state = KEY_UP;
		break;
	}

	// SHORT key_state = GetKeyState(VK_LCONTROL);
	// SHORT async_key_state = GetAsyncKeyState(VK_LCONTROL);
	// gui->keyboard.key_states[]
	keyboard_scancode_t scancode = win32_virtual_key_to_scancode(wp, lp);
	gui->keyboard[scancode].scancode = scancode;
	gui->keyboard[scancode].state = key_state;
	evt.keyboard.key.scancode = scancode;
	evt.keyboard.key.state = key_state;
	evt.keyboard.mod = KEY_MOD_NONE;

	// info("Scan Code: %d", (int)scancode);
	// info("MapVirtualKey: %d", vk);
	// info("GetAsyncKeyState: %d", async_key_state);

	//     switch(wp) {
	//         case VK_LSHIFT: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_LSHIFT; break;     // 0xA0
	//         case VK_RSHIFT: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_RSHIFT; break;     // 0xA1
	//         case VK_LCONTROL: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_LCTRL; break; // 0xA2
	//         case VK_RCONTROL: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_RCTRL; break; // 0xA3
	//         case VK_LMENU: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_LMETA; break;       // 0xA4
	//         case VK_RMENU: evt.keyboard.keys[scancode].modifier = GUI_KEY_MOD_RMETA; break;       // 0xA5
	// #if(_WIN32_WINNT >= 0x0500)
	//         case VK_BROWSER_BACK: break;        // 0xA6
	//         case VK_BROWSER_FORWARD: break;     // 0xA7
	//         case VK_BROWSER_REFRESH: break;     // 0xA8
	//         case VK_BROWSER_STOP: break;        // 0xA9
	//         case VK_BROWSER_SEARCH: break;      // 0xAA
	//         case VK_BROWSER_FAVORITES: break;   // 0xAB
	//         case VK_BROWSER_HOME: break;        // 0xAC
	//         case VK_VOLUME_MUTE: break;         // 0xAD
	//         case VK_VOLUME_DOWN: break;         // 0xAE
	//         case VK_VOLUME_UP: break;           // 0xAF
	//         case VK_MEDIA_NEXT_TRACK: break;    // 0xB0
	//         case VK_MEDIA_PREV_TRACK: break;    // 0xB1
	//         case VK_MEDIA_STOP: break;          // 0xB2
	//         case VK_MEDIA_PLAY_PAUSE: break;    // 0xB3
	//         case VK_LAUNCH_MAIL: break;         // 0xB4
	//         case VK_LAUNCH_MEDIA_SELECT: break; // 0xB5
	//         case VK_LAUNCH_APP1: break;         // 0xB6
	//         case VK_LAUNCH_APP2: break;         // 0xB7
	// #endif
	//     }

	gui_event_push(&evt);

	return result;
}

void get_window_bounds_for_client_bounds(const rect_t* bounds, DWORD style,
					 DWORD ex_style, rect_t* new_bounds)
{
	RECT wr;
	wr.left = bounds->x;
	wr.top = bounds->y;
	wr.right = bounds->x + bounds->w;
	wr.bottom = bounds->y + bounds->h;
	AdjustWindowRectEx(&wr, style, FALSE, ex_style);
	new_bounds->x = max(0, wr.left);
	new_bounds->y = max(0, wr.top);
	new_bounds->w = wr.right - wr.left;
	new_bounds->h = wr.bottom - wr.top;
}

void update_window_position(gui_window_t* window)
{
	rect_t* b = &window->bounds;
	HWND hwnd = (HWND)gui->get_handle(window);
	DWORD window_style = GetWindowLong(hwnd, GWL_STYLE);
	DWORD window_style_ex = GetWindowLong(hwnd, GWL_EXSTYLE);
	rect_t new_bounds;
	get_window_bounds_for_client_bounds(b, window_style, window_style_ex,
					    &new_bounds);
	window->bounds.x = new_bounds.x;
	window->bounds.y = new_bounds.y;
	window->bounds.w = new_bounds.w;
	window->bounds.h = new_bounds.h;
	SetWindowPos(hwnd, NULL, new_bounds.x, new_bounds.y, new_bounds.w,
		     new_bounds.h, SWP_NOREPOSITION);
}

typedef HRESULT(WINAPI* DwmIsCompositionEnabledFunction)(__out BOOL* isEnabled);
typedef HRESULT(WINAPI* DwmGetWindowAttributeFunction)(__in HWND hwnd,
						       __in DWORD dwAttribute,
						       __out PVOID pvAttribute,
						       DWORD cbAttribute);

static void get_extended_bounds(HWND hwnd, RECT* ex_bounds)
{
	RECT extendedBounds;
	// DWORD resultSize;
	HINSTANCE dwmapiDllHandle = (HINSTANCE)os_dlopen("dwmapi.dll");
	if (NULL !=
	    dwmapiDllHandle) // not on Vista/Windows7 so no aero so no need to account for aero.
	{
		DwmIsCompositionEnabledFunction DwmIsCompositionEnabled;
		DwmIsCompositionEnabled =
			(DwmIsCompositionEnabledFunction)os_dlsym(
				dwmapiDllHandle, "DwmIsCompositionEnabled");
		if (NULL != DwmIsCompositionEnabled) {
			BOOL isEnabled;
			HRESULT hr = DwmIsCompositionEnabled(&isEnabled);
			DwmGetWindowAttributeFunction DwmGetWindowAttribute;
			DwmGetWindowAttribute =
				(DwmGetWindowAttributeFunction)os_dlsym(
					dwmapiDllHandle,
					"DwmGetWindowAttribute");
			hr = DwmGetWindowAttribute(hwnd,
						   DWMWA_EXTENDED_FRAME_BOUNDS,
						   &extendedBounds,
						   sizeof(RECT));
		}
	}
}

bool gui_create_window_win32(gui_window_t* window)
{
	if (!gui || !window)
		return false;

	HWND hwnd = NULL;
	HWND parent_hwnd = NULL;
	HMENU menu = NULL;
	DWORD style = GUI_WIN32_WINDOW_STYLE;
	DWORD style_ex = GUI_WIN32_WINDOW_STYLE_EX;

	if (window->parent && window->parent->data) {
		style = GUI_WIN32_WINDOW_CHILD_STYLE;
		parent_hwnd = window->parent->data->hwnd;
	}

	if (parent_hwnd == HWND_DESKTOP)
		parent_hwnd = GetDesktopWindow();

	wchar_t window_title[512];
	memset(&window_title, 0, sizeof(window_title));
	os_utf8_to_wcs(window->title, 0, window_title, sizeof(window_title));

	window->data = (gui_window_data_t*)BM_ALLOC(sizeof(gui_window_data_t));
	memset(window->data, 0, sizeof(gui_window_data_t));
	window->data->instance = g_hinstance;
	window->data->window = window;
	window->data->wndproc = NULL;
	window->data->style = style;
	window->data->style_ex = style_ex;

	RECT wr = {
		.left = window->bounds.x,
		.top = window->bounds.y,
		.right = window->bounds.w,
		.bottom = window->bounds.h,
	};

	AdjustWindowRectEx(&wr, style, FALSE, style_ex);
	hwnd = CreateWindowEx(style_ex, (LPCWSTR)g_atom, (LPCWSTR)window_title,
			      style, CW_USEDEFAULT, CW_USEDEFAULT,
			      wr.right - wr.left, wr.bottom - wr.top,
			      parent_hwnd, menu, window->data->instance,
			      window->data);
	window->bounds.x = wr.left;
	window->bounds.y = wr.top;
	window->bounds.w = wr.right - wr.left;
	window->bounds.h = wr.bottom - wr.top;
	if (window->flags & GUI_WINDOW_SHOW)
		gui->show_window(window, true);
	if (window->flags & GUI_WINDOW_CENTERED)
		gui->center_window(window);

	if (!SetProp(hwnd, TEXT("gui_window_data_t"), window->data)) {
		BM_FREE(window->data);
		return false;
	}

	return hwnd != NULL;
}

void gui_destroy_window_win32(gui_window_t* window)
{
	if (window && window->data) {
		BM_FREE((gui_window_data_t*)window->data);
		window->data = NULL;
	}
}

bool gui_is_valid_window_win32(const gui_window_t* window)
{
	if (window && window->data)
		return window->data->hwnd != NULL;
	else
		return false;
}

void gui_show_window_win32(gui_window_t* window, bool shown)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		ShowWindow(data->hwnd, (int)shown);
	}
}

void gui_set_window_pos_win32(gui_window_t* window, const rect_t* rect)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		SetWindowPos(data->hwnd, HWND_TOP, rect->x, rect->y, rect->w,
			     rect->h, SWP_NOSIZE);
	}
}

bool gui_set_focused_window_win32(const gui_window_t* window)
{
	if (gui->window_focused && !window) {
		gui_clear_key_state();
	}

	if (gui->window_focused != window) {
		// release mouse from current window
		if (gui_is_valid_window(gui->window_focused)) {
			if (gui->window_focused->flags & GUI_WINDOW_CAPTURE_MOUSE) {
				gui_capture_mouse(gui->window_focused, false);
			}
		}
		// capture mouse in new window
		if (gui_is_valid_window(window)) {
			// Left mouse pressed and mouse in window
			POINT p = { 0 };
			GetCursorPos(&p);
			rect_t wr = { 0 };
			gui_get_window_rect(window, &wr, false);
			bool mouse_in_window = ((p.x > wr.x) && (p.x < wr.w) &&
				(p.y > wr.y) && (p.y < wr.h));
			if ((window->flags & GUI_WINDOW_CAPTURE_MOUSE) &&
				(GetAsyncKeyState(VK_LBUTTON) & 0x8000) &&
				mouse_in_window) {
				gui_capture_mouse(window, true);
			}
		}
	}

	gui->window_focused = window;
	if (gui->window_focused != gui->mouse.window)
		gui->mouse.window = gui->window_focused;

	return true;
}

void rect_to_win32_rect(const rect_t* r, RECT* wr)
{
	if (r && wr) {
		wr->left = r->x;
		wr->top = r->y;
		wr->right = r->w;
		wr->bottom = r->h;
	}
}

void win32_rect_to_rect(const RECT* wr, rect_t* r)
{
	if (wr && r) {
		r->x = wr->left;
		r->y = wr->top;
		r->w = wr->right;
		r->h = wr->bottom;
	}
}

void gui_center_window_win32(gui_window_t* window)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		HWND parent_hwnd = (HWND)gui->get_handle(window->parent);
		if (parent_hwnd == HWND_DESKTOP)
			parent_hwnd = GetDesktopWindow();
		if (parent_hwnd != NULL) {
			RECT wr, wr_parent;
			GetWindowRect(parent_hwnd, &wr_parent);
			rect_to_win32_rect(&window->bounds, &wr);

			int width = wr.right - wr.left;
			int height = wr.bottom - wr.top;
			int x = ((wr_parent.right - wr_parent.left) - width) /
					2 +
				wr_parent.left;
			int y = ((wr_parent.bottom - wr_parent.top) - height) /
					2 +
				wr_parent.top;
			int screen_width = GetSystemMetrics(SM_CXSCREEN);
			int screen_height = GetSystemMetrics(SM_CYSCREEN);
			// make sure that the dialog box never moves outside of the screen
			if (x < 0)
				x = 0;
			if (y < 0)
				y = 0;
			if (x + width > screen_width)
				x = screen_width - width;
			if (y + height > screen_height)
				y = screen_height - height;
			RECT new_bounds;
			new_bounds.left = x;
			new_bounds.top = y;
			new_bounds.right = width;
			new_bounds.bottom = height;
			AdjustWindowRectEx(&new_bounds, window->data->style,
					   FALSE, window->data->style_ex);
			gui->set_window_pos(window, &new_bounds);
		}
	}
}

bool gui_get_window_size_win32(const gui_window_t* window, s32* w, s32* h, bool client)
{
	rect_t rect = { 0 };
	if (gui_get_window_rect(window, &rect, client)) {
		*w = rect.w;
		*h = rect.h;
		return true;
	} else {
		return false;
	}
}

bool gui_get_window_rect_win32(const gui_window_t* window, rect_t* rect,
			       bool client)
{
	BOOL ok = FALSE;
	RECT r;
	if (client)
		ok = GetClientRect(window->data->hwnd, &r);
	else
		ok = GetWindowRect(window->data->hwnd, &r);
	win32_rect_to_rect(&r, rect);
	return (bool)ok;
}

bool gui_get_window_centerpoint_win32(const gui_window_t* window, vec2i_t* p, bool client)
{
	rect_t r = { 0 };
	if (gui_get_window_rect(window, &r, client)) {
		rect_centerpoint(&r, p);
		return true;
	}

	return false;
}

void* gui_get_window_handle_win32(gui_window_t* window)
{
	if (window && window->data && window->data->hwnd)
		return (void*)window->data->hwnd;
	return NULL;
}

void gui_read_mouse_state_win32(mouse_t* mouse)
{
	static bool wnd_first = true;
	if (wnd_first && gui->mouse.is_captured && mouse->window && mouse->window->centerpoint.x && mouse->window->centerpoint.y) {
		gui->mouse.window_delta = vec2i_set(mouse->window->centerpoint.x, mouse->window->centerpoint.y);
		wnd_first = false;
	}

	gui->mouse.screen_pos_last = vec2i_copy(gui->mouse.screen_pos);
	gui->mouse.window_pos_last = vec2i_copy(gui->mouse.window_pos);

	// screen pos
	POINT p = {.x = 0, .y = 0};
	GetCursorPos(&p);
	vec2i_t pos = vec2i_set(p.x, p.y);
	gui->mouse.screen_pos = vec2i_copy(pos);
	vec2i_t screen_delta = vec2i_sub(gui->mouse.screen_pos, gui->mouse.screen_pos_last);
	gui->mouse.screen_delta = vec2i_add(gui->mouse.screen_pos, screen_delta);
	// window pos
	HWND hwnd = GetForegroundWindow();
	ScreenToClient(hwnd, &p);
	pos = vec2i_set(p.x, p.y);
	gui->mouse.window_pos = vec2i_copy(pos);
	if (gui->mouse.window_pos_last.x) {
		vec2i_t window_delta = vec2i_sub(gui->mouse.window_pos, gui->mouse.window_pos_last);
		gui->mouse.window_delta = vec2i_add(gui->mouse.window_delta, window_delta);
	}

	gui->mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
	gui->mouse.buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
	gui->mouse.buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
	gui->mouse.buttons[MOUSE_BUTTON_X1].button = MOUSE_BUTTON_X1;
	gui->mouse.buttons[MOUSE_BUTTON_X2].button = MOUSE_BUTTON_X2;
	gui->mouse.buttons[MOUSE_BUTTON_LEFT].state = GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	gui->mouse.buttons[MOUSE_BUTTON_RIGHT].state = GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	gui->mouse.buttons[MOUSE_BUTTON_MIDDLE].state = GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;
	gui->mouse.buttons[MOUSE_BUTTON_X1].state = GetAsyncKeyState(VK_XBUTTON1) & 0x8000 ? 1 : 0;
	gui->mouse.buttons[MOUSE_BUTTON_X2].state = GetAsyncKeyState(VK_XBUTTON2) & 0x8000 ? 1 : 0;

	// vec2f_t screen_delta = { 0 };
	// vec2f_t window_delta = { 0 };
	// POINT sp = { 0 };
	// POINT wp = { 0 };
	// GetCursorPos(&sp);
	// vec2f_t screen_pos = vec2_set((f32)sp.x, (f32)sp.y);
	// mouse->screen_delta = vec2_sub(mouse->screen_pos, screen_pos);
	// mouse->screen_pos = vec2_copy(screen_pos);
	// HWND hwnd = GetForegroundWindow();
	// if (mouse->window) {
	// 	hwnd = mouse->window->data->hwnd;
	// }
	// ScreenToClient(hwnd, &wp); // FIXME: Need to determine the foreground window according to our GUI system!
	// vec2f_t window_pos = vec2_set((f32)wp.x, (f32)wp.y);
	// mouse->window_delta = vec2_sub(mouse->window_pos, window_pos);
	// mouse->window_pos = vec2_copy(window_pos);

	// if (mouse->mode == MOUSE_MODE_RELATIVE) {
	// 	screen_delta = vec2_copy(screen_pos);
	// 	screen_pos = vec2_add(mouse->scr_last, screen_delta);
	// 	mouse->screen_pos = vec2_add(mouse->screen_pos, screen_delta);
	// } else {
	// 	screen_delta = vec2_copy(vec2_sub(screen_pos, mouse->scr_last));
	// 	mouse->screen_pos = vec2_copy(screen_pos);
	// }

	// if (mouse->window && ((mouse->window->flags & GUI_WINDOW_CAPTURE_MOUSE) == 0)) {
	// 	int x_max = 0, y_max = 0;
	// 	rect_t wr = { 0, 0, 1920, 1080 };
	// 	// gui_get_window_rect(mouse->window, &wr, true);
	// 	x_max = wr.w - 1;
	// 	y_max = wr.h - 1;
	// 	if (mouse->screen_pos.x > x_max)
	// 		mouse->screen_pos.x = x_max;
	// 	if (mouse->screen_pos.x < 0)
	// 		mouse->screen_pos.x = 0;
	// 	if (mouse->screen_pos.y > y_max)
	// 		mouse->screen_pos.y = y_max;
	// 	if (mouse->screen_pos.y < 0)
	// 		mouse->screen_pos.y = 0;
	// }

	// mouse->screen_delta = vec2_add(mouse->screen_delta, screen_delta);

	// if (mouse->mode == MOUSE_MODE_RELATIVE) {
	// 	mouse->scr_last = vec2_copy(mouse->screen_pos);
	// } else {
	// 	mouse->scr_last = vec2_copy(screen_pos);
	// }

	// 	get_scaled_mouse_delta(mouse->relative_speed_scale, x, &mouse->scale_accum.x);
	// 	get_scaled_mouse_delta(mouse->relative_speed_scale, y, &mouse->scale_accum.y);
    //     // if (mouse->relative_mode) {
    //     //     x = GetScaledMouseDelta(mouse->relative_speed_scale, x, &mouse->scale_accum_x);
    //     //     y = GetScaledMouseDelta(mouse->relative_speed_scale, y, &mouse->scale_accum_y);
    //     // } else {
    //     //     x = GetScaledMouseDelta(mouse->normal_speed_scale, x, &mouse->scale_accum_x);
    //     //     y = GetScaledMouseDelta(mouse->normal_speed_scale, y, &mouse->scale_accum_y);
    //     // }
    //     xrel = x;
    //     yrel = y;
    //     x = (mouse->last_x + xrel);
    //     y = (mouse->last_y + yrel);
    // } else {
    //     xrel = x - mouse->last_x;
    //     yrel = y - mouse->last_y;
    // }
}

bool gui_set_mouse_mode_win32(enum mouse_mode mode)
{
	return true;
}

bool gui_capture_mouse_win32(gui_window_t* window, bool captured)
{
	bool res = false;

	if (gui_is_valid_window(window)) {
		if (captured && !gui->mouse.is_captured) {
			rect_t r = { 0 };
			gui_get_window_rect(window, &r, false);
			rect_centerpoint(&r, &window->centerpoint);

			gui_move_mouse(window->centerpoint.x, window->centerpoint.y);

			RECT wr = { 0 };
			rect_to_win32_rect(&r, &wr);
			ClipCursor(&wr);

			gui_show_mouse(false);

			HWND prev_hwnd = SetCapture(window->data->hwnd);
			logger(LOG_DEBUG, "SetCapture");
			(void)prev_hwnd;

			gui_event_t evt;
			memset(&evt, 0, sizeof(gui_event_t));
			evt.type = GUI_EVENT_MOUSE_CAPTURE;
			evt.timestamp = os_get_time_ns();
			gui_event_push(&evt);
			gui->mouse.window = window;
			gui->mouse.is_captured = true;
		} else if (gui->mouse.is_captured) {
			gui_show_mouse(true);
			res = ReleaseCapture(window->data->hwnd);
			logger(LOG_DEBUG, "ReleaseCapture");
			gui->mouse.is_captured = false;
		}
	}

	return gui->mouse.is_captured;
}

bool gui_move_mouse_win32(s32 x, s32 y)
{
	return SetCursorPos(x, y);
}

bool gui_show_mouse_win32(bool shown)
{
	if (ShowCursor(shown)) {
		return true;
	}
	return false;
}

/* Win32 Utility Functions */
keyboard_scancode_t win32_virtual_key_to_scancode(WPARAM wp, LPARAM lp)
{
	keyboard_scancode_t scancode = SCANCODE_NONE;
	int vk = (int)wp;
	int sc = ((UINT)lp & 0x00ff0000) >> 16u;
	int s2vk = (int)MapVirtualKey(sc, MAPVK_VSC_TO_VK_EX);
	// assert(s2vk == vk);
	bool is_extended = (lp & (1 << 24)) != 0;
	// logger(LOG_DEBUG,  "SCANCODE: %d VK: %d MapVirtualKey: %d Extended: %s",
	//     sc, vk, s2vk, is_extended ? "true":"false");
	if (sc <= 127)
		scancode = win32_scancode_lut[sc];
	else
		scancode = 0xfdfdfdfd;
	// logger(LOG_DEBUG,  "SCAN: %d, SC: %d VK: %d MapVirtualKey: %d Extended: %s",
	//     scancode, sc, vk, s2vk, is_extended ? "true":"false");

	// switch (vk) {
	//     case
	// }
	// int sc = (lp >> 16) & 0xFF;
	// assert(sc == scancode);
	// // assert(vk == vkey);
	// keyboard_scancode_t code;
	return scancode;
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

HMODULE get_module_from_wndproc(WNDPROC wp)
{
	HMODULE instance = NULL;
	void* address = (void*)wp;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
				  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			  (LPCWSTR)address, &instance);
	return instance;
}
