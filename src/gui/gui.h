#ifndef H_BM_GUI
#define H_BM_GUI

#include "core/types.h"
#include "core/export.h"
#include "core/vector.h"

#include "gui/gui_usb_hid.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BM_WINDOW_POS_UNDEFINED_MASK 0x1fff0000u
#define BM_WINDOW_POS_UNDEFINED_DO_MASK(x) (BM_WINDOW_POS_UNDEFINED_MASK | (x))
#define BM_WINDOW_POS_UNDEFINED BM_WINDOW_POS_UNDEFINED_DO_MASK(0)
#define BM_WINDOW_POS_IS_UNDEFINED(x) \
    (((x)&0xFFFF0000) == BM_WINDOW_POS_UNDEFINED_MASK)

#define BM_WINDOW_POS_CENTERED_MASK 0x2fff0000u
#define BM_WINDOW_POS_CENTERED_DO_MASK(x) (BM_WINDOW_POS_CENTERED_MASK | (x))
#define BM_WINDOW_POS_CENTERED BM_WINDOW_POS_CENTERED_DO_MASK(0)
#define BM_WINDOW_POS_IS_CENTERED(x) \
    (((x)&0xFFFF0000) == BM_WINDOW_POS_CENTERED_MASK)

typedef struct gui_window gui_window_t;
typedef struct gui_window_data gui_window_data_t;
typedef struct gui_subsystem gui_platform_t;
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

typedef struct {
    gui_scancode_t scancode;
    gui_key_action_t action;
    gui_key_mod_t modifier;
} gui_key_event_t;

typedef struct {
    enum mouse_button button;
    enum mouse_action action;
    int x;
    int y;
} gui_mouse_event_t;

typedef enum {
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
    void* ctx;
    union {
        gui_key_event_t keyboard;
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

struct gui_subsystem {
    VECTOR(gui_window_t*) windows;
    VECTOR(gui_event_t) events;
    struct gui_keyboard keyboard;
    bool (*create_window)(gui_window_t* window);
    void (*destroy_window)(gui_window_t* window);
    void (*show_window)(gui_window_t* window, bool shown);
    void* (*get_handle)(gui_window_t* window);
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
BM_EXPORT void gui_shutdown(void);

#if defined(_WIN32)
BM_EXPORT bool gui_init_win32(gui_platform_t* gs);
#elif defined(__APPLE__)
BM_EXPORT bool gui_init_macos(gui_platform_t* gs);
#elif defined(__linux__)
BM_EXPORT bool gui_init_linux(gui_platform_t* gs);
#endif

BM_EXPORT gui_display_t* gui_create_display(s32 index);
BM_EXPORT void gui_destroy_display(gui_display_t* display);

BM_EXPORT gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w,
                       s32 h, s32 flags, gui_window_t* parent);
BM_EXPORT void gui_destroy_window(gui_window_t* window);
BM_EXPORT void gui_show_window(gui_window_t* window, bool shown);
BM_EXPORT void* gui_get_window_handle(gui_window_t* window);
BM_EXPORT void gui_clear_key_state(u8* key_state);

#ifdef __cplusplus
}
#endif

#endif
