#include "gui/gui.h"
#include "platform/keyboard-scancode.h"

#include "core/logger.h"
#include "core/types.h"
#include "core/memory.h"

#define MAX_GUI_EVENTS 65535

gui_system_t* gui = NULL;

result gui_init(enum gui_backend_t backend)
{
	logger(LOG_INFO, "gui_init: Initializing GUI subsystem...");

	if (gui)
		gui_shutdown();

	gui = (gui_system_t*)BM_ALLOC(sizeof(*gui));
	memset(gui, 0, sizeof(*gui));
	vec_init(gui->windows);
	vec_init(gui->events);
	gui->window_focused = NULL;
	gui->backend = backend;
	if (backend == GUI_BACKEND_SDL2) {
#ifdef BM_USE_SDL2
		return gui_init_sdl2(gui);
#endif
	} else {
#if defined(_WIN32)
		return gui_init_win32(gui);
#elif defined(__APPLE__)
		return gui_init_macos(gui);
#elif defined(__linux__)
		return gui_init_linux(gui);
#endif
	}

	return RESULT_NOT_IMPL;
}

void gui_refresh(void)
{
	if (gui->backend == GUI_BACKEND_SDL2) {
#ifdef BM_USE_SDL2
		gui_refresh_sdl2(gui);
#endif
	} else {
#if defined(_WIN32)
		gui_refresh_win32(gui);
#elif defined(__APPLE__)
		gui_init_macos(gui);
#elif defined(__linux__)
		gui_init_linux(gui);
#endif
	}
}

void gui_shutdown(void)
{
	if (gui) {
		for (size_t i = 0; i < gui->windows.num_elems; i++) {
			gui_window_t* window =
				(gui_window_t*)gui->windows.elems[i];
			gui_destroy_window(window);
		}
		vec_free(gui->windows);
		vec_free(gui->events);
		BM_FREE(gui);
		gui = NULL;
	}
}

bool gui_event_push(const gui_event_t* event)
{
	if (gui->events.num_elems >= MAX_GUI_EVENTS)
		return false;
	vec_push_back(gui->events, event);
	return true;
}

bool gui_event_pop(gui_event_t* event)
{
	if (!gui || gui->events.num_elems == 0)
		return false;

	// logger(LOG_DEBUG, "events: %zu\n", gui->events.num_elems);
	gui_event_t* evt = (gui_event_t*)vec_begin(gui->events);
	if (evt == NULL)
		return false;
	memcpy(event, evt, sizeof(gui_event_t));
	vec_erase(gui->events, 0);
	return true;
}

mouse_t* gui_get_mouse()
{
	return &gui->mouse;
}

bool gui_set_mouse_mode(enum mouse_mode mode)
{
	if (gui) {
		gui->mouse.mode = mode;
		return gui->set_mouse_mode(mode);
	} else {
		return false;
	}
}

gui_display_t* gui_create_display(s32 index)
{
	gui_display_t* display = NULL;
	return display;
}

void gui_destroy_display(gui_display_t* display) {}

gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w, s32 h,
				s32 flags, gui_window_t* parent)
{
	if (!gui)
		return NULL;

	gui_window_t* window = (gui_window_t*)BM_ALLOC(sizeof(*window));
	memset(window, 0, sizeof(*window));

	size_t title_len = strlen(title);
	window->title = BM_REALLOC(window->title, title_len + 1);
	memset(window->title, 0, title_len + 1);
	strncpy(window->title, title, title_len);
	if (title[title_len] != '\0')
		window->title[title_len] = '\0';
	window->flags = flags;
	window->bounds.x = x;
	window->bounds.y = x;
	window->bounds.w = w;
	window->bounds.h = h;
	window->parent = parent;
	if (GUI_WINDOW_POS_IS_UNDEFINED(x) || GUI_WINDOW_POS_IS_UNDEFINED(y)) {
	}

	window->destroy_me = false;

	if (!gui->create_window(window))
		gui->destroy_window(window);

	vec_push_back(gui->windows, &window);

	return window;
}

