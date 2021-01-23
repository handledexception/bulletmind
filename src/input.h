#ifndef _H_INPUT
#define _H_INPUT

#include "c99defs.h"
#include "command.h"
#include "types.h"
#include <stdlib.h>

#define KEY_DOWN 1
#define KEY_UP 0

static const i32 kMaxGamepads = 8;
static const i32 kMaxGamepadButtons = 64;
static const i32 kMaxGamepadAxes = 16;
static const i32 kMaxMouseButtons = 16;
static const i32 kMaxKeyboardKeys = 512;

typedef enum {
    kInputKeyboard = 0,
    kInputMouse = 1,
    kInputJoystick = 2,
    kInputGamepad = 3,
} input_type_t;

struct gamepad_state {
    const char* name;
    bool is_connected;
    i32 num_buttons;
    i32 num_axes;
    bool pressed[kMaxGamepadButtons];
    bool down[kMaxGamepadButtons];
    bool up[kMaxGamepadButtons];
    float axis[kMaxGamepadAxes];
    u64 button_timestamp[kMaxGamepadButtons];
    u64 axis_timestamp[kMaxGamepadAxes];
    u16 product_id;
    u16 vendor_id;
    u16 version;
};

struct gamepad_button {
    i32 index;
    const char* name;
    const char* alt_name;
};

typedef enum {
    kAxisNone = -1,
    kAxisLeftStickX = 0,
    kAxisLeftStickY = 1,
    kAxisRightStickX = 2,
    kAxisRightStickY = 3,
    kAxisLeftTrigger = 4,
    kAxisRightTrigger = 5,
} gamepad_axis_t;

struct gamepad_stick {
    i32 index;
    const char* name;
    gamepad_axis_t axes[kMaxGamepadAxes];
};

// struct input_state {

// };

void inp_init();
u32 inp_refresh();
void inp_shutdown();

void inp_set_key_state(u16 key, u8 state);
u8   inp_get_key_state(u16 key);
bool inp_set_key_bind(u16 key, command_t cmd);

bool inp_set_mouse_bind(u8 button, command_t cmd);
u8   inp_get_mouse_state(u16 button);
void inp_set_mouse_state(u8 button, u8 state);

#endif
