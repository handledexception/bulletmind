#include "gui/gui.h"
#include "platform/platform.h"
#include <SDL.h>
#include <SDL_syswm.h>
#define SDL_FLAGS                                                             \
	(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | \
	 SDL_INIT_GAMECONTROLLER)
#if defined(_WIN32)
#include <Windows.h>
#endif

#if defined(BM_USE_CIMGUI)
#include "cimgui.h"
#include "cimgui_impl.h"
#endif

struct gui_window_data {
	gui_window_t* window;
	SDL_Window* sdl_window;
};

bool gui_create_window_sdl2(gui_window_t* window);
void gui_destroy_window_sdl2(gui_window_t* window);
bool gui_is_valid_window_sdl2(const gui_window_t* window);
void gui_show_window_sdl2(gui_window_t* window, bool shown);
void gui_set_window_pos_sdl2(gui_window_t* window, const rect_t* rect);
bool gui_set_focused_window_sdl2(gui_window_t* window);
void gui_center_window_sdl2(gui_window_t* window);
bool gui_get_window_size_sdl2(const gui_window_t* window, s32* w, s32* h,
			      bool client);
bool gui_get_window_rect_sdl2(const gui_window_t* window, rect_t* rect,
			      bool client);
void* gui_get_window_handle_sdl2(gui_window_t* window);
void gui_read_mouse_state_sdl2(mouse_t* mouse);
bool gui_set_mouse_mode_sdl2(enum mouse_mode mode);
bool gui_capture_mouse_sdl2(gui_window_t* window, bool captured);
bool gui_move_mouse_sdl2(s32 x, s32 y);
bool gui_show_mouse_sdl2(bool shown);

enum mouse_button sdl_mouse_button_to_button(u8 button)
{
	enum mouse_button b = MOUSE_BUTTON_NONE;
	switch (button) {
	case 1:
		b = MOUSE_BUTTON_LEFT;
		break;
	case 2:
		b = MOUSE_BUTTON_MIDDLE;
		break;
	case 3:
		b = MOUSE_BUTTON_RIGHT;
		break;
	case 4:
		b = MOUSE_BUTTON_X1;
		break;
	case 5:
		b = MOUSE_BUTTON_X2;
		break;
	}
	return b;
}

result gui_init_sdl2(gui_system_t* gs)
{
	if (SDL_Init(SDL_FLAGS) != 0)
		return RESULT_ERROR;
	/* function pointerz */
	gs->create_window = gui_create_window_sdl2;
	gs->destroy_window = gui_destroy_window_sdl2;
	gs->is_valid_window = gui_is_valid_window_sdl2;
	gs->show_window = gui_show_window_sdl2;
	gs->set_window_pos = gui_set_window_pos_sdl2;
	gs->focus_window = gui_set_focused_window_sdl2;
	// gs->center_window = gui_center_window_sdl2;
	// gs->get_window_size = gui_get_window_size_sdl2;
	gs->get_window_rect = gui_get_window_rect_sdl2;
	gs->get_handle = gui_get_window_handle_sdl2;
	gs->read_mouse_state = gui_read_mouse_state_sdl2;
	gs->set_mouse_mode = gui_set_mouse_mode_sdl2;
	gs->capture_mouse = gui_capture_mouse_sdl2;
	gs->move_mouse = gui_move_mouse_sdl2;
	gs->show_mouse = gui_show_mouse_sdl2;

	return RESULT_OK;
}

void gui_refresh_sdl2(gui_system_t* gs)
{
	SDL_Event evt = {0};
	while (SDL_PollEvent(&evt)) {
#if defined(BM_USE_CIMGUI)
		ImGui_ImplSDL2_ProcessEvent(&evt);
#endif
		gui_event_t ge = {0};
		ge.timestamp = os_get_time_ns();
		switch (evt.type) {
		case SDL_QUIT:
			ge.type = GUI_EVENT_EXIT;
			break;
		case SDL_KEYUP:
			ge.type = GUI_EVENT_KEY_UP;
			ge.keyboard.key.scancode = evt.key.keysym.scancode;
			ge.keyboard.key.state = KEY_UP;
			ge.keyboard.mod = KEY_MOD_NONE;
			break;
		case SDL_KEYDOWN:
			ge.type = GUI_EVENT_KEY_DOWN;
			ge.keyboard.key.scancode = evt.key.keysym.scancode;
			ge.keyboard.key.state = KEY_DOWN;
			ge.keyboard.mod = KEY_MOD_NONE;
			break;
		case SDL_MOUSEBUTTONDOWN:
			ge.type = GUI_EVENT_MOUSE_BUTTON_DOWN;
			ge.mouse.button.button =
				sdl_mouse_button_to_button(evt.button.button);
			ge.mouse.button.state = MOUSE_BUTTON_DOWN;
			break;
		case SDL_MOUSEBUTTONUP:
			ge.type = GUI_EVENT_MOUSE_BUTTON_UP;
			ge.mouse.button.button =
				sdl_mouse_button_to_button(evt.button.button);
			ge.mouse.button.state = MOUSE_BUTTON_UP;
			break;
		case SDL_WINDOWEVENT: {
			if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
				ge.type = GUI_EVENT_WINDOW_SIZE;
				ge.window_width = evt.window.data1;
				ge.window_height = evt.window.data2;
			}
			break;
		}
		}

		gui_event_push(&ge);
	}
}

