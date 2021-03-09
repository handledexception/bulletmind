#pragma once

#include "scancodes.h"

#include "c99defs.h"
#include "command.h"
#include "vector.h"
#include <stdlib.h>

#include <SDL.h>

#define KEY_DOWN true
#define KEY_UP false

static const i32 kMaxGamepads = 8;
static const i32 kMaxGamepadButtons = 16;
static const i32 kMaxGamepadAxes = 6;
static const i32 kMaxMouseButtons = 16;
static const i32 kMaxKeyboardKeys = kScancodeMax;
static const i32 kMaxVirtualButtons = 
    kMaxKeyboardKeys + kMaxMouseButtons + kMaxGamepadButtons;

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

typedef enum {
    kGamepadeButtonNone = -1,
    kGamepadButtonA = 0,
    kGamepadButtonB = 1,
    kGamepadButtonX = 2,
    kGamepadButtonY = 3,
    kGamepadButtonBack = 4,
    kGamepadButtonGuide = 5,
    kGamepadButtonStart = 6,
    kGamepadButtonLeftStick = 7,
    kGamepadButtonRightStick = 8,
    kGamepadButtonLeftShoulder = 9,
    kGamepadButtonRightShoulder = 10,
    kGamepadButtonDPadUp = 11,
    kGamepadButtonDPadDown = 12,
    kGamepadButtonDPadLeft = 13,
    kGamepadButtonDPadRight = 14,
    kGamepadButtonMax = 15
} gamepad_button_kind_t;

typedef struct gamepad_axis_s {
    i32 index;
    const char* name;
    i16 value;
    u64 timestamp;
    gamepad_axis_kind_t kind;
} gamepad_axis_t;

typedef struct gamepad_button_s {
    i32 index;
    const char* name;
    const char* alt_name;
    gamepad_button_kind_t kind;
    u8 state;
    u64 timestamp;
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
} key_t;

typedef struct mbutton_s {
    u64 timestamp;                          // timestamp of last mouse button state change
    u16 button;                             // button number
    u8 state;                               // button up/down state
} mouse_button_t;

typedef struct mouse_s {
    vec2i_t screen_pos;                     // mouse position on the screen
    vec2i_t window_pos;                     // mouse position within the window
    vec2i_t wheel;                          // mouse wheel vector
    mouse_button_t buttons[kMaxMouseButtons];    // array of mouse buttons
} mouse_t;

typedef struct virtual_button_s {
    const char* name;                       // display name
    u8 state;                               // 0 = up/released, 1 = down/pressed
    mouse_button_t* mouse_button;
    key_t* keyboard_key;
    gamepad_button_t* gamepad_button;
} virtual_button_t;

typedef struct input_state_s {
    gamepad_t gamepads[kMaxGamepads];        // array of gamepad states
    key_t keys[kMaxKeyboardKeys];            // array of keyboard key states
    mouse_t mouse;                          // mouse state
    virtual_button_t buttons[kMaxVirtualButtons];
} input_state_t;

bool inp_init(input_state_t* inputs);
void inp_refresh_mouse(mouse_t* mouse, f32 scale_x, f32 scale_y);
void inp_refresh_pressed(input_state_t* inputs, const SDL_Event* evt);
void inp_shutdown(input_state_t* inputs);

bool inp_init_keyboard(input_state_t* inputs);
bool inp_init_mouse(input_state_t* inputs);
bool inp_init_gamepads(input_state_t* inputs);

void inp_set_key_state(key_t* keys, u16 scancode, u8 state);
u8   inp_get_key_state(key_t* keys, u16 scancode);

void inp_set_mouse_pos(mouse_t* mouse, const vec2i_t scr, const vec2i_t wnd);
void inp_set_mouse_button_state(mouse_t* mouse, u16 button, u8 state);
u8 inp_get_mouse_button_state(mouse_t* mouse, u16 button);

// Gamepad deadzone
// https://www.gamasutra.com/blogs/JoshSutphin/20130416/190541/Doing_Thumbstick_Dead_Zones_Right.php
gamepad_button_kind_t inp_gamepad_button_kind_from_sdl(const SDL_GameControllerButton button);
gamepad_axis_kind_t inp_gamepad_axis_kind_from_sdl(const SDL_GameControllerAxis axis);
bool inp_enumerate_gamepad_buttons(gamepad_t* gamepad);
bool inp_enumerate_gamepad_axes(gamepad_t* gamepad);
void inp_set_gamepad_button_state(gamepad_t* gamepad, gamepad_button_kind_t button, u8 state);
u8 inp_get_gamepad_button_state(gamepad_t* gamepad, gamepad_button_kind_t button);
void inp_set_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis, u16 value);
i16 inp_get_gamepad_axis_value(gamepad_t* gamepad, gamepad_axis_kind_t axis);

bool inp_bind_virtual_key(input_state_t* inputs, command_t cmd, u16 scancode);
bool inp_bind_virtual_mouse_button(input_state_t* inputs, command_t cmd, u16 mouse_button);
bool inp_bind_virtual_gamepad_button(input_state_t* inputs, command_t cmd, u32 gamepad, gamepad_button_kind_t button);