void gui_destroy_window(gui_window_t* window)
{
	if (!gui || !window)
		return;

	window->destroy_me = true;
	if (window->title) {
		BM_FREE(window->title);
		window->title = NULL;
	}

	gui->destroy_window(window);
	BM_FREE(window);
	window = NULL;
}

bool gui_is_valid_window(const gui_window_t* window)
{
	if (gui)
		return gui->is_valid_window(window);
	else
		return false;
}

void gui_show_window(gui_window_t* window, bool shown)
{
	if (!gui || !window)
		return;
	gui->show_window(window, shown);
}

void gui_set_window_pos(gui_window_t* window, const rect_t* rect)
{
	if (!gui || !window || !rect)
		return;
	gui->set_window_pos(window, rect);
}

void gui_center_window(gui_window_t* window)
{
	if (!gui || !window)
		return;
	gui->center_window(window);
}

bool gui_get_window_size(const gui_window_t* window, s32* w, s32* h,
			 bool client)
{
	if (!gui || !window || !w || !h)
		return false;
	return gui->get_window_size(window, w, h, client);
}

bool gui_get_window_rect(const gui_window_t* window, rect_t* rect, bool client)
{
	if (!gui || !window || !rect)
		return false;
	return gui->get_window_rect(window, rect, client);
}

bool gui_get_window_centerpoint(const gui_window_t* window, vec2i_t* p,
				bool client)
{
	rect_t r = {0};
	if (gui_get_window_rect(window, &r, client)) {
		rect_centerpoint(&r, p);
		return true;
	}

	return false;
}

void* gui_get_window_handle(gui_window_t* window)
{
	if (window && window->data)
		return gui->get_handle(window);
	return NULL;
}

gui_window_t* gui_get_window_by_handle(void* handle)
{
	if (!gui)
		return NULL;
	gui_window_t* window = NULL;
	for (size_t i = 0; i < gui->windows.num_elems; i++) {
		window = gui->windows.elems[i];
		if (window) {
			void* hnd = gui->get_handle(window);
			if (hnd == handle)
				break;
		}
	}
	return window;
}

gui_window_t* gui_get_focused_window()
{
	if (gui)
		return gui->window_focused;
	else
		return NULL;
}

bool gui_set_focused_window(gui_window_t* window)
{
	if (gui)
		return gui->focus_window(window);
	else
		return false;
}

void gui_clear_key_state()
{
	if (!gui)
		return;
	for (size_t i = 0; i < SCANCODE_MAX; i++)
		gui->keyboard[i].state = KEY_UP;
}

void gui_read_mouse_state(mouse_t* mouse)
{
	if (!gui)
		return;
	gui->read_mouse_state(&gui->mouse);
	if (mouse) {
		memcpy(mouse, &gui->mouse, sizeof(mouse_t));
	}
}

bool gui_capture_mouse(gui_window_t* window, bool captured)
{
	if (!gui)
		return false;
	return gui->capture_mouse(window, captured);
}

bool gui_move_mouse(s32 x, s32 y)
{
	if (!gui)
		return false;
	return gui->move_mouse(x, y);
}

bool gui_show_mouse(bool shown)
{
	if (!gui)
		return false;
	return gui->show_mouse(shown);
}

bool gui_constrain_mouse(s32 curr_x, s32 curr_y, const rect_t* r)
{
	if (!r)
		return false;
	s32 w = (r->w - r->x);
	s32 h = (r->h - r->y);
	s32 cx = r->x + (w / 2);
	s32 cy = r->y + (h / 2);
	s32 max_x = w;
	s32 min_x = cx;
	s32 max_y = h;
	s32 min_y = cy;
	if ((s32)curr_x >= max_x - 1)
		gui_move_mouse(r->x + 1, r->y + curr_y);
	if ((s32)curr_x <= 0)
		gui_move_mouse(r->w - 1, r->y + curr_y);
	if ((s32)curr_y >= max_y - 1)
		gui_move_mouse(r->x + curr_x, r->y + 1);
	if ((s32)curr_y <= 0)
		gui_move_mouse(r->x + curr_x, r->h - 1);
	return true;
}
