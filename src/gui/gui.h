#pragma once

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"

#include "platform/keyboard.h"
#include "platform/keyboard-scancode.h"
#include "platform/mouse.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_WINDOW_POS_UNDEFINED_MASK 0x1fff0000u
#define GUI_WINDOW_POS_UNDEFINED_DO_MASK(x) \
	(GUI_WINDOW_POS_UNDEFINED_MASK | (x))
#define GUI_WINDOW_POS_UNDEFINED GUI_WINDOW_POS_UNDEFINED_DO_MASK(0)
#define GUI_WINDOW_POS_IS_UNDEFINED(x) \
	(((x)&0xFFFF0000) == GUI_WINDOW_POS_UNDEFINED_MASK)

// #define GUI_WINDOW_POS_CENTERED_MASK 0x2fff0000u
// #define GUI_WINDOW_POS_CENTERED_DO_MASK(x) (GUI_WINDOW_POS_CENTERED_MASK | (x))
// #define GUI_WINDOW_POS_CENTERED GUI_WINDOW_POS_CENTERED_DO_MASK(0)
// #define GUI_WINDOW_POS_IS_CENTERED(x) \
// 	(((x)&0xFFFF0000) == GUI_WINDOW_POS_CENTERED_MASK)

typedef struct gui_window gui_window_t;
typedef struct gui_window_data gui_window_data_t;
typedef struct gui_system gui_system_t;
typedef struct gui_display gui_display_t;

extern gui_system_t* gui;

typedef enum {
	GUI_BACKEND_DEFAULT = 0,
	GUI_BACKEND_SDL2 = 1,
} gui_backend_t;

typedef struct {
	u32 format;
	s32 w;
	s32 h;
	s32 refresh_rate;
	void* data;
} gui_display_mode_t;

typedef enum {
	GUI_ORIENTATION_DEFAULT,
	GUI_ORIENTATION_LANDSCAPE = GUI_ORIENTATION_DEFAULT,

	GUI_ORIENTATION_CW_90,
	GUI_ORIENTATION_PORTRAIT = GUI_ORIENTATION_CW_90,

	GUI_ORIENTATION_CW_180,
	GUI_ORIENTATION_LANDSCAPE_FLIP = GUI_ORIENTATION_CW_180,

	GUI_ORIENTATION_CCW_90,
	GUI_ORIENTATION_PORTRAIT_FLIP = GUI_ORIENTATION_CCW_90,

	GUI_ORIENTATION_UNKNOWN
} gui_display_orientation_t;

typedef enum {
	GUI_EVENT_NONE,
	GUI_EVENT_MOUSE_CAPTURE,
	GUI_EVENT_MOUSE_MOTION,
	GUI_EVENT_MOUSE_BUTTON_DOWN,
	GUI_EVENT_MOUSE_BUTTON_UP,
	GUI_EVENT_KEY_DOWN,
	GUI_EVENT_KEY_UP,
	GUI_EVENT_TEXT,
	GUI_EVENT_WINDOW_SIZE,
	GUI_EVENT_WINDOW_ACTIVATE,
	GUI_EVENT_WINDOW_MOUSE_ACTIVATE,
	GUI_EVENT_TOUCH,
	GUI_EVENT_APP,
	GUI_EVENT_EXIT,
} gui_event_type_t;

enum gui_window_flags {
	GUI_WINDOW_SHOW = (1 << 0),
	GUI_WINDOW_CENTERED = (1 << 1),
	GUI_WINDOW_CAPTURE_MOUSE = (1 << 2),
};

typedef struct gui_event {
	u32 index;
	gui_event_type_t type;
	u64 timestamp;
	void* ctx;
	keyboard_event_t keyboard;
	mouse_event_t mouse;
	u32 window_width;
	u32 window_height;
} gui_event_t;

BM_EXPORT bool gui_event_push(const gui_event_t* event);
BM_EXPORT bool gui_event_pop(gui_event_t* event);

struct gui_window {
	s32 id;
	s32 flags;
	char* title;
	s32 min_w;
	s32 min_h;
	s32 max_w;
	s32 max_h;
	vec2i_t centerpoint;
	rect_t bounds;
	bool destroy_me;
	gui_window_data_t* data;
	gui_window_t* parent;
	enum gui_backend_t backend;
};

