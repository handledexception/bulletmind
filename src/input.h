#pragma once

#include "c99defs.h"
#include "command.h"
#include "vector.h"
#include <stdlib.h>

#include <SDL.h>

#define KEY_DOWN true
#define KEY_UP false

static const i32 kMaxGamepads = 8;
static const i32 kMaxGamepadButtons = SDL_CONTROLLER_BUTTON_MAX;
static const i32 kMaxGamepadAxes = SDL_CONTROLLER_AXIS_MAX;
static const i32 kMaxMouseButtons = 16;
static const i32 kMaxKeyboardKeys = 512;

typedef enum {
    kInputNone = -1,
    kInputKeyboard = 0,
    kInputMouse = 1,
    kInputJoystick = 2,
    kInputGamepad = 3,
    kInputMax = 4
} input_type_t;

typedef enum {
    kGamepadAxisNone = -1,
    kGamepadAxisLeftStickX = 0,
    kGamepadAxisLeftStickY = 1,
    kGamepadAxisRightStickX = 2,
    kGamepadAxisRightStickY = 3,
    kGamepadAxisLeftTrigger = 4,
    kGamepadAxisRightTrigger = 5,
    kGamepadAxisMax = 6
} gamepad_axis_kind_t;

typedef struct gamepad_axis_s {
    const char* name;
    i16 value;
    u64 timestamp;
    gamepad_axis_kind_t kind;
} gamepad_axis_t;

typedef struct gamepad_button_s {
    i32 index;
    const char* name;
    const char* alt_name;
    bool pressed;
    bool down;
    bool up;
    u64 timestamp;
    command_t cmd;
} gamepad_button_t;

typedef struct gamepad_s {
    const char* name;
    void* instance;
    i32 index;
    bool is_connected;
    u16 product_id;
    u16 vendor_id;
    u16 version;
    gamepad_button_t buttons[kMaxGamepadButtons];
    gamepad_axis_t axes[kGamepadAxisMax];
} gamepad_t;

typedef struct key_s {
    u64 timestamp;                          // timestamp of last key state change
    u16 scancode;                           // keyboard scancode
    u8 state;                               // key up/down state
    command_t cmd;                          // command_t bound to this key
} key_t;

typedef struct mbutton_s {
    u64 timestamp;                          // timestamp of last mouse button state change
    u16 button;                             // button number
    u8 state;                               // button up/down state
    command_t cmd;                          // command_t bound to this mouse button
} mbutton_t;

typedef struct mouse_s {
    vec2i_t screen_pos;                     // mouse position on the screen
    vec2i_t window_pos;                     // mouse position within the window
    vec2i_t wheel;                          // mouse wheel vector
    mbutton_t buttons[kMaxMouseButtons];    // array of mouse buttons
} mouse_t;

typedef struct virtual_button {
    const char* name;                       // display name
    u8 state;                               // 0 = up/released, 1 = down/pressed
    mbutton_t* mouse_button;
    key_t* keyboard_key;
    gamepad_button_t* gamepad_button;
};

typedef struct input_state_s {
    gamepad_t gamepad[kMaxGamepads];        // array of gamepad states
    key_t key[kMaxKeyboardKeys];            // array of keyboard key states
    mouse_t mouse;                          // mouse state
} input_state_t;

bool inp_init(input_state_t* inputs);
void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y);
void inp_refresh_pressed(input_state_t* inputs, const SDL_Event* evt);
void inp_shutdown(input_state_t* inputs);

bool inp_init_keyboard(input_state_t* inputs);
bool inp_init_mouse(input_state_t* mouse);
bool inp_init_gamepads(input_state_t* inputs);

void inp_set_key_state(key_t* keys, u16 scancode, u8 state);
u8   inp_get_key_state(key_t* keys, u16 scancode);
bool inp_set_key_bind(key_t* keys, u16 scancode, command_t cmd);

void inp_set_mouse_pos(mouse_t* mouse, const vec2i_t scr, const vec2i_t wnd);
void inp_set_mouse_button_state(mouse_t* mouse, u16 button, u8 state);
u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button);
bool inp_set_mouse_button_bind(mouse_t* mouse, u8 button, command_t cmd);

gamepad_axis_kind_t inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis);
bool inp_get_gamepad_axes(gamepad_t* gamepad);
