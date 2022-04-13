#include "gui/gui.h"
#include "platform/keyboard-scancode.h"

#include "core/logger.h"
#include "core/types.h"
#include "core/memory.h"

gui_platform_t* gui = NULL;

result gui_init(void)
{
	logger(LOG_INFO, "gui_init: Initializing GUI subsystem...");

	if (gui)
		gui_shutdown();

	gui = (gui_platform_t*)mem_alloc(sizeof(*gui));
	memset(gui, 0, sizeof(*gui));
	vec_init(gui->events);

#if defined(_WIN32)
	return gui_init_win32(gui);
#elif defined(__APPLE__)
	return gui_init_macos(gui);
#elif defined(__linux__)
	return gui_init_linux(gui);
#endif

	return RESULT_NOT_IMPL;
}

void gui_refresh(void)
{
#if defined(_WIN32)
	gui_refresh_win32(gui);
#elif defined(__APPLE__)
	gui_init_macos(gui);
#elif defined(__linux__)
	gui_init_linux(gui);
#endif
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
		mem_free(gui);
		gui = NULL;
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

	gui_window_t* window = (gui_window_t*)mem_alloc(sizeof(*window));
	memset(window, 0, sizeof(*window));

	// TODO(paulh): Fix mem_realloc...
	// window->title = mem_alloc(window->title, sizeof(title));
	memset(window->title, 0, 4096);
	strcpy(window->title, title);
	window->x = x;
	window->y = y;
	window->w = w;
	window->h = h;
	window->flags = flags;
	window->bounds.x = window->x;
	window->bounds.y = window->x;
	window->bounds.w = window->w;
	window->bounds.h = window->h;
	window->parent = parent;
	if (GUI_WINDOW_POS_IS_UNDEFINED(window->x) ||
	    GUI_WINDOW_POS_IS_UNDEFINED(window->y)) {
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

	// for (int i = 0; i < gui->windows.num_elems; i++) {
	// 	gui_window_t* wnd = (gui_window_t*)gui->windows.elems[i];
	// 	if (wnd && wnd == window)
	// 		vec_erase(gui->windows, i);
	// }
	window->destroy_me = true;
	// if (window->title) {
	// 	mem_free(window->title);
	// 	window->title = NULL;
	// }

	gui->destroy_window(window);
	mem_free(window);
	window = NULL;
}

void gui_show_window(gui_window_t* window, bool shown)
{
	if (!gui || !window)
		return;
	gui->show_window(window, shown);
}

void gui_set_window_pos(gui_window_t* window, int x, int y)
{
	if (!gui || !window)
		return;
	gui->set_window_pos(window, x, y);
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

void gui_clear_key_state()
{
	if (!gui)
		return;
	for (size_t i = 0; i < SCANCODE_MAX; i++)
		gui->keyboard[i].state = KEY_UP;
}

void gui_get_global_mouse_state(struct mouse_device* mouse)
{
	if (!gui || !mouse)
		return;
	gui->get_global_mouse_state(mouse);
}

bool gui_poll_event(gui_event_t* event)
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
