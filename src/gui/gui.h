#ifndef H_BM_GUI
#define H_BM_GUI

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
	GUI_EVENT_MOUSE_MOTION,
	GUI_EVENT_MOUSE_BUTTON_DOWN,
	GUI_EVENT_MOUSE_BUTTON_UP,
	GUI_EVENT_KEY_DOWN,
	GUI_EVENT_KEY_UP,
	GUI_EVENT_TEXT,
	GUI_EVENT_WINDOW_SIZE,
	GUI_EVENT_TOUCH,
	GUI_EVENT_APP
} gui_event_type_t;

enum gui_window_flags {
	GUI_WINDOW_SHOW = (1 << 0),
	GUI_WINDOW_CENTERED = (1 << 1),
};

typedef struct gui_event {
	u32 index;
	gui_event_type_t type;
	u64 timestamp;
	void* ctx;
	union {
		keyboard_event_t keyboard;
		mouse_event_t mouse;
	};
} gui_event_t;

struct gui_window {
	s32 id;
	s32 flags;
	char title[4096];
	s32 min_w;
	s32 min_h;
	s32 max_w;
	s32 max_h;
	rect_t bounds;
	bool destroy_me;
	gui_window_data_t* data;
	gui_window_t* parent;
};

/* clang-format off */
struct gui_system {
	VECTOR(gui_window_t*) windows;                                  /* window list */
	VECTOR(gui_event_t) events;                                     /* GUI events */
	struct keyboard_key keyboard[MAX_KEYBOARD_KEYS];                /* raw keyboard state */
	struct mouse_device mouse;                                      /* raw mouse state */
	bool (*create_window)(gui_window_t* window);                    /* platform window create function */
	void (*destroy_window)(gui_window_t* window);                   /* platform window destroy function */
	void (*show_window)(gui_window_t* window, bool shown);          /* platform window show function */
	void (*set_window_pos)(gui_window_t* window, const rect_t* rect);
	bool (*get_window_rect)(const gui_window_t* window, rect_t* rect, bool client);
	void (*center_window)(gui_window_t* window);                    /* center window within parent/screen */
	void* (*get_handle)(gui_window_t* window);                      /* platform window get handle function */
	void (*get_global_mouse_state)(struct mouse_device* mouse);     /* platform window get global mouse state function */
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

BM_EXPORT result gui_init(void);
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

BM_EXPORT gui_display_t* gui_create_display(s32 index);
BM_EXPORT void gui_destroy_display(gui_display_t* display);

BM_EXPORT gui_window_t* gui_create_window(const char* title, s32 x, s32 y,
					  s32 w, s32 h, s32 flags,
					  gui_window_t* parent);
BM_EXPORT void gui_destroy_window(gui_window_t* window);
BM_EXPORT void gui_show_window(gui_window_t* window, bool shown);
BM_EXPORT void gui_set_window_pos(gui_window_t* window, const rect_t* rect);
BM_EXPORT void gui_center_window(gui_window_t* window);
BM_EXPORT bool gui_get_window_rect(const gui_window_t* window, rect_t* rect,
				   bool client);
BM_EXPORT void* gui_get_window_handle(gui_window_t* window);
BM_EXPORT gui_window_t* gui_get_window_by_handle(void* handle);
BM_EXPORT void gui_clear_key_state();
BM_EXPORT void gui_get_global_mouse_state(struct mouse_device* mouse);

BM_EXPORT bool gui_poll_event(gui_event_t* event);
BM_EXPORT void gui_refresh_win32(gui_system_t* gp);

#ifdef __cplusplus
}
#endif

#endif
