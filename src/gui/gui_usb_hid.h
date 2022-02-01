/* NOTE(paulh): Portions of this code adapted from the SDL2 library
 * and https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
 */

/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef H_BM_GUI_SCANCODE
#define H_BM_GUI_SCANCODE

// from gunslinger ... maybe use?
// typedef enum {
//     GUI_KEY_MOD_NONE    = 0x00,
//     GUI_KEY_MOD_LSHIFT   = 0x01,
//     GUI_KEY_MOD_LCONTROL = 0x02,
//     GUI_KEY_MOD_LALT     = 0x04,
//     GUI_KEY_MOD_RSHIFT = 0x05,
//     GUI_KEY_MOD_RCONTROL = 0x06,
//     GUI_KEY_MOD_RALT = 0x07,
//     GUI_KEY_MOD_LMENU = 0x08,
//     GUI_KEY_MOD_RMENU = 0x09,
// } gui_key_mod_t;
typedef enum {
    /**
     * Modifier masks - used for the first byte in the HID report.
     * NOTE: The second byte in the report is reserved, 0x00
     */
    GUI_KEY_MOD_NONE    = 0x00,
    GUI_KEY_MOD_LCTRL   = 0x01,
    GUI_KEY_MOD_LSHIFT  = 0x02,
    GUI_KEY_MOD_LALT    = 0x04,
    GUI_KEY_MOD_LMETA   = 0x08,
    GUI_KEY_MOD_RCTRL   = 0x10,
    GUI_KEY_MOD_RSHIFT  = 0x20,
    GUI_KEY_MOD_RALT    = 0x40,
    GUI_KEY_MOD_RMETA   = 0x80,
} gui_key_mod_t;

typedef enum {
    GUI_KEY_PRESSED,
    GUI_KEY_DOWN,
    GUI_KEY_UP
} gui_key_action_t;