bool gui_create_window_sdl2(gui_window_t* window)
{
	if (!gui || !window)
		return false;
	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_CAPTURE |
		    SDL_WINDOW_RESIZABLE;
	window->data = (gui_window_data_t*)BM_ALLOC(sizeof(gui_window_data_t));
	memset(window->data, 0, sizeof(gui_window_data_t));
	window->data->sdl_window = SDL_CreateWindow(
		window->title, window->bounds.x, window->bounds.y,
		window->bounds.w, window->bounds.h, flags);
	if (window->data->sdl_window == NULL)
		return false;
	if (window->flags & GUI_WINDOW_CENTERED) {
		SDL_SetWindowPosition(window->data->sdl_window,
				      SDL_WINDOWPOS_CENTERED,
				      SDL_WINDOWPOS_CENTERED);
	}
	gui_set_focused_window(window);
	return true;
}

void gui_destroy_window_sdl2(gui_window_t* window)
{
	if (window && window->data && window->data->sdl_window) {
		SDL_DestroyWindow(window->data->sdl_window);
		window->data->sdl_window = NULL;
		BM_FREE((gui_window_data_t*)window->data);
		window->data = NULL;
	}
}

bool gui_is_valid_window_sdl2(const gui_window_t* window)
{
	if (window && window->data)
		return window->data->sdl_window != NULL;
	else
		return false;
}

void gui_show_window_sdl2(gui_window_t* window, bool shown)
{
	if (gui_is_valid_window(window)) {
		if (shown)
			SDL_ShowWindow(window->data->sdl_window);
		else
			SDL_HideWindow(window->data->sdl_window);
	}
}

void gui_set_window_pos_sdl2(gui_window_t* window, const rect_t* rect)
{
	if (gui_is_valid_window(window))
		SDL_SetWindowPosition(window->data->sdl_window, rect->x,
				      rect->y);
}

bool gui_set_focused_window_sdl2(const gui_window_t* window)
{
	if (window && window->data && window->data->sdl_window) {
		SDL_RaiseWindow(window->data->sdl_window);
		return true;
	} else {
		return false;
	}
}

// void gui_center_window_sdl2(gui_window_t* window);
// bool gui_get_window_size_sdl2(const gui_window_t* window, s32* w, s32* h, bool client);
bool gui_get_window_rect_sdl2(const gui_window_t* window, rect_t* rect,
			      bool client)
{
	(void)client;
	if (gui_is_valid_window(window) && rect) {
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		SDL_GetWindowSize(window->data->sdl_window, &w, &h);
		if (!client) {
			//FIXME: how to get the entire SDL window size?
			SDL_GetWindowPosition(window->data->sdl_window, &x, &y);
			w += x;
			h += x;
		}

		rect->x = x;
		rect->y = y;
		rect->w = w;
		rect->h = h;
		return true;
	} else {
		return false;
	}
}

void* gui_get_window_handle_sdl2(gui_window_t* window)
{
	if (gui_is_valid_window(window)) {
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window->data->sdl_window, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;
		return (void*)hwnd;
	} else {
		return NULL;
	}
}

void gui_read_mouse_state_sdl2(mouse_t* mouse)
{
	SDL_GetMouseState(&mouse->window_pos.x, &mouse->window_pos.y);
	SDL_GetGlobalMouseState(&mouse->screen_pos.x, &mouse->screen_pos.y);
	SDL_GetRelativeMouseState(&mouse->relative.x, &mouse->relative.y);
}

bool gui_set_mouse_mode_sdl2(enum mouse_mode mode)
{
	if (gui) {
		return (bool)SDL_SetRelativeMouseMode(mode ==
						      MOUSE_MODE_RELATIVE);
	} else {
		return false;
	}
}

bool gui_capture_mouse_sdl2(gui_window_t* window, bool captured)
{
	if (gui) {
		gui->mouse.window = captured ? window : NULL;
	}
	return SDL_CaptureMouse(captured) == 0;
}

bool gui_move_mouse_sdl2(s32 x, s32 y)
{
	if (gui && gui->mouse.window && gui->mouse.window->data) {
		SDL_WarpMouseInWindow(gui->mouse.window->data->sdl_window, x,
				      y);
		return true;
	} else {
		return false;
	}
}

bool gui_show_mouse_sdl2(bool shown)
{
	int res = SDL_ShowCursor(shown);
	return res == SDL_ENABLE || res == SDL_DISABLE;
}

SDL_Window* gui_get_sdl_window_from_window(const gui_window_t* window)
{
	if (window && window->data)
		return window->data->sdl_window;
	else
		return NULL;
}
