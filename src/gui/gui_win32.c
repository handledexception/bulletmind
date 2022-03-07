#include "core/types.h"
#include "core/memory.h"
#include "core/logger.h"

#include "platform/platform.h"

#include "gui/gui.h"
#include "gui/gui_vkey_win32.h"

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

gui_scancode_t win32_virtual_key_to_scancode(WPARAM wp, LPARAM lp)
{
    gui_scancode_t scancode = GUI_SCANCODE_NONE;
    int vk = (int)wp;
    int sc = ((UINT)lp & 0x00ff0000) >> 16u;
    int s2vk = (int)MapVirtualKey(sc, MAPVK_VSC_TO_VK_EX);
    // assert(s2vk == vk);
    bool is_extended = (lp & (1 << 24)) != 0;
    logger(LOG_DEBUG,  "SCANCODE: %d VK: %d MapVirtualKey: %d Extended: %s",
        sc, vk, s2vk, is_extended ? "true":"false");
    if (sc <= 127)
        scancode = win32_scancode_lut[sc];
    else
        scancode = 0xfdfdfdfd;
    logger(LOG_DEBUG,  "SCAN: %d, SC: %d VK: %d MapVirtualKey: %d Extended: %s",
        scancode, sc, vk, s2vk, is_extended ? "true":"false");
    // switch (vk) {
    //     case 
    // }
    // int sc = (lp >> 16) & 0xFF;
    // assert(sc == scancode);
    // // assert(vk == vkey);
    // gui_scancode_t code;
    return scancode;
}

LRESULT gui_process_keyboard_msg_win32(UINT msg, WPARAM wp, LPARAM lp)
{
    if (!gui)
        return RESULT_NULL;

    LRESULT result = 0;
    u8 key_state = 0;
    gui_event_t evt;
    memset(&evt, 0, sizeof(gui_event_t));

    switch (msg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        result = 1;
        key_state = GUI_KEY_DOWN;
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
    default:
        key_state = GUI_KEY_UP;
        break;
    }

    // SHORT key_state = GetKeyState(VK_LCONTROL);
    // SHORT async_key_state = GetAsyncKeyState(VK_LCONTROL);
    // gui->keyboard.key_states[]
    gui_scancode_t scancode = win32_virtual_key_to_scancode(wp, lp);
    evt.keyboard.keys[scancode].state = key_state;
    evt.keyboard.keys[scancode].scancode = scancode;
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

    if (key_state == GUI_KEY_UP) {
    }

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
        gui_clear_key_state(&gui->keyboard.key_states[0]);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        return gui_process_keyboard_msg_win32(msg, wp, lp);
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

    window->data = (gui_window_data_t*)mem_alloc(sizeof(gui_window_data_t));
    memset(window->data, 0, sizeof(gui_window_data_t));
    window->data->instance = g_hinstance;
    window->data->window = window;
    window->data->wndproc = NULL;

    hwnd = CreateWindowEx(style_ex, (LPCWSTR)g_atom, (LPCWSTR)window_title,
                  style, window->x, window->y, window->w, window->h,
                  parent_hwnd, menu, window->data->instance,
                  window->data);

    gui->show_window(window, true);

    return true;
}

void gui_destroy_window_win32(gui_window_t* window)
{
    if (window) {
        if (window->data) {
            mem_free((gui_window_data_t*)window->data);
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

void* gui_get_window_handle_win32(gui_window_t* window)
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

gui_scancode_t convert_scancode(void* sc)
{
    // switch(sc) {

    // }
}
