#include "core/types.h"
#include "core/memory.h"
#include "core/logger.h"

#include "platform/platform.h"

#include "gui/gui.h"
#include "platform/keyboard-vkey-win32.h"

#include <Windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define GUI_WIN32_CLASS_STYLE CS_DBLCLKS
#define GUI_WIN32_WINDOW_STYLE (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)
#define GUI_WIN32_WINDOW_CHILD_STYLE \
	(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS)
#define GUI_WIN32_WINDOW_STYLE_EX                    \
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

LRESULT gui_process_mouse_move_win32(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	POINT pos = {.x = 0, .y = 0};
	POINT win_pos = {.x = 0, .y = 0};
	GetCursorPos(&pos);
	gui->mouse.screen_pos.x = pos.x;
	gui->mouse.screen_pos.y = pos.y;
	ScreenToClient(hwnd, &pos);
	gui->mouse.window_pos.x = pos.x;
	gui->mouse.window_pos.y = pos.y;
	// gui->mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
	// gui->mouse.buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
	// gui->mouse.buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
	// gui->mouse.buttons[MOUSE_BUTTON_X1].button = MOUSE_BUTTON_X1;
	// gui->mouse.buttons[MOUSE_BUTTON_X2].button = MOUSE_BUTTON_X2;
	// gui->mouse.buttons[MOUSE_BUTTON_LEFT].state =
	// 	GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	// gui->mouse.buttons[MOUSE_BUTTON_RIGHT].state =
	// 	GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	// gui->mouse.buttons[MOUSE_BUTTON_MIDDLE].state =
	// 	GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;
	// gui->mouse.buttons[MOUSE_BUTTON_X1].state =
	// 	GetAsyncKeyState(VK_XBUTTON1) & 0x8000 ? 1 : 0;
	// gui->mouse.buttons[MOUSE_BUTTON_X2].state =
	// 	GetAsyncKeyState(VK_XBUTTON2) & 0x8000 ? 1 : 0;

	gui_event_t evt;
	memset(&evt, 0, sizeof(gui_event_t));
	evt.timestamp = os_get_time_ns();
	evt.type = GUI_EVENT_MOUSE_MOTION;
	evt.mouse.screen_pos.x = gui->mouse.screen_pos.x;
	evt.mouse.screen_pos.y = gui->mouse.screen_pos.y;
	evt.mouse.window_pos.x = gui->mouse.window_pos.x;
	evt.mouse.window_pos.y = gui->mouse.window_pos.y;
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
	vec_push_back(gui->events, &evt);

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
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].state = MOUSE_BUTTON_DOWN;
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		break;
	case WM_LBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].state = MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		break;
	case WM_RBUTTONDOWN:
		evt.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].state = MOUSE_BUTTON_DOWN;
		evt.mouse.buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
		break;
	case WM_RBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_RIGHT].state = MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_RIGHT].button =
			MOUSE_BUTTON_RIGHT;
		break;
	case WM_MBUTTONDOWN:
		evt.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
		evt.mouse.buttons[MOUSE_BUTTON_RIGHT].state = MOUSE_BUTTON_DOWN;
		evt.mouse.buttons[MOUSE_BUTTON_RIGHT].button =
			MOUSE_BUTTON_RIGHT;
		break;
	case WM_MBUTTONUP:
		evt.type = GUI_EVENT_MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_MIDDLE].state = MOUSE_BUTTON_UP;
		evt.mouse.buttons[MOUSE_BUTTON_MIDDLE].button =
			MOUSE_BUTTON_MIDDLE;
		break;
	}
	vec_push_back(gui->events, &evt);
	return 0;
}

LRESULT gui_process_mouse_activate_win32(UINT msg, WPARAM wp, LPARAM lp)
{
	return 0;
}

