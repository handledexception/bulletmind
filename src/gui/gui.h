#ifndef H_BM_GUI
#define H_BM_GUI

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"
#include "math/vec2.h"
#include "gui/gui_scancode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_GAMEPADS 8
#define MAX_GAMEPAD_BUTTONS 16
#define MAX_GAMEPAD_AXES 6
#define MAX_MOUSE_BUTTONS 16
#define MAX_KEYBOARD_KEYS GUI_SCANCODE_MAX
#define MAX_VIRTUAL_BUTTONS \
	(MAX_KEYBOARD_KEYS + MAX_MOUSE_BUTTONS + MAX_GAMEPAD_BUTTONS)

#define GUI_WINDOW_POS_UNDEFINED_MASK 0x1fff0000u
#define GUI_WINDOW_POS_UNDEFINED_DO_MASK(x) (GUI_WINDOW_POS_UNDEFINED_MASK | (x))
#define GUI_WINDOW_POS_UNDEFINED GUI_WINDOW_POS_UNDEFINED_DO_MASK(0)
#define GUI_WINDOW_POS_IS_UNDEFINED(x) \
    (((x)&0xFFFF0000) == GUI_WINDOW_POS_UNDEFINED_MASK)

#define GUI_WINDOW_POS_CENTERED_MASK 0x2fff0000u
#define GUI_WINDOW_POS_CENTERED_DO_MASK(x) (GUI_WINDOW_POS_CENTERED_MASK | (x))
#define GUI_WINDOW_POS_CENTERED GUI_WINDOW_POS_CENTERED_DO_MASK(0)
#define GUI_WINDOW_POS_IS_CENTERED(x) \
    (((x)&0xFFFF0000) == GUI_WINDOW_POS_CENTERED_MASK)

typedef struct gui_window gui_window_t;
typedef struct gui_window_data gui_window_data_t;
typedef struct gui_platform gui_platform_t;
typedef struct gui_display gui_display_t;

extern gui_platform_t* gui;

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

enum key_state {
    GUI_KEY_UP = 0,
    GUI_KEY_DOWN = 1,
};

typedef struct {
    gui_scancode_t scancode; /* keyboard scancode */
    gui_key_mod_t modifier;  /* key modifier */
    u8 state;                /* key pressed or released state */
} gui_key_t;

typedef struct {
    gui_key_t keys[MAX_KEYBOARD_KEYS]; /* keyboard key array */
} gui_keyboard_event_t;

enum mouse_button {
    GUI_MOUSE_BUTTON_LEFT = 0,
    GUI_MOUSE_BUTTON_RIGHT = 1,
    GUI_MOUSE_BUTTON_MIDDLE = 2,
    GUI_MOUSE_BUTTON_X1 = 3,
    GUI_MOUSE_BUTTON_X2 = 4,
};

typedef struct {
	u16 button; /* mouse button number */
	u8 state;   /* mouse button up/down state */
} gui_mouse_button_t;

typedef struct {
    gui_mouse_button_t buttons[MAX_MOUSE_BUTTONS]; /* mouse button array */
    struct vec2i screen_pos;                       /* mouse position on virtual screen */
    struct vec2i window_pos;                       /* mouse position in foreground window */
    struct vec2i wheel;                            /* mouse wheel position */
} gui_mouse_event_t;

typedef enum {
    GUI_EVENT_NONE,
    GUI_EVENT_MOUSE,
    GUI_EVENT_KEY,
    GUI_EVENT_TEXT,
    GUI_EVENT_WINDOW,
    GUI_EVENT_TOUCH,
    GUI_EVENT_APP
} gui_event_type_t;

typedef struct gui_event {
    u32 index;
    gui_event_type_t type;
    u64 timestamp;
    void* ctx;
    union {
        gui_keyboard_event_t keyboard;
        gui_mouse_event_t mouse;
    };
} gui_event_t;

struct gui_window {
    s32 id;
    s32 flags;
    char* title;
    s32 x;
    s32 y;
    s32 w;
    s32 h;
    s32 min_w;
    s32 min_h;
    s32 max_w;
    s32 max_h;
    rect_t bounds;
    bool destroy_me;
    gui_window_data_t* data;
    gui_window_t* parent;
};

struct gui_keyboard {
    u8 key_states[GUI_NUM_SCANCODES];
    int (*read_key_states)();
};

struct gui_mouse_button {
    u16 button;
    u8 state;
};

struct gui_mouse {
    struct vec2i screen_pos;
    struct vec2i window_pos;
    struct vec2i wheel;
    gui_window_t* window;
    struct gui_mouse_button buttons[MAX_MOUSE_BUTTONS];
};

struct gui_platform {
    VECTOR(gui_window_t*) windows;                          /* window list */
    VECTOR(gui_event_t) events;                             /* GUI events */
    struct gui_keyboard keyboard;                           /* raw keyboard state */
    struct gui_mouse mouse;                                 /* raw mouse state */
    bool (*create_window)(gui_window_t* window);
    void (*destroy_window)(gui_window_t* window);
    void (*show_window)(gui_window_t* window, bool shown);
    void* (*get_handle)(gui_window_t* window);
    void (*get_global_mouse_state)(struct gui_mouse* mouse);
};

struct gui_display {
    s32 id;
    s32 index;
    char* name;
    rect_t bounds;
    gui_display_mode_t mode;
    gui_display_orientation_t orientation;
};

BM_EXPORT bool gui_init(void);
BM_EXPORT void gui_refresh(void);
BM_EXPORT void gui_shutdown(void);

#if defined(_WIN32)
BM_EXPORT bool gui_init_win32(gui_platform_t* gp);
BM_EXPORT void gui_refresh_win32(gui_platform_t* gp);
#elif defined(__APPLE__)
BM_EXPORT bool gui_init_macos(gui_platform_t* gp);
BM_EXPORT void gui_refresh_macos(gui_platform_t* gp);
#elif defined(__linux__)
BM_EXPORT bool gui_init_linux(gui_platform_t* gp);
BM_EXPORT void gui_refresh_linux(gui_platform_t* gp);
#endif

BM_EXPORT gui_display_t* gui_create_display(s32 index);
BM_EXPORT void gui_destroy_display(gui_display_t* display);

BM_EXPORT gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w,
                       s32 h, s32 flags, gui_window_t* parent);
BM_EXPORT void gui_destroy_window(gui_window_t* window);
BM_EXPORT void gui_show_window(gui_window_t* window, bool shown);
BM_EXPORT void* gui_get_window_handle(gui_window_t* window);
BM_EXPORT gui_window_t* gui_get_window_by_handle(void* handle);
BM_EXPORT void gui_clear_key_state(u8* key_state);
BM_EXPORT void gui_get_global_mouse_state(struct gui_mouse* mouse);

BM_EXPORT bool gui_poll_event(gui_event_t* event);
BM_EXPORT void gui_refresh_win32(gui_platform_t* gp);

#ifdef __cplusplus
}
#endif

#endif