typedef enum {
    GUI_SCANCODE_NONE = 0,
    /* From Universal Serial Bus HID Usage Tables, Keyboard Page 0x07 */
    GUI_SCANCODE_A = 4,
    GUI_SCANCODE_B = 5,
    GUI_SCANCODE_C = 6,
    GUI_SCANCODE_D = 7,
    GUI_SCANCODE_E = 8,
    GUI_SCANCODE_F = 9,
    GUI_SCANCODE_G = 10,
    GUI_SCANCODE_H = 11,
    GUI_SCANCODE_I = 12,
    GUI_SCANCODE_J = 13,
    GUI_SCANCODE_K = 14,
    GUI_SCANCODE_L = 15,
    GUI_SCANCODE_M = 16,
    GUI_SCANCODE_N = 17,
    GUI_SCANCODE_O = 18,
    GUI_SCANCODE_P = 19,
    GUI_SCANCODE_Q = 20,
    GUI_SCANCODE_R = 21,
    GUI_SCANCODE_S = 22,
    GUI_SCANCODE_T = 23,
    GUI_SCANCODE_U = 24,
    GUI_SCANCODE_V = 25,
    GUI_SCANCODE_W = 26,
    GUI_SCANCODE_X = 27,
    GUI_SCANCODE_Y = 28,
    GUI_SCANCODE_Z = 29,

    GUI_SCANCODE_1 = 30,
    GUI_SCANCODE_2 = 31,
    GUI_SCANCODE_3 = 32,
    GUI_SCANCODE_4 = 33,
    GUI_SCANCODE_5 = 34,
    GUI_SCANCODE_6 = 35,
    GUI_SCANCODE_7 = 36,
    GUI_SCANCODE_8 = 37,
    GUI_SCANCODE_9 = 38,
    GUI_SCANCODE_0 = 39,

    GUI_SCANCODE_RETURN = 40,
    GUI_SCANCODE_ESCAPE = 41,
    GUI_SCANCODE_BACKSPACE = 42,
    GUI_SCANCODE_TAB = 43,
    GUI_SCANCODE_SPACE = 44,

    GUI_SCANCODE_MINUS = 45,
    GUI_SCANCODE_EQUALS = 46,
    GUI_SCANCODE_LEFTBRACKET = 47,
    GUI_SCANCODE_RIGHTBRACKET = 48,
    GUI_SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    GUI_SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate GUI_SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    GUI_SCANCODE_SEMICOLON = 51,
    GUI_SCANCODE_APOSTROPHE = 52,
    GUI_SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    GUI_SCANCODE_COMMA = 54,
    GUI_SCANCODE_PERIOD = 55,
    GUI_SCANCODE_SLASH = 56,

    GUI_SCANCODE_CAPSLOCK = 57,

    GUI_SCANCODE_F1 = 58,
    GUI_SCANCODE_F2 = 59,
    GUI_SCANCODE_F3 = 60,
    GUI_SCANCODE_F4 = 61,
    GUI_SCANCODE_F5 = 62,
    GUI_SCANCODE_F6 = 63,
    GUI_SCANCODE_F7 = 64,
    GUI_SCANCODE_F8 = 65,
    GUI_SCANCODE_F9 = 66,
    GUI_SCANCODE_F10 = 67,
    GUI_SCANCODE_F11 = 68,
    GUI_SCANCODE_F12 = 69,

    GUI_SCANCODE_PRINTSCREEN = 70,
    GUI_SCANCODE_SCROLLLOCK = 71,
    GUI_SCANCODE_PAUSE = 72,
    GUI_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    GUI_SCANCODE_HOME = 74,
    GUI_SCANCODE_PAGEUP = 75,
    GUI_SCANCODE_DELETE = 76,
    GUI_SCANCODE_END = 77,
    GUI_SCANCODE_PAGEDOWN = 78,
    GUI_SCANCODE_RIGHT = 79,
    GUI_SCANCODE_LEFT = 80,
    GUI_SCANCODE_DOWN = 81,
    GUI_SCANCODE_UP = 82,

    GUI_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    GUI_SCANCODE_KP_DIVIDE = 84,
    GUI_SCANCODE_KP_MULTIPLY = 85,
    GUI_SCANCODE_KP_MINUS = 86,
    GUI_SCANCODE_KP_PLUS = 87,
    GUI_SCANCODE_KP_ENTER = 88,
    GUI_SCANCODE_KP_1 = 89,
    GUI_SCANCODE_KP_2 = 90,
    GUI_SCANCODE_KP_3 = 91,
    GUI_SCANCODE_KP_4 = 92,
    GUI_SCANCODE_KP_5 = 93,
    GUI_SCANCODE_KP_6 = 94,
    GUI_SCANCODE_KP_7 = 95,
    GUI_SCANCODE_KP_8 = 96,
    GUI_SCANCODE_KP_9 = 97,
    GUI_SCANCODE_KP_0 = 98,
    GUI_SCANCODE_KP_PERIOD = 99,

    GUI_SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    GUI_SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
    GUI_SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    GUI_SCANCODE_KP_EQUALS = 103,
    GUI_SCANCODE_F13 = 104,
    GUI_SCANCODE_F14 = 105,
    GUI_SCANCODE_F15 = 106,
    GUI_SCANCODE_F16 = 107,
    GUI_SCANCODE_F17 = 108,
    GUI_SCANCODE_F18 = 109,
    GUI_SCANCODE_F19 = 110,
    GUI_SCANCODE_F20 = 111,
    GUI_SCANCODE_F21 = 112,
    GUI_SCANCODE_F22 = 113,
    GUI_SCANCODE_F23 = 114,
    GUI_SCANCODE_F24 = 115,
    GUI_SCANCODE_EXECUTE = 116,
    GUI_SCANCODE_HELP = 117,
    GUI_SCANCODE_MENU = 118,
    GUI_SCANCODE_SELECT = 119,
    GUI_SCANCODE_STOP = 120,
    GUI_SCANCODE_AGAIN = 121,   /**< redo */
    GUI_SCANCODE_UNDO = 122,
    GUI_SCANCODE_CUT = 123,
    GUI_SCANCODE_COPY = 124,
    GUI_SCANCODE_PASTE = 125,
    GUI_SCANCODE_FIND = 126,
    GUI_SCANCODE_MUTE = 127,
    GUI_SCANCODE_VOLUMEUP = 128,
    GUI_SCANCODE_VOLUMEDOWN = 129,
/* not sure whether there's a reason to enable these */
/*     GUI_SCANCODE_LOCKINGCAPSLOCK = 130,  */
/*     GUI_SCANCODE_LOCKINGNUMLOCK = 131, */
/*     GUI_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    GUI_SCANCODE_KP_COMMA = 133,
    GUI_SCANCODE_KP_EQUALSAS400 = 134,

    GUI_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    GUI_SCANCODE_INTERNATIONAL2 = 136,
    GUI_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    GUI_SCANCODE_INTERNATIONAL4 = 138,
    GUI_SCANCODE_INTERNATIONAL5 = 139,
    GUI_SCANCODE_INTERNATIONAL6 = 140,
    GUI_SCANCODE_INTERNATIONAL7 = 141,
    GUI_SCANCODE_INTERNATIONAL8 = 142,
    GUI_SCANCODE_INTERNATIONAL9 = 143,
    GUI_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    GUI_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    GUI_SCANCODE_LANG3 = 146, /**< Katakana */
    GUI_SCANCODE_LANG4 = 147, /**< Hiragana */
    GUI_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    GUI_SCANCODE_LANG6 = 149, /**< reserved */
    GUI_SCANCODE_LANG7 = 150, /**< reserved */
    GUI_SCANCODE_LANG8 = 151, /**< reserved */
    GUI_SCANCODE_LANG9 = 152, /**< reserved */

    GUI_SCANCODE_ALTERASE = 153, /**< Erase-Eaze */
    GUI_SCANCODE_SYSREQ = 154,
    GUI_SCANCODE_CANCEL = 155,
    GUI_SCANCODE_CLEAR = 156,
    GUI_SCANCODE_PRIOR = 157,
    GUI_SCANCODE_RETURN2 = 158,
    GUI_SCANCODE_SEPARATOR = 159,
    GUI_SCANCODE_OUT = 160,
    GUI_SCANCODE_OPER = 161,
    GUI_SCANCODE_CLEARAGAIN = 162,
    GUI_SCANCODE_CRSEL = 163,
    GUI_SCANCODE_EXSEL = 164,

    GUI_SCANCODE_KP_00 = 176,
    GUI_SCANCODE_KP_000 = 177,
    GUI_SCANCODE_THOUSANDSSEPARATOR = 178,
    GUI_SCANCODE_DECIMALSEPARATOR = 179,
    GUI_SCANCODE_CURRENCYUNIT = 180,
    GUI_SCANCODE_CURRENCYSUBUNIT = 181,
    GUI_SCANCODE_KP_LEFTPAREN = 182,
    GUI_SCANCODE_KP_RIGHTPAREN = 183,
    GUI_SCANCODE_KP_LEFTBRACE = 184,
    GUI_SCANCODE_KP_RIGHTBRACE = 185,
    GUI_SCANCODE_KP_TAB = 186,
    GUI_SCANCODE_KP_BACKSPACE = 187,
    GUI_SCANCODE_KP_A = 188,
    GUI_SCANCODE_KP_B = 189,
    GUI_SCANCODE_KP_C = 190,
    GUI_SCANCODE_KP_D = 191,
    GUI_SCANCODE_KP_E = 192,
    GUI_SCANCODE_KP_F = 193,
    GUI_SCANCODE_KP_XOR = 194,
    GUI_SCANCODE_KP_POWER = 195,
    GUI_SCANCODE_KP_PERCENT = 196,
    GUI_SCANCODE_KP_LESS = 197,
    GUI_SCANCODE_KP_GREATER = 198,
    GUI_SCANCODE_KP_AMPERSAND = 199,
    GUI_SCANCODE_KP_DBLAMPERSAND = 200,
    GUI_SCANCODE_KP_VERTICALBAR = 201,
    GUI_SCANCODE_KP_DBLVERTICALBAR = 202,
    GUI_SCANCODE_KP_COLON = 203,
    GUI_SCANCODE_KP_HASH = 204,
    GUI_SCANCODE_KP_SPACE = 205,
    GUI_SCANCODE_KP_AT = 206,
    GUI_SCANCODE_KP_EXCLAM = 207,
    GUI_SCANCODE_KP_MEMSTORE = 208,
    GUI_SCANCODE_KP_MEMRECALL = 209,
    GUI_SCANCODE_KP_MEMCLEAR = 210,
    GUI_SCANCODE_KP_MEMADD = 211,
    GUI_SCANCODE_KP_MEMSUBTRACT = 212,
    GUI_SCANCODE_KP_MEMMULTIPLY = 213,
    GUI_SCANCODE_KP_MEMDIVIDE = 214,
    GUI_SCANCODE_KP_PLUSMINUS = 215,
    GUI_SCANCODE_KP_CLEAR = 216,
    GUI_SCANCODE_KP_CLEARENTRY = 217,
    GUI_SCANCODE_KP_BINARY = 218,
    GUI_SCANCODE_KP_OCTAL = 219,
    GUI_SCANCODE_KP_DECIMAL = 220,
    GUI_SCANCODE_KP_HEXADECIMAL = 221,

    GUI_SCANCODE_LCTRL = 224,
    GUI_SCANCODE_LSHIFT = 225,
    GUI_SCANCODE_LALT = 226, /**< alt, option */
    GUI_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    GUI_SCANCODE_RCTRL = 228,
    GUI_SCANCODE_RSHIFT = 229,
    GUI_SCANCODE_RALT = 230, /**< alt gr, option */
    GUI_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    GUI_SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    GUI_SCANCODE_AUDIONEXT = 258,
    GUI_SCANCODE_AUDIOPREV = 259,
    GUI_SCANCODE_AUDIOSTOP = 260,
    GUI_SCANCODE_AUDIOPLAY = 261,
    GUI_SCANCODE_AUDIOMUTE = 262,
    GUI_SCANCODE_MEDIASELECT = 263,
    GUI_SCANCODE_WWW = 264,
    GUI_SCANCODE_MAIL = 265,
    GUI_SCANCODE_CALCULATOR = 266,
    GUI_SCANCODE_COMPUTER = 267,
    GUI_SCANCODE_AC_SEARCH = 268,
    GUI_SCANCODE_AC_HOME = 269,
    GUI_SCANCODE_AC_BACK = 270,
    GUI_SCANCODE_AC_FORWARD = 271,
    GUI_SCANCODE_AC_STOP = 272,
    GUI_SCANCODE_AC_REFRESH = 273,
    GUI_SCANCODE_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    GUI_SCANCODE_BRIGHTNESSDOWN = 275,
    GUI_SCANCODE_BRIGHTNESSUP = 276,
    GUI_SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    GUI_SCANCODE_KBDILLUMTOGGLE = 278,
    GUI_SCANCODE_KBDILLUMDOWN = 279,
    GUI_SCANCODE_KBDILLUMUP = 280,
    GUI_SCANCODE_EJECT = 281,
    GUI_SCANCODE_SLEEP = 282,

    GUI_SCANCODE_APP1 = 283,
    GUI_SCANCODE_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    GUI_SCANCODE_AUDIOREWIND = 285,
    GUI_SCANCODE_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

    GUI_NUM_SCANCODES = 512, /**< not a key, just marks the number of scancodes
                                 for array bounds */
    GUI_SCANCODE_MAX = GUI_NUM_SCANCODES
} gui_scancode_t;

#endif
