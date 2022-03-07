#ifndef H_BM_VKEY_WIN32
#define H_BM_VKEY_WIN32

#include "core/types.h"
#include "gui/gui_scancode.h"

enum gui_vkey_win32 {
	GUI_WIN32_VK_NONE		= 0x00,
	GUI_WIN32_VK_FIRST = GUI_WIN32_VK_NONE,
	GUI_WIN32_VK_LBUTTON	= 0x01,	/* Left mouse button */
	GUI_WIN32_VK_RBUTTON	= 0x02,	/* Right mouse button */
	GUI_WIN32_VK_CANCEL		= 0x03,	/* Control-break processing */
	GUI_WIN32_VK_MBUTTON	= 0x04,	/* Middle mouse button (three-button mouse) */
	GUI_WIN32_VK_XBUTTON1	= 0x05,	/* X1 mouse button */
	GUI_WIN32_VK_XBUTTON2	= 0x06,	/* X2 mouse button */
	/* -	0x07	Undefined */
	GUI_WIN32_VK_BACK 		= 0x08,	/* BACKSPACE key */
	GUI_WIN32_VK_TAB 		= 0x09,	/* TAB key */
	/* -	0x0A-0B	Reserved */
	GUI_WIN32_VK_CLEAR		= 0x0C,	/* CLEAR key */
	GUI_WIN32_VK_RETURN		= 0x0D,	/* ENTER key */
	/* -	0x0E-0F	Undefined */
	GUI_WIN32_VK_SHIFT		= 0x10,	/* SHIFT key */
	GUI_WIN32_VK_CONTROL	= 0x11,	/* CTRL key */
	GUI_WIN32_VK_MENU		= 0x12,	/* ALT key */
	GUI_WIN32_VK_PAUSE		= 0x13,	/* PAUSE key */
	GUI_WIN32_VK_CAPITAL	= 0x14,	/* CAPS LOCK key */
	GUI_WIN32_VK_KANA		= 0x15,	/* IME Kana mode */
	GUI_WIN32_VK_HANGUEL	= 0x15,	/* IME Hanguel mode (maintained for compatibility; use GUI_WIN32_VK_HANGUL) */
	GUI_WIN32_VK_HANGUL		= 0x15,	/* IME Hangul mode */
	GUI_WIN32_VK_IME_ON		= 0x16,	/* IME On */
	GUI_WIN32_VK_JUNJA		= 0x17,	/* IME Junja mode */
	GUI_WIN32_VK_FINAL		= 0x18,	/* IME final mode */
	GUI_WIN32_VK_HANJA		= 0x19,	/* IME Hanja mode */
	GUI_WIN32_VK_KANJI		= 0x19,	/* IME Kanji mode */
	GUI_WIN32_VK_IME_OFF	= 0x1A,	/* IME Off */
	GUI_WIN32_VK_ESCAPE		= 0x1B,	/* ESC key */
	GUI_WIN32_VK_CONVERT	= 0x1C,	/* IME convert */
	GUI_WIN32_VK_NONCONVERT	= 0x1D,	/* IME nonconvert */
	GUI_WIN32_VK_ACCEPT		= 0x1E,	/* IME accept */
	GUI_WIN32_VK_MODECHANGE	= 0x1F,	/* IME mode change request */
	GUI_WIN32_VK_SPACE		= 0x20,	/* SPACEBAR */
	GUI_WIN32_VK_PRIOR		= 0x21,	/* PAGE UP key */
	GUI_WIN32_VK_NEXT		= 0x22,	/* PAGE DOWN key */
	GUI_WIN32_VK_END		= 0x23,	/* END key */
	GUI_WIN32_VK_HOME		= 0x24,	/* HOME key */
	GUI_WIN32_VK_LEFT		= 0x25,	/* LEFT ARROW key */
	GUI_WIN32_VK_UP			= 0x26,	/* UP ARROW key */
	GUI_WIN32_VK_RIGHT		= 0x27,	/* RIGHT ARROW key */
	GUI_WIN32_VK_DOWN		= 0x28,	/* DOWN ARROW key */
	GUI_WIN32_VK_SELECT		= 0x29,	/* SELECT key */
	GUI_WIN32_VK_PRINT		= 0x2A,	/* PRINT key */
	GUI_WIN32_VK_EXECUTE	= 0x2B,	/* EXECUTE key */
	GUI_WIN32_VK_SNAPSHOT	= 0x2C,	/* PRINT SCREEN key */
	GUI_WIN32_VK_INSERT		= 0x2D,	/* INS key */
	GUI_WIN32_VK_DELETE		= 0x2E,	/* DEL key */
	GUI_WIN32_VK_HELP		= 0x2F,	/* HELP key */
	GUI_WIN32_VK_0 			= 0x30,	/* 0 key */
	GUI_WIN32_VK_1 			= 0x31,	/* 1 key */
	GUI_WIN32_VK_2 			= 0x32,	/* 2 key */
	GUI_WIN32_VK_3 			= 0x33,	/* 3 key */
	GUI_WIN32_VK_4 			= 0x34,	/* 4 key */
	GUI_WIN32_VK_5 			= 0x35,	/* 5 key */
	GUI_WIN32_VK_6 			= 0x36,	/* 6 key */
	GUI_WIN32_VK_7 			= 0x37,	/* 7 key */
	GUI_WIN32_VK_8 			= 0x38,	/* 8 key */
	GUI_WIN32_VK_9 			= 0x39,	/* 9 key */
	/* -	0x3A-40	Undefined */
	GUI_WIN32_VK_A			= 0x41,	/* A key */
	GUI_WIN32_VK_B			= 0x42,	/* B key */
	GUI_WIN32_VK_C			= 0x43,	/* C key */
	GUI_WIN32_VK_D			= 0x44,	/* D key */
	GUI_WIN32_VK_E			= 0x45,	/* E key */
	GUI_WIN32_VK_F			= 0x46,	/* F key */
	GUI_WIN32_VK_G			= 0x47,	/* G key */
	GUI_WIN32_VK_H			= 0x48,	/* H key */
	GUI_WIN32_VK_I			= 0x49,	/* I key */
	GUI_WIN32_VK_J			= 0x4A,	/* J key */
	GUI_WIN32_VK_K			= 0x4B,	/* K key */
	GUI_WIN32_VK_L			= 0x4C,	/* L key */
	GUI_WIN32_VK_M			= 0x4D,	/* M key */
	GUI_WIN32_VK_N			= 0x4E,	/* N key */
	GUI_WIN32_VK_O			= 0x4F,	/* O key */
	GUI_WIN32_VK_P			= 0x50,	/* P key */
	GUI_WIN32_VK_Q			= 0x51,	/* Q key */
	GUI_WIN32_VK_R			= 0x52,	/* R key */
	GUI_WIN32_VK_S			= 0x53,	/* S key */
	GUI_WIN32_VK_T			= 0x54,	/* T key */
	GUI_WIN32_VK_U			= 0x55,	/* U key */
	GUI_WIN32_VK_V			= 0x56,	/* V key */
	GUI_WIN32_VK_W			= 0x57,	/* W key */
	GUI_WIN32_VK_X			= 0x58,	/* X key */
	GUI_WIN32_VK_Y			= 0x59,	/* Y key */
	GUI_WIN32_VK_Z			= 0x5A,	/* Z key */
	GUI_WIN32_VK_LWIN		= 0x5B,	/* Left Windows key (Natural keyboard) */
	GUI_WIN32_VK_RWIN		= 0x5C,	/* Right Windows key (Natural keyboard) */
	GUI_WIN32_VK_APPS		= 0x5D,	/* Applications key (Natural keyboard) */
	/* -	0x5E	Reserved */
	GUI_WIN32_VK_SLEEP		= 0x5F,	/*, Computer Sleep key */
	GUI_WIN32_VK_NUMPAD0	= 0x60,	/* Numeric, keypad 0 key */
	GUI_WIN32_VK_NUMPAD1	= 0x61,	/* Numeric, keypad 1 key */
	GUI_WIN32_VK_NUMPAD2	= 0x62,	/* Numeric, keypad 2 key */
	GUI_WIN32_VK_NUMPAD3	= 0x63,	/* Numeric, keypad 3 key */
	GUI_WIN32_VK_NUMPAD4	= 0x64,	/* Numeric, keypad 4 key */
	GUI_WIN32_VK_NUMPAD5	= 0x65,	/* Numeric, keypad 5 key */
	GUI_WIN32_VK_NUMPAD6	= 0x66,	/* Numeric, keypad 6 key */
	GUI_WIN32_VK_NUMPAD7	= 0x67,	/* Numeric, keypad 7 key */
	GUI_WIN32_VK_NUMPAD8	= 0x68,	/* Numeric, keypad 8 key */
	GUI_WIN32_VK_NUMPAD9	= 0x69,	/* Numeric, keypad 9 key */
	GUI_WIN32_VK_MULTIPLY	= 0x6A,	/* Multiply key */
	GUI_WIN32_VK_ADD		= 0x6B,	/* Add key */
	GUI_WIN32_VK_SEPARATOR	= 0x6C,	/* Separator key */
	GUI_WIN32_VK_SUBTRACT	= 0x6D,	/* Subtract key */
	GUI_WIN32_VK_DECIMAL	= 0x6E,	/* Decimal key */
	GUI_WIN32_VK_DIVIDE		= 0x6F,	/* Divide key */
	GUI_WIN32_VK_F1			= 0x70,	/* F1 key */
	GUI_WIN32_VK_F2			= 0x71,	/* F2 key */
	GUI_WIN32_VK_F3			= 0x72,	/* F3 key */
	GUI_WIN32_VK_F4			= 0x73,	/* F4 key */
	GUI_WIN32_VK_F5			= 0x74,	/* F5 key */
	GUI_WIN32_VK_F6			= 0x75,	/* F6 key */
	GUI_WIN32_VK_F7			= 0x76,	/* F7 key */
	GUI_WIN32_VK_F8			= 0x77,	/* F8 key */
	GUI_WIN32_VK_F9			= 0x78,	/* F9 key */
	GUI_WIN32_VK_F10		= 0x79,	/* F10 key */
	GUI_WIN32_VK_F11		= 0x7A,	/* F11 key */
	GUI_WIN32_VK_F12		= 0x7B,	/* F12 key */
	GUI_WIN32_VK_F13		= 0x7C,	/* F13 key */
	GUI_WIN32_VK_F14		= 0x7D,	/* F14 key */
	GUI_WIN32_VK_F15		= 0x7E,	/* F15 key */
	GUI_WIN32_VK_F16		= 0x7F,	/* F16 key */
	GUI_WIN32_VK_F17		= 0x80,	/* F17 key */
	GUI_WIN32_VK_F18		= 0x81,	/* F18 key */
	GUI_WIN32_VK_F19		= 0x82,	/* F19 key */
	GUI_WIN32_VK_F20		= 0x83,	/* F20 key */
	GUI_WIN32_VK_F21		= 0x84,	/* F21 key */
	GUI_WIN32_VK_F22		= 0x85,	/* F22 key */
	GUI_WIN32_VK_F23		= 0x86,	/* F23 key */
	GUI_WIN32_VK_F24		= 0x87,	/* F24 key */
	/* -	0x88-8F	Unassigned */
	GUI_WIN32_VK_NUMLOCK	= 0x90,	/* NUM LOCK key */
	GUI_WIN32_VK_SCROLL		= 0x91,	/* SCROLL LOCK key */
	/* 0x92-96	OEM specific */
	/* -	0x97-9F	Unassigned */
	GUI_WIN32_VK_LSHIFT					= 0xA0,	/* Left SHIFT key */
	GUI_WIN32_VK_RSHIFT					= 0xA1,	/* Right SHIFT key */
	GUI_WIN32_VK_LCONTROL				= 0xA2,	/* Left CONTROL key */
	GUI_WIN32_VK_RCONTROL				= 0xA3,	/* Right CONTROL key */
	GUI_WIN32_VK_LMENU					= 0xA4,	/* Left MENU key */
	GUI_WIN32_VK_RMENU					= 0xA5,	/* Right MENU key */
	GUI_WIN32_VK_BROWSER_BACK			= 0xA6,	/* Browser Back key */
	GUI_WIN32_VK_BROWSER_FORWARD		= 0xA7,	/* Browser Forward key */
	GUI_WIN32_VK_BROWSER_REFRESH		= 0xA8,	/* Browser Refresh key */
	GUI_WIN32_VK_BROWSER_STOP			= 0xA9,	/* Browser Stop key */
	GUI_WIN32_VK_BROWSER_SEARCH			= 0xAA,	/* Browser Search key */
	GUI_WIN32_VK_BROWSER_FAVORITES		= 0xAB,	/* Browser Favorites key */
	GUI_WIN32_VK_BROWSER_HOME			= 0xAC,	/* Browser Start and Home key */
	GUI_WIN32_VK_VOLUME_MUTE			= 0xAD,	/* Volume Mute key */
	GUI_WIN32_VK_VOLUME_DOWN			= 0xAE,	/* Volume Down key */
	GUI_WIN32_VK_VOLUME_UP				= 0xAF,	/* Volume Up key */
	GUI_WIN32_VK_MEDIA_NEXT_TRACK		= 0xB0,	/* Next Track key */
	GUI_WIN32_VK_MEDIA_PREV_TRACK		= 0xB1,	/* Previous Track key */
	GUI_WIN32_VK_MEDIA_STOP				= 0xB2,	/* Stop Media key */
	GUI_WIN32_VK_MEDIA_PLAY_PAUSE		= 0xB3,	/* Play/Pause Media key */
	GUI_WIN32_VK_LAUNCH_MAIL			= 0xB4,	/* Start Mail key */
	GUI_WIN32_VK_LAUNCH_MEDIA_SELECT	= 0xB5,	/* Select Media key */
	GUI_WIN32_VK_LAUNCH_APP1			= 0xB6,	/* Start Application 1 key */
	GUI_WIN32_VK_LAUNCH_APP2			= 0xB7,	/* Start Application 2 key */
	/* -	0xB8-B9	Reserved */
	GUI_WIN32_VK_OEM_1		= 0xBA,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key */
	GUI_WIN32_VK_OEM_PLUS	= 0xBB,	/* For any country/region, the '+' key */
	GUI_WIN32_VK_OEM_COMMA	= 0xBC,	/* For any country/region, the ',' key */
	GUI_WIN32_VK_OEM_MINUS	= 0xBD,	/* For any country/region, the '-' key */
	GUI_WIN32_VK_OEM_PERIOD = 0xBE,	/* For any country/region, the '.' key */
	GUI_WIN32_VK_OEM_2		= 0xBF,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key */
	GUI_WIN32_VK_OEM_3		= 0xC0,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key */
	/* -	0xC1-D7	Reserved */
	/* -	0xD8-DA	Unassigned */
	GUI_WIN32_VK_OEM_4		= 0xDB,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key */
	GUI_WIN32_VK_OEM_5		= 0xDC,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key */
	GUI_WIN32_VK_OEM_6		= 0xDD,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key */
	GUI_WIN32_VK_OEM_7		= 0xDE,	/* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key */
	GUI_WIN32_VK_OEM_8		= 0xDF,	/* Used for miscellaneous characters; it can vary by keyboard. */
	/* -	0xE0	Reserved */
	/* 0xE1	OEM specific */
	GUI_WIN32_VK_OEM_102	= 0xE2,	/* The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard */
	/* 0xE3-E4	OEM specific */
	GUI_WIN32_VK_PROCESSKEY = 0xE5,	/* IME PROCESS key */
	/* 0xE6	OEM specific */
	GUI_WIN32_VK_PACKET		= 0xE7,	/* Used to pass Unicode characters as if they were keystrokes. The GUI_WIN32_VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP */
	/* -	0xE8	Unassigned */
	/* 0xE9-F5	OEM specific */
	GUI_WIN32_VK_ATTN		= 0xF6,	/* Attn key */
	GUI_WIN32_VK_CRSEL		= 0xF7,	/* CrSel key */
	GUI_WIN32_VK_EXSEL		= 0xF8,	/* ExSel key */
	GUI_WIN32_VK_EREOF		= 0xF9,	/* Erase EOF key */
	GUI_WIN32_VK_PLAY		= 0xFA,	/* Play key */
	GUI_WIN32_VK_ZOOM		= 0xFB,	/* Zoom key */
	GUI_WIN32_VK_NONAME		= 0xFC,	/* Reserved */
	GUI_WIN32_VK_PA1		= 0xFD,	/* PA1 key */
	GUI_WIN32_VK_OEM_CLEAR	= 0xFE,	/* Clear key */