/* clang-format off */
struct gui_system {
	enum gui_backend_t backend;
	VECTOR(gui_window_t*) windows;									/* window list */
	VECTOR(gui_event_t) events;										/* GUI events */
	keyboard_key_t keyboard[MAX_KEYBOARD_KEYS];						/* raw keyboard state */
	mouse_t mouse;													/* raw mouse state */
	gui_window_t* window_focused;									/* currently focused window */
	bool  (*create_window)(gui_window_t* window);					/* platform window create function */
	void  (*destroy_window)(gui_window_t* window);					/* platform window destroy function */
	bool  (*is_valid_window)(const gui_window_t* window);			/* is window and internal handle valid? */
	void  (*show_window)(gui_window_t* window, bool shown);			/* platform window show function */
	bool  (*focus_window)(gui_window_t* window);				/* set focused window */
	void  (*set_window_pos)(gui_window_t* window, const rect_t* rect);
	bool  (*get_window_size)(const gui_window_t* window, s32* w, s32* h, bool client);
	bool  (*get_window_rect)(const gui_window_t* window, rect_t* rect, bool client);
	bool  (*get_window_centerpoint)(const gui_window_t* window, vec2i_t* p, bool client);
	void  (*center_window)(gui_window_t* window);					/* center window within parent/screen */
	void* (*get_handle)(gui_window_t* window);						/* platform window get handle function */
	void  (*read_mouse_state)(mouse_t* mouse);				/* platform window get global mouse state function */
	bool  (*set_mouse_mode)(enum mouse_mode mode);
	bool  (*capture_mouse)(gui_window_t* window, bool captured);	/* platform mouse capture */
	bool  (*move_mouse)(s32 x, s32 y);
	bool  (*show_mouse)(bool shown);
};
/* clang-format on */

struct gui_display {
	s32 id;
	s32 index;
	char* name;
	rect_t bounds;
	gui_display_mode_t mode;
	gui_display_orientation_t orientation;
};

BM_EXPORT result gui_init(enum gui_backend_t platform);
BM_EXPORT void gui_refresh(void);
BM_EXPORT void gui_shutdown(void);

#if defined(_WIN32)
BM_EXPORT result gui_init_win32(gui_system_t* gp);
BM_EXPORT void gui_refresh_win32(gui_system_t* gp);
#elif defined(__APPLE__)
BM_EXPORT result gui_init_macos(gui_system_t* gp);
BM_EXPORT void gui_refresh_macos(gui_system_t* gp);
#elif defined(__linux__)
BM_EXPORT result gui_init_linux(gui_system_t* gp);
BM_EXPORT void gui_refresh_linux(gui_system_t* gp);
#endif
#ifdef BM_USE_SDL2
typedef struct SDL_Window SDL_Window;
BM_EXPORT result gui_init_sdl2(gui_system_t* gs);
BM_EXPORT void gui_refresh_sdl2(gui_system_t* gs);
BM_EXPORT SDL_Window*
gui_get_sdl_window_from_window(const gui_window_t* window);
#endif

BM_EXPORT mouse_t* gui_get_mouse();
BM_EXPORT bool gui_set_mouse_mode(enum mouse_mode mode);
BM_EXPORT bool gui_capture_mouse(gui_window_t* window, bool captured);
BM_EXPORT bool gui_move_mouse(s32 x, s32 y);
BM_EXPORT bool gui_show_mouse(bool shown);
BM_EXPORT bool gui_constrain_mouse(s32 curr_x, s32 curr_y, const rect_t* r);
BM_EXPORT gui_display_t* gui_create_display(s32 index);
BM_EXPORT void gui_destroy_display(gui_display_t* display);
BM_EXPORT gui_window_t* gui_create_window(const char* title, s32 x, s32 y,
					  s32 w, s32 h, s32 flags,
					  gui_window_t* parent);
BM_EXPORT void gui_destroy_window(gui_window_t* window);
BM_EXPORT bool gui_is_valid_window(const gui_window_t* window);
BM_EXPORT void gui_show_window(gui_window_t* window, bool shown);
BM_EXPORT void gui_set_window_pos(gui_window_t* window, const rect_t* rect);
BM_EXPORT void gui_center_window(gui_window_t* window);
BM_EXPORT bool gui_get_window_size(const gui_window_t* window, s32* w, s32* h,
				   bool client);
BM_EXPORT bool gui_get_window_rect(const gui_window_t* window, rect_t* rect,
				   bool client);
BM_EXPORT bool gui_get_window_centerpoint(const gui_window_t* window,
					  vec2i_t* p, bool client);
BM_EXPORT void* gui_get_window_handle(gui_window_t* window);
BM_EXPORT gui_window_t* gui_get_window_by_handle(void* handle);
BM_EXPORT bool gui_set_focused_window(gui_window_t* window);
BM_EXPORT gui_window_t* gui_get_focused_window();
BM_EXPORT void gui_clear_key_state();
BM_EXPORT void gui_read_mouse_state(mouse_t* mouse);
BM_EXPORT void gui_refresh_win32(gui_system_t* gp);

#ifdef __cplusplus
}
#endif
