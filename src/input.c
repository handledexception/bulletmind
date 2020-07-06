#include "command.h"
#include "input.h"

#include <SDL.h>

#include <stdio.h>
#include <string.h>

#define MAX_KEYS 285
#define MAX_MBUTTONS 64

typedef struct {
    uint8_t state;
    int32_t cmd;
} key_t;

typedef struct {
    uint8_t state;
    int32_t cmd;
} mbutton_t;

static key_t *array_keys = NULL;
size_t sz_arraykeys = sizeof(key_t) * MAX_KEYS;

static mbutton_t *array_mbuttons = NULL;
size_t sz_arraymbuttons = sizeof(mbutton_t) * MAX_MBUTTONS;

void inp_init() {
    array_keys = (key_t *)malloc(sz_arraykeys);
    if (array_keys != NULL) { memset(array_keys, 0, sz_arraykeys); }

    array_mbuttons = (mbutton_t *)malloc(sz_arraymbuttons);
    if (array_mbuttons != NULL) { memset(array_mbuttons, 0, sz_arraymbuttons); }

    inp_set_key_bind(SDL_SCANCODE_ESCAPE, CMD_QUIT);
    inp_set_key_bind(SDL_SCANCODE_W, CMD_PLAYER_UP);
    inp_set_key_bind(SDL_SCANCODE_S, CMD_PLAYER_DOWN);
    inp_set_key_bind(SDL_SCANCODE_A, CMD_PLAYER_LEFT);
    inp_set_key_bind(SDL_SCANCODE_D, CMD_PLAYER_RIGHT);
    inp_set_key_bind(SDL_SCANCODE_LSHIFT, CMD_PLAYER_SPEED);
    inp_set_key_bind(SDL_SCANCODE_F5, CMD_SET_FPS_60);
    inp_set_key_bind(SDL_SCANCODE_F6, CMD_SET_FPS_10);
    inp_set_key_bind(SDL_SCANCODE_F1, CMD_SET_DEBUG);
    //inp_set_key_bind(SDL_SCANCODE_X, 0x7f); // fail case for testing

    inp_set_mouse_bind(SDL_BUTTON_LEFT, CMD_PLAYER_PRIMARY_FIRE);
    inp_set_mouse_bind(SDL_BUTTON_RIGHT, CMD_PLAYER_ALTERNATE_FIRE);

    printf("inp_init OK\n");
}

uint32_t inp_refresh() {
    return 0;
}

void inp_shutdown() {
    if (array_keys) {
        free(array_keys);
        array_keys = NULL;
    }

    if (array_mbuttons) {
        free(array_mbuttons);
        array_mbuttons = NULL;
    }

    printf("inp_shutdown OK\n");
}

void inp_set_key_state(uint16_t key, uint8_t state) {
    if (array_keys != NULL) {
        if (array_keys[key].state != state) {
            array_keys[key].state = state;
        }

        int32_t cmd = (array_keys[key].state > 0) ? array_keys[key].cmd : -array_keys[key].cmd;
        if (cmd > 0) { *array_cmds |= cmd; }
        if (cmd < 0) { *array_cmds &= ~(-cmd); }
        //printf("inp_set_key_state - [key:state:cmd] %d : %d : %d\n", key, state, cmd);
    }
}

uint8_t	inp_get_key_state(uint16_t key) {
    uint8_t state = 0;
    if (array_keys != NULL) {
        state = array_keys[key].state;
    }

    return state;
}

bool inp_set_key_bind(uint16_t key, int32_t cmd) {
    bool found_cmd = false;
    const char *cmd_name = NULL;

    for (uint32_t i = 0; i < COMMAND_COUNT; i++) {
        if (COMMAND_LIST[i] == cmd) {
            cmd_name = COMMAND_NAMES[i];
            found_cmd = true;
        }
    }

    if (!found_cmd) {
        printf("inp_set_key_bind - error binding Key \"%s\", unknown Command ID \"%d\"!\n", SDL_GetScancodeName(key), cmd);
        return false;
    }

    if (array_keys) {
        array_keys[key].cmd = cmd;
        //printf("scancode = %d\n", key);
        printf("inp_set_key_bind - successfully bound Key \"%s\" to Command \"%s\"\n", SDL_GetScancodeName(key), cmd_name);
    }

    return true;
}

bool inp_set_mouse_bind(uint8_t button, int32_t cmd) {
    bool found_cmd = false;
    const char *cmd_name = NULL;

    for (uint32_t i = 0; i < COMMAND_COUNT; i++) {
        if (COMMAND_LIST[i] == cmd) {
            cmd_name = COMMAND_NAMES[i];
            found_cmd = true;
        }
    }

    if (!found_cmd) {
        printf("inp_set_mouse_bind - error binding Button \"%d\", unknown Command ID \"%d\"!\n", button, cmd);
        return false;
    }

    if (array_mbuttons) {
        array_mbuttons[button].cmd = cmd;
        printf("inp_set_mouse_bind - successfully bound Button \"%d\" to Command \"%s\"\n", button, cmd_name);
    }

    return true;
}

void inp_set_mouse_state(uint8_t button, uint8_t state) {
    if (array_mbuttons != NULL) {
        if (array_mbuttons[button].state != state) {
            array_mbuttons[button].state = state;
        }
    }

    int32_t cmd = (array_mbuttons[button].state > 0) ? array_mbuttons[button].cmd : -array_mbuttons[button].cmd;
    if (cmd > 0) { *array_cmds |= cmd; }
    if (cmd < 0) { *array_cmds &= ~cmd; }
    //printf("inp_set_mouse_state - [button:state:cmd] %d : %d : %d\n", button, state, cmd);
}