	GUI_VK_WIN32_LAST
};

/* Look-up table to find our scancode from win32 scancode. */
static const gui_scancode_t win32_scancode_lut[] = {
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_ESCAPE,
	GUI_SCANCODE_1,
	GUI_SCANCODE_2,
	GUI_SCANCODE_3,
	GUI_SCANCODE_4,
	GUI_SCANCODE_5,
	GUI_SCANCODE_6,
	GUI_SCANCODE_7,
	GUI_SCANCODE_8,
	GUI_SCANCODE_9,
	GUI_SCANCODE_0,
	GUI_SCANCODE_MINUS,
	GUI_SCANCODE_EQUALS,
	GUI_SCANCODE_BACKSPACE,
	GUI_SCANCODE_TAB,
	GUI_SCANCODE_Q,
	GUI_SCANCODE_W,
	GUI_SCANCODE_E,
	GUI_SCANCODE_R,
	GUI_SCANCODE_T,
	GUI_SCANCODE_Y,
	GUI_SCANCODE_U,
	GUI_SCANCODE_I,
	GUI_SCANCODE_O,
	GUI_SCANCODE_P,
	GUI_SCANCODE_LEFTBRACKET,
	GUI_SCANCODE_RIGHTBRACKET,
	GUI_SCANCODE_RETURN,
	GUI_SCANCODE_LCTRL,
	GUI_SCANCODE_A,
	GUI_SCANCODE_S,
	GUI_SCANCODE_D,
	GUI_SCANCODE_F,
	GUI_SCANCODE_G,
	GUI_SCANCODE_H,
	GUI_SCANCODE_J,
	GUI_SCANCODE_K,
	GUI_SCANCODE_L,
	GUI_SCANCODE_SEMICOLON,
	GUI_SCANCODE_APOSTROPHE,
	GUI_SCANCODE_GRAVE,
	GUI_SCANCODE_LSHIFT,
	GUI_SCANCODE_BACKSLASH,
	GUI_SCANCODE_Z,
	GUI_SCANCODE_X,
	GUI_SCANCODE_C,
	GUI_SCANCODE_V,
	GUI_SCANCODE_B,
	GUI_SCANCODE_N,
	GUI_SCANCODE_M,
	GUI_SCANCODE_COMMA,
	GUI_SCANCODE_PERIOD,
	GUI_SCANCODE_SLASH,
	GUI_SCANCODE_RSHIFT,
	GUI_SCANCODE_PRINTSCREEN,
	GUI_SCANCODE_LALT,
	GUI_SCANCODE_SPACE,
	GUI_SCANCODE_CAPSLOCK,
	GUI_SCANCODE_F1,
	GUI_SCANCODE_F2,
	GUI_SCANCODE_F3,
	GUI_SCANCODE_F4,
	GUI_SCANCODE_F5,
	GUI_SCANCODE_F6,
	GUI_SCANCODE_F7,
	GUI_SCANCODE_F8,
	GUI_SCANCODE_F9,
	GUI_SCANCODE_F10,
	GUI_SCANCODE_NUMLOCKCLEAR,
	GUI_SCANCODE_SCROLLLOCK,
	GUI_SCANCODE_HOME,
	GUI_SCANCODE_UP,
	GUI_SCANCODE_PAGEUP,
	GUI_SCANCODE_KP_MINUS,
	GUI_SCANCODE_LEFT,
	GUI_SCANCODE_KP_5,
	GUI_SCANCODE_RIGHT,
	GUI_SCANCODE_KP_PLUS,
	GUI_SCANCODE_END,
	GUI_SCANCODE_DOWN,
	GUI_SCANCODE_PAGEDOWN,
	GUI_SCANCODE_INSERT,
	GUI_SCANCODE_DELETE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONUSBACKSLASH,
	GUI_SCANCODE_F11,
	GUI_SCANCODE_F12,
	GUI_SCANCODE_PAUSE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_LGUI,
	GUI_SCANCODE_RGUI,
	GUI_SCANCODE_APPLICATION,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_F13,
	GUI_SCANCODE_F14,
	GUI_SCANCODE_F15,
	GUI_SCANCODE_F16,
	GUI_SCANCODE_F17,
	GUI_SCANCODE_F18,
	GUI_SCANCODE_F19,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_INTERNATIONAL2,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_INTERNATIONAL1,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_INTERNATIONAL4,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_INTERNATIONAL5,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_INTERNATIONAL3,
	GUI_SCANCODE_NONE,
	GUI_SCANCODE_NONE
};