LRESULT gui_process_mouse_capture_win32(UINT msg, WPARAM wp, LPARAM lp)
{
	if (!gui || !lp)
		return 1;
	HWND captured = (HWND)lp;
	gui->mouse.window = gui_get_window_by_handle((void*)captured);
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

	vec_push_back(gui->events, &evt);

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
	case WM_ACTIVATEAPP:
		gui_clear_key_state();
		break;
	case WM_MOUSEMOVE:
		return gui_process_mouse_move_win32(hwnd, msg, wp, lp);
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		return gui_process_mouse_button_win32(hwnd, msg, wp, lp);
	// case WM_CAPTURECHANGED:
	//     return gui_process_mouse_capture_win32(msg, wp, lp);
	case WM_MOUSEACTIVATE:
		return gui_process_mouse_activate_win32(msg, wp, lp);
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
		parent_hwnd = get_window_to_parent_to(false);
	else if (parent_hwnd == GetDesktopWindow())
		parent_hwnd = get_window_to_parent_to(true);

	wchar_t window_title[512];
	memset(&window_title, 0, sizeof(window_title));
	os_utf8_to_wcs(window->title, 0, window_title, sizeof(window_title));

	window->data = (gui_window_data_t*)MEM_ALLOC(sizeof(gui_window_data_t));
	memset(window->data, 0, sizeof(gui_window_data_t));
	window->data->instance = g_hinstance;
	window->data->window = window;
	window->data->wndproc = NULL;

	RECT wr = {
		.left = 0, .top = 0, .right = window->w, .bottom = window->h};
	AdjustWindowRectEx(&wr, style, FALSE, style_ex);
	hwnd = CreateWindowEx(style_ex, (LPCWSTR)g_atom, (LPCWSTR)window_title,
			      style, wr.left, wr.top, wr.right - wr.left,
			      wr.bottom - wr.top, parent_hwnd, menu,
			      window->data->instance, window->data);
	gui->set_window_pos(window, window->x, window->y);
	gui->show_window(window, true);

	return true;
}

void gui_destroy_window_win32(gui_window_t* window)
{
	if (window) {
		if (window->data) {
			BM_MEM_FREE((gui_window_data_t*)window->data);
			window->data = NULL;
		}
	}
}

void gui_show_window_win32(gui_window_t* window, bool shown)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		ShowWindow(data->hwnd, (int)shown);
	}
}

void gui_set_window_pos_win32(gui_window_t* window, s32 cx, s32 cy)
{
	if (window) {
		gui_window_data_t* data = (gui_window_data_t*)window->data;
		SetWindowPos(data->hwnd, HWND_TOP, cx, cy, window->w, window->h,
			     0);
	}
}

void* gui_get_window_handle_win32(gui_window_t* window)
{
	return (void*)window->data->hwnd;
}

void gui_get_global_mouse_state_win32(struct mouse_device* mouse)
{
	POINT pos = {.x = 0, .y = 0};
	GetCursorPos(&pos);
	mouse->screen_pos.x = pos.x;
	mouse->screen_pos.y = pos.y;
	ScreenToClient(GetForegroundWindow(), &pos);
	mouse->window_pos.x = pos.x;
	mouse->window_pos.y = pos.y;
	mouse->buttons[MOUSE_BUTTON_LEFT].button = MOUSE_BUTTON_LEFT;
	mouse->buttons[MOUSE_BUTTON_RIGHT].button = MOUSE_BUTTON_RIGHT;
	mouse->buttons[MOUSE_BUTTON_MIDDLE].button = MOUSE_BUTTON_MIDDLE;
	mouse->buttons[MOUSE_BUTTON_X1].button = MOUSE_BUTTON_X1;
	mouse->buttons[MOUSE_BUTTON_X2].button = MOUSE_BUTTON_X2;
	mouse->buttons[MOUSE_BUTTON_LEFT].state =
		GetAsyncKeyState(VK_LBUTTON) & 0x8000 ? 1 : 0;
	mouse->buttons[MOUSE_BUTTON_RIGHT].state =
		GetAsyncKeyState(VK_RBUTTON) & 0x8000 ? 1 : 0;
	mouse->buttons[MOUSE_BUTTON_MIDDLE].state =
		GetAsyncKeyState(VK_MBUTTON) & 0x8000 ? 1 : 0;
	mouse->buttons[MOUSE_BUTTON_X1].state =
		GetAsyncKeyState(VK_XBUTTON1) & 0x8000 ? 1 : 0;
	mouse->buttons[MOUSE_BUTTON_X2].state =
		GetAsyncKeyState(VK_XBUTTON2) & 0x8000 ? 1 : 0;
}

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
	gp->show_window = gui_show_window_win32;
	gp->set_window_pos = gui_set_window_pos_win32;
	gp->get_handle = gui_get_window_handle_win32;
	gp->get_global_mouse_state = gui_get_global_mouse_state_win32;

	return RESULT_OK;
}

void gui_refresh_win32(gui_system_t* gp)
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

keyboard_scancode_t convert_scancode(void* sc)
{
	return SCANCODE_NONE;
	// switch(sc) {

	// }
}
