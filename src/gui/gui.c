#include "gui/gui.h"

#include "core/types.h"

static gui_platform_t* gui = NULL;

bool gui_init(void)
{
	bool result = false;

	if (gui) {
		free(gui);
		gui = NULL;
	}

	gui = calloc(1, sizeof(*gui));

#ifdef BM_WINDOWS
	result = gui_init_win32(gui);
#elif BM_MACOS
	result = gui_init_macos(gui);
#elif BM_LINUX
	result = gui_init_linux(gui);
#endif
	return result;
}

void gui_shutdown(void) {}

gui_display_t* gui_create_display(s32 index)
{
	gui_display_t* display = NULL;
	return display;
}

void gui_destroy_display(gui_display_t* display) {}

gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w, s32 h,
				s32 flags, gui_window_t* parent)
{
	if (!gui) {
		if (!gui_init())
			return NULL;
	}

	gui_window_t* window = NULL;

	window = (gui_window_t*)calloc(1, sizeof(*window));

	window->title = realloc(window->title, sizeof(title));
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
	if (BM_WINDOW_POS_IS_UNDEFINED(window->x) ||
	    BM_WINDOW_POS_IS_UNDEFINED(window->y)) {
	}

	window->destroy_me = false;

	if (!gui->create_window(gui, window))
		gui->destroy_window(gui, window);

	return window;
}

void gui_destroy_window(gui_window_t* window)
{
	if (!gui)
		return;

	window->destroy_me = true;
	if (window->title) {
		free(window->title);
		window->title = NULL;
	}

	gui->destroy_window(gui, window);

	free(window);
	window = NULL;
}

void gui_show_window(gui_window_t* window, bool shown)
{
	if (!gui)
		return;

	gui->show_window(gui, window, shown);
}

void* gui_get_window_handle(gui_window_t* window)
{
	if (window && window->data)
		return gui->get_handle(gui, window);
	return NULL;
}