struct gui_vkey_state_win32 {
	s8 Reserved0 : 8;
	s8 Back : 1; // VK_BACK, 0x8
	s8 Tab : 1;  // VK_TAB, 0x9
	s8 Reserved1 : 3;
	s8 Enter : 1; // VK_RETURN, 0xD
	s8 Reserved2 : 2;
	s8 Reserved3 : 3;
	s8 Pause : 1;    // VK_PAUSE, 0x13
	s8 CapsLock : 1; // VK_CAPITAL, 0x14
	s8 Kana : 1;     // VK_KANA, 0x15
	s8 Reserved4 : 2;
	s8 Reserved5 : 1;
	s8 Kanji : 1; // VK_KANJI, 0x19
	s8 Reserved6 : 1;
	s8 Escape : 1;       // VK_ESCAPE, 0x1B
	s8 ImeConvert : 1;   // VK_CONVERT, 0x1C
	s8 ImeNoConvert : 1; // VK_NONCONVERT, 0x1D
	s8 Reserved7 : 2;
	s8 Space : 1;       // VK_SPACE, 0x20
	s8 PageUp : 1;      // VK_PRIOR, 0x21
	s8 PageDown : 1;    // VK_NEXT, 0x22
	s8 End : 1;         // VK_END, 0x23
	s8 Home : 1;        // VK_HOME, 0x24
	s8 Left : 1;        // VK_LEFT, 0x25
	s8 Up : 1;          // VK_UP, 0x26
	s8 Right : 1;       // VK_RIGHT, 0x27
	s8 Down : 1;        // VK_DOWN, 0x28
	s8 Select : 1;      // VK_SELECT, 0x29
	s8 Print : 1;       // VK_PRINT, 0x2A
	s8 Execute : 1;     // VK_EXECUTE, 0x2B
	s8 PrintScreen : 1; // VK_SNAPSHOT, 0x2C
	s8 Insert : 1;      // VK_INSERT, 0x2D
	s8 Delete : 1;      // VK_DELETE, 0x2E
	s8 Help : 1;        // VK_HELP, 0x2F
	s8 D0 : 1;          // 0x30
	s8 D1 : 1;          // 0x31
	s8 D2 : 1;          // 0x32
	s8 D3 : 1;          // 0x33
	s8 D4 : 1;          // 0x34
	s8 D5 : 1;          // 0x35
	s8 D6 : 1;          // 0x36
	s8 D7 : 1;          // 0x37
	s8 D8 : 1;          // 0x38
	s8 D9 : 1;          // 0x39
	s8 Reserved8 : 6;
	s8 Reserved9 : 1;
	s8 A : 1;            // 0x41
	s8 B : 1;            // 0x42
	s8 C : 1;            // 0x43
	s8 D : 1;            // 0x44
	s8 E : 1;            // 0x45
	s8 F : 1;            // 0x46
	s8 G : 1;            // 0x47
	s8 H : 1;            // 0x48
	s8 I : 1;            // 0x49
	s8 J : 1;            // 0x4A
	s8 K : 1;            // 0x4B
	s8 L : 1;            // 0x4C
	s8 M : 1;            // 0x4D
	s8 N : 1;            // 0x4E
	s8 O : 1;            // 0x4F
	s8 P : 1;            // 0x50
	s8 Q : 1;            // 0x51
	s8 R : 1;            // 0x52
	s8 S : 1;            // 0x53
	s8 T : 1;            // 0x54
	s8 U : 1;            // 0x55
	s8 V : 1;            // 0x56
	s8 W : 1;            // 0x57
	s8 X : 1;            // 0x58
	s8 Y : 1;            // 0x59
	s8 Z : 1;            // 0x5A
	s8 LeftWindows : 1;  // VK_LWIN, 0x5B
	s8 RightWindows : 1; // VK_RWIN, 0x5C
	s8 Apps : 1;         // VK_APPS, 0x5D
	s8 Reserved10 : 1;
	s8 Sleep : 1;     // VK_SLEEP, 0x5F
	s8 NumPad0 : 1;   // VK_NUMPAD0, 0x60
	s8 NumPad1 : 1;   // VK_NUMPAD1, 0x61
	s8 NumPad2 : 1;   // VK_NUMPAD2, 0x62
	s8 NumPad3 : 1;   // VK_NUMPAD3, 0x63
	s8 NumPad4 : 1;   // VK_NUMPAD4, 0x64
	s8 NumPad5 : 1;   // VK_NUMPAD5, 0x65
	s8 NumPad6 : 1;   // VK_NUMPAD6, 0x66
	s8 NumPad7 : 1;   // VK_NUMPAD7, 0x67
	s8 NumPad8 : 1;   // VK_NUMPAD8, 0x68
	s8 NumPad9 : 1;   // VK_NUMPAD9, 0x69
	s8 Multiply : 1;  // VK_MULTIPLY, 0x6A
	s8 Add : 1;       // VK_ADD, 0x6B
	s8 Separator : 1; // VK_SEPARATOR, 0x6C
	s8 Subtract : 1;  // VK_SUBTRACT, 0x6D
	s8 Decimal : 1;   // VK_DECIMANL, 0x6E
	s8 Divide : 1;    // VK_DIVIDE, 0x6F
	s8 F1 : 1;        // VK_F1, 0x70
	s8 F2 : 1;        // VK_F2, 0x71
	s8 F3 : 1;        // VK_F3, 0x72
	s8 F4 : 1;        // VK_F4, 0x73
	s8 F5 : 1;        // VK_F5, 0x74
	s8 F6 : 1;        // VK_F6, 0x75
	s8 F7 : 1;        // VK_F7, 0x76
	s8 F8 : 1;        // VK_F8, 0x77
	s8 F9 : 1;        // VK_F9, 0x78
	s8 F10 : 1;       // VK_F10, 0x79
	s8 F11 : 1;       // VK_F11, 0x7A
	s8 F12 : 1;       // VK_F12, 0x7B
	s8 F13 : 1;       // VK_F13, 0x7C
	s8 F14 : 1;       // VK_F14, 0x7D
	s8 F15 : 1;       // VK_F15, 0x7E
	s8 F16 : 1;       // VK_F16, 0x7F
	s8 F17 : 1;       // VK_F17, 0x80
	s8 F18 : 1;       // VK_F18, 0x81
	s8 F19 : 1;       // VK_F19, 0x82
	s8 F20 : 1;       // VK_F20, 0x83
	s8 F21 : 1;       // VK_F21, 0x84
	s8 F22 : 1;       // VK_F22, 0x85
	s8 F23 : 1;       // VK_F23, 0x86
	s8 F24 : 1;       // VK_F24, 0x87
	s8 Reserved11 : 8;
	s8 NumLock : 1; // VK_NUMLOCK, 0x90
	s8 Scroll : 1;  // VK_SCROLL, 0x91
	s8 Reserved12 : 6;
	s8 Reserved13 : 8;
	s8 LeftShift : 1;          // VK_LSHIFT, 0xA0
	s8 RightShift : 1;         // VK_RSHIFT, 0xA1
	s8 LeftControl : 1;        // VK_LCONTROL, 0xA2
	s8 RightControl : 1;       // VK_RCONTROL, 0xA3
	s8 LeftAlt : 1;            // VK_LMENU, 0xA4
	s8 RightAlt : 1;           // VK_RMENU, 0xA5
	s8 BrowserBack : 1;        // VK_BROWSER_BACK, 0xA6
	s8 BrowserForward : 1;     // VK_BROWSER_FORWARD, 0xA7
	s8 BrowserRefresh : 1;     // VK_BROWSER_REFRESH, 0xA8
	s8 BrowserStop : 1;        // VK_BROWSER_STOP, 0xA9
	s8 BrowserSearch : 1;      // VK_BROWSER_SEARCH, 0xAA
	s8 BrowserFavorites : 1;   // VK_BROWSER_FAVORITES, 0xAB
	s8 BrowserHome : 1;        // VK_BROWSER_HOME, 0xAC
	s8 VolumeMute : 1;         // VK_VOLUME_MUTE, 0xAD
	s8 VolumeDown : 1;         // VK_VOLUME_DOWN, 0xAE
	s8 VolumeUp : 1;           // VK_VOLUME_UP, 0xAF
	s8 MediaNextTrack : 1;     // VK_MEDIA_NEXT_TRACK, 0xB0
	s8 MediaPreviousTrack : 1; // VK_MEDIA_PREV_TRACK, 0xB1
	s8 MediaStop : 1;          // VK_MEDIA_STOP, 0xB2
	s8 MediaPlayPause : 1;     // VK_MEDIA_PLAY_PAUSE, 0xB3
	s8 LaunchMail : 1;         // VK_LAUNCH_MAIL, 0xB4
	s8 SelectMedia : 1;        // VK_LAUNCH_MEDIA_SELECT, 0xB5
	s8 LaunchApplication1 : 1; // VK_LAUNCH_APP1, 0xB6
	s8 LaunchApplication2 : 1; // VK_LAUNCH_APP2, 0xB7
	s8 Reserved14 : 2;
	s8 OemSemicolon : 1; // VK_OEM_1, 0xBA
	s8 OemPlus : 1;      // VK_OEM_PLUS, 0xBB
	s8 OemComma : 1;     // VK_OEM_COMMA, 0xBC
	s8 OemMinus : 1;     // VK_OEM_MINUS, 0xBD
	s8 OemPeriod : 1;    // VK_OEM_PERIOD, 0xBE
	s8 OemQuestion : 1;  // VK_OEM_2, 0xBF
	s8 OemTilde : 1;     // VK_OEM_3, 0xC0
	s8 Reserved15 : 7;
	s8 Reserved16 : 8;
	s8 Reserved17 : 8;
	s8 Reserved18 : 3;
	s8 OemOpenBrackets : 1;  // VK_OEM_4, 0xDB
	s8 OemPipe : 1;          // VK_OEM_5, 0xDC
	s8 OemCloseBrackets : 1; // VK_OEM_6, 0xDD
	s8 OemQuotes : 1;        // VK_OEM_7, 0xDE
	s8 Oem8 : 1;             // VK_OEM_8, 0xDF
	s8 Reserved19 : 2;
	s8 OemBackslash : 1; // VK_OEM_102, 0xE2
	s8 Reserved20 : 2;
	s8 ProcessKey : 1; // VK_PROCESSKEY, 0xE5
	s8 Reserved21 : 2;
	s8 Reserved22 : 8;
	s8 Reserved23 : 2;
	s8 OemCopy : 1; // 0XF2
	s8 OemAuto : 1; // 0xF3
	s8 OemEnlW : 1; // 0xF4
	s8 Reserved24 : 1;
	s8 Attn : 1;     // VK_ATTN, 0xF6
	s8 Crsel : 1;    // VK_CRSEL, 0xF7
	s8 Exsel : 1;    // VK_EXSEL, 0xF8
	s8 EraseEof : 1; // VK_EREOF, 0xF9
	s8 Play : 1;     // VK_PLAY, 0xFA
	s8 Zoom : 1;     // VK_ZOOM, 0xFB
	s8 Reserved25 : 1;
	s8 Pa1 : 1;      // VK_PA1, 0xFD
	s8 OemClear : 1; // VK_OEM_CLEAR, 0xFE
	s8 Reserved26 : 1;
};

#endif
