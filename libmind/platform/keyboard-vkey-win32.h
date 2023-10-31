#pragma once

#include "core/types.h"
#include "platform/keyboard-scancode.h"

enum vkey_win32 {
	VKEY_WIN32_NONE = 0x00,
	VKEY_WIN32_FIRST = VKEY_WIN32_NONE,
	VKEY_WIN32_LBUTTON = 0x01, /* Left mouse button */
	VKEY_WIN32_RBUTTON = 0x02, /* Right mouse button */
	VKEY_WIN32_CANCEL = 0x03,  /* Control-break processing */
	VKEY_WIN32_MBUTTON =
		0x04, /* Middle mouse button (three-button mouse) */
	VKEY_WIN32_XBUTTON1 = 0x05, /* X1 mouse button */
	VKEY_WIN32_XBUTTON2 = 0x06, /* X2 mouse button */
	/* -	0x07	Undefined */
	VKEY_WIN32_BACK = 0x08, /* BACKSPACE key */
	VKEY_WIN32_TAB = 0x09,  /* TAB key */
	/* -	0x0A-0B	Reserved */
	VKEY_WIN32_CLEAR = 0x0C,  /* CLEAR key */
	VKEY_WIN32_RETURN = 0x0D, /* ENTER key */
	/* -	0x0E-0F	Undefined */
	VKEY_WIN32_SHIFT = 0x10,   /* SHIFT key */
	VKEY_WIN32_CONTROL = 0x11, /* CTRL key */
	VKEY_WIN32_MENU = 0x12,    /* ALT key */
	VKEY_WIN32_PAUSE = 0x13,   /* PAUSE key */
	VKEY_WIN32_CAPITAL = 0x14, /* CAPS LOCK key */
	VKEY_WIN32_KANA = 0x15,    /* IME Kana mode */
	VKEY_WIN32_HANGUEL =
		0x15, /* IME Hanguel mode (maintained for compatibility; use VKEY_WIN32_HANGUL) */
	VKEY_WIN32_HANGUL = 0x15,     /* IME Hangul mode */
	VKEY_WIN32_IME_ON = 0x16,     /* IME On */
	VKEY_WIN32_JUNJA = 0x17,      /* IME Junja mode */
	VKEY_WIN32_FINAL = 0x18,      /* IME final mode */
	VKEY_WIN32_HANJA = 0x19,      /* IME Hanja mode */
	VKEY_WIN32_KANJI = 0x19,      /* IME Kanji mode */
	VKEY_WIN32_IME_OFF = 0x1A,    /* IME Off */
	VKEY_WIN32_ESCAPE = 0x1B,     /* ESC key */
	VKEY_WIN32_CONVERT = 0x1C,    /* IME convert */
	VKEY_WIN32_NONCONVERT = 0x1D, /* IME nonconvert */
	VKEY_WIN32_ACCEPT = 0x1E,     /* IME accept */
	VKEY_WIN32_MODECHANGE = 0x1F, /* IME mode change request */
	VKEY_WIN32_SPACE = 0x20,      /* SPACEBAR */
	VKEY_WIN32_PRIOR = 0x21,      /* PAGE UP key */
	VKEY_WIN32_NEXT = 0x22,       /* PAGE DOWN key */
	VKEY_WIN32_END = 0x23,        /* END key */
	VKEY_WIN32_HOME = 0x24,       /* HOME key */
	VKEY_WIN32_LEFT = 0x25,       /* LEFT ARROW key */
	VKEY_WIN32_UP = 0x26,         /* UP ARROW key */
	VKEY_WIN32_RIGHT = 0x27,      /* RIGHT ARROW key */
	VKEY_WIN32_DOWN = 0x28,       /* DOWN ARROW key */
	VKEY_WIN32_SELECT = 0x29,     /* SELECT key */
	VKEY_WIN32_PRINT = 0x2A,      /* PRINT key */
	VKEY_WIN32_EXECUTE = 0x2B,    /* EXECUTE key */
	VKEY_WIN32_SNAPSHOT = 0x2C,   /* PRINT SCREEN key */
	VKEY_WIN32_INSERT = 0x2D,     /* INS key */
	VKEY_WIN32_DELETE = 0x2E,     /* DEL key */
	VKEY_WIN32_HELP = 0x2F,       /* HELP key */
	VKEY_WIN32_0 = 0x30,          /* 0 key */
	VKEY_WIN32_1 = 0x31,          /* 1 key */
	VKEY_WIN32_2 = 0x32,          /* 2 key */
	VKEY_WIN32_3 = 0x33,          /* 3 key */
	VKEY_WIN32_4 = 0x34,          /* 4 key */
	VKEY_WIN32_5 = 0x35,          /* 5 key */
	VKEY_WIN32_6 = 0x36,          /* 6 key */
	VKEY_WIN32_7 = 0x37,          /* 7 key */
	VKEY_WIN32_8 = 0x38,          /* 8 key */
	VKEY_WIN32_9 = 0x39,          /* 9 key */
	/* -	0x3A-40	Undefined */
	VKEY_WIN32_A = 0x41,    /* A key */
	VKEY_WIN32_B = 0x42,    /* B key */
	VKEY_WIN32_C = 0x43,    /* C key */
	VKEY_WIN32_D = 0x44,    /* D key */
	VKEY_WIN32_E = 0x45,    /* E key */
	VKEY_WIN32_F = 0x46,    /* F key */
	VKEY_WIN32_G = 0x47,    /* G key */
	VKEY_WIN32_H = 0x48,    /* H key */
	VKEY_WIN32_I = 0x49,    /* I key */
	VKEY_WIN32_J = 0x4A,    /* J key */
	VKEY_WIN32_K = 0x4B,    /* K key */
	VKEY_WIN32_L = 0x4C,    /* L key */
	VKEY_WIN32_M = 0x4D,    /* M key */
	VKEY_WIN32_N = 0x4E,    /* N key */
	VKEY_WIN32_O = 0x4F,    /* O key */
	VKEY_WIN32_P = 0x50,    /* P key */
	VKEY_WIN32_Q = 0x51,    /* Q key */
	VKEY_WIN32_R = 0x52,    /* R key */
	VKEY_WIN32_S = 0x53,    /* S key */
	VKEY_WIN32_T = 0x54,    /* T key */
	VKEY_WIN32_U = 0x55,    /* U key */
	VKEY_WIN32_V = 0x56,    /* V key */
	VKEY_WIN32_W = 0x57,    /* W key */
	VKEY_WIN32_X = 0x58,    /* X key */
	VKEY_WIN32_Y = 0x59,    /* Y key */
	VKEY_WIN32_Z = 0x5A,    /* Z key */
	VKEY_WIN32_LWIN = 0x5B, /* Left Windows key (Natural keyboard) */
	VKEY_WIN32_RWIN = 0x5C, /* Right Windows key (Natural keyboard) */
	VKEY_WIN32_APPS = 0x5D, /* Applications key (Natural keyboard) */
	/* -	0x5E	Reserved */
	VKEY_WIN32_SLEEP = 0x5F,     /*, Computer Sleep key */
	VKEY_WIN32_NUMPAD0 = 0x60,   /* Numeric, keypad 0 key */
	VKEY_WIN32_NUMPAD1 = 0x61,   /* Numeric, keypad 1 key */
	VKEY_WIN32_NUMPAD2 = 0x62,   /* Numeric, keypad 2 key */
	VKEY_WIN32_NUMPAD3 = 0x63,   /* Numeric, keypad 3 key */
	VKEY_WIN32_NUMPAD4 = 0x64,   /* Numeric, keypad 4 key */
	VKEY_WIN32_NUMPAD5 = 0x65,   /* Numeric, keypad 5 key */
	VKEY_WIN32_NUMPAD6 = 0x66,   /* Numeric, keypad 6 key */
	VKEY_WIN32_NUMPAD7 = 0x67,   /* Numeric, keypad 7 key */
	VKEY_WIN32_NUMPAD8 = 0x68,   /* Numeric, keypad 8 key */
	VKEY_WIN32_NUMPAD9 = 0x69,   /* Numeric, keypad 9 key */
	VKEY_WIN32_MULTIPLY = 0x6A,  /* Multiply key */
	VKEY_WIN32_ADD = 0x6B,       /* Add key */
	VKEY_WIN32_SEPARATOR = 0x6C, /* Separator key */
	VKEY_WIN32_SUBTRACT = 0x6D,  /* Subtract key */
	VKEY_WIN32_DECIMAL = 0x6E,   /* Decimal key */
	VKEY_WIN32_DIVIDE = 0x6F,    /* Divide key */
	VKEY_WIN32_F1 = 0x70,        /* F1 key */
	VKEY_WIN32_F2 = 0x71,        /* F2 key */
	VKEY_WIN32_F3 = 0x72,        /* F3 key */
	VKEY_WIN32_F4 = 0x73,        /* F4 key */
	VKEY_WIN32_F5 = 0x74,        /* F5 key */
	VKEY_WIN32_F6 = 0x75,        /* F6 key */
	VKEY_WIN32_F7 = 0x76,        /* F7 key */
	VKEY_WIN32_F8 = 0x77,        /* F8 key */
	VKEY_WIN32_F9 = 0x78,        /* F9 key */
	VKEY_WIN32_F10 = 0x79,       /* F10 key */
	VKEY_WIN32_F11 = 0x7A,       /* F11 key */
	VKEY_WIN32_F12 = 0x7B,       /* F12 key */
	VKEY_WIN32_F13 = 0x7C,       /* F13 key */
	VKEY_WIN32_F14 = 0x7D,       /* F14 key */
	VKEY_WIN32_F15 = 0x7E,       /* F15 key */
	VKEY_WIN32_F16 = 0x7F,       /* F16 key */
	VKEY_WIN32_F17 = 0x80,       /* F17 key */
	VKEY_WIN32_F18 = 0x81,       /* F18 key */
	VKEY_WIN32_F19 = 0x82,       /* F19 key */
	VKEY_WIN32_F20 = 0x83,       /* F20 key */
	VKEY_WIN32_F21 = 0x84,       /* F21 key */
	VKEY_WIN32_F22 = 0x85,       /* F22 key */
	VKEY_WIN32_F23 = 0x86,       /* F23 key */
	VKEY_WIN32_F24 = 0x87,       /* F24 key */
	/* -	0x88-8F	Unassigned */
	VKEY_WIN32_NUMLOCK = 0x90, /* NUM LOCK key */
	VKEY_WIN32_SCROLL = 0x91,  /* SCROLL LOCK key */
	/* 0x92-96	OEM specific */
	/* -	0x97-9F	Unassigned */
	VKEY_WIN32_LSHIFT = 0xA0,              /* Left SHIFT key */
	VKEY_WIN32_RSHIFT = 0xA1,              /* Right SHIFT key */
	VKEY_WIN32_LCONTROL = 0xA2,            /* Left CONTROL key */
	VKEY_WIN32_RCONTROL = 0xA3,            /* Right CONTROL key */
	VKEY_WIN32_LMENU = 0xA4,               /* Left MENU key */
	VKEY_WIN32_RMENU = 0xA5,               /* Right MENU key */
	VKEY_WIN32_BROWSER_BACK = 0xA6,        /* Browser Back key */
	VKEY_WIN32_BROWSER_FORWARD = 0xA7,     /* Browser Forward key */
	VKEY_WIN32_BROWSER_REFRESH = 0xA8,     /* Browser Refresh key */
	VKEY_WIN32_BROWSER_STOP = 0xA9,        /* Browser Stop key */
	VKEY_WIN32_BROWSER_SEARCH = 0xAA,      /* Browser Search key */
	VKEY_WIN32_BROWSER_FAVORITES = 0xAB,   /* Browser Favorites key */
	VKEY_WIN32_BROWSER_HOME = 0xAC,        /* Browser Start and Home key */
	VKEY_WIN32_VOLUME_MUTE = 0xAD,         /* Volume Mute key */
	VKEY_WIN32_VOLUME_DOWN = 0xAE,         /* Volume Down key */
	VKEY_WIN32_VOLUME_UP = 0xAF,           /* Volume Up key */
	VKEY_WIN32_MEDIA_NEXT_TRACK = 0xB0,    /* Next Track key */
	VKEY_WIN32_MEDIA_PREV_TRACK = 0xB1,    /* Previous Track key */
	VKEY_WIN32_MEDIA_STOP = 0xB2,          /* Stop Media key */
	VKEY_WIN32_MEDIA_PLAY_PAUSE = 0xB3,    /* Play/Pause Media key */
	VKEY_WIN32_LAUNCH_MAIL = 0xB4,         /* Start Mail key */
	VKEY_WIN32_LAUNCH_MEDIA_SELECT = 0xB5, /* Select Media key */
	VKEY_WIN32_LAUNCH_APP1 = 0xB6,         /* Start Application 1 key */
	VKEY_WIN32_LAUNCH_APP2 = 0xB7,         /* Start Application 2 key */
	/* -	0xB8-B9	Reserved */
	VKEY_WIN32_OEM_1 =
		0xBA, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key */
	VKEY_WIN32_OEM_PLUS = 0xBB,   /* For any country/region, the '+' key */
	VKEY_WIN32_OEM_COMMA = 0xBC,  /* For any country/region, the ',' key */
	VKEY_WIN32_OEM_MINUS = 0xBD,  /* For any country/region, the '-' key */
	VKEY_WIN32_OEM_PERIOD = 0xBE, /* For any country/region, the '.' key */
	VKEY_WIN32_OEM_2 =
		0xBF, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key */
	VKEY_WIN32_OEM_3 =
		0xC0, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key */
	/* -	0xC1-D7	Reserved */
	/* -	0xD8-DA	Unassigned */
	VKEY_WIN32_OEM_4 =
		0xDB, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key */
	VKEY_WIN32_OEM_5 =
		0xDC, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key */
	VKEY_WIN32_OEM_6 =
		0xDD, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key */
	VKEY_WIN32_OEM_7 =
		0xDE, /* Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key */
	VKEY_WIN32_OEM_8 =
		0xDF, /* Used for miscellaneous characters; it can vary by keyboard. */
	/* -	0xE0	Reserved */
	/* 0xE1	OEM specific */
	VKEY_WIN32_OEM_102 =
		0xE2, /* The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard */
	/* 0xE3-E4	OEM specific */
	VKEY_WIN32_PROCESSKEY = 0xE5, /* IME PROCESS key */
	/* 0xE6	OEM specific */
	VKEY_WIN32_PACKET =
		0xE7, /* Used to pass Unicode characters as if they were keystrokes. The VKEY_WIN32_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP */
	/* -	0xE8	Unassigned */
	/* 0xE9-F5	OEM specific */
	VKEY_WIN32_ATTN = 0xF6,      /* Attn key */
	VKEY_WIN32_CRSEL = 0xF7,     /* CrSel key */
	VKEY_WIN32_EXSEL = 0xF8,     /* ExSel key */
	VKEY_WIN32_EREOF = 0xF9,     /* Erase EOF key */
	VKEY_WIN32_PLAY = 0xFA,      /* Play key */
	VKEY_WIN32_ZOOM = 0xFB,      /* Zoom key */
	VKEY_WIN32_NONAME = 0xFC,    /* Reserved */
	VKEY_WIN32_PA1 = 0xFD,       /* PA1 key */
	VKEY_WIN32_OEM_CLEAR = 0xFE, /* Clear key */

	VK_WIN32_LAST
};

/* Look-up table to find our scancode from win32 scancode. */
static const keyboard_scancode_t win32_scancode_lut[] = {
	SCANCODE_NONE,
	SCANCODE_ESCAPE,
	SCANCODE_1,
	SCANCODE_2,
	SCANCODE_3,
	SCANCODE_4,
	SCANCODE_5,
	SCANCODE_6,
	SCANCODE_7,
	SCANCODE_8,
	SCANCODE_9,
	SCANCODE_0,
	SCANCODE_MINUS,
	SCANCODE_EQUALS,
	SCANCODE_BACKSPACE,
	SCANCODE_TAB,
	SCANCODE_Q,
	SCANCODE_W,
	SCANCODE_E,
	SCANCODE_R,
	SCANCODE_T,
	SCANCODE_Y,
	SCANCODE_U,
	SCANCODE_I,
	SCANCODE_O,
	SCANCODE_P,
	SCANCODE_LEFTBRACKET,
	SCANCODE_RIGHTBRACKET,
	SCANCODE_RETURN,
	SCANCODE_LCTRL,
	SCANCODE_A,
	SCANCODE_S,
	SCANCODE_D,
	SCANCODE_F,
	SCANCODE_G,
	SCANCODE_H,
	SCANCODE_J,
	SCANCODE_K,
	SCANCODE_L,
	SCANCODE_SEMICOLON,
	SCANCODE_APOSTROPHE,
	SCANCODE_GRAVE,
	SCANCODE_LSHIFT,
	SCANCODE_BACKSLASH,
	SCANCODE_Z,
	SCANCODE_X,
	SCANCODE_C,
	SCANCODE_V,
	SCANCODE_B,
	SCANCODE_N,
	SCANCODE_M,
	SCANCODE_COMMA,
	SCANCODE_PERIOD,
	SCANCODE_SLASH,
	SCANCODE_RSHIFT,
	SCANCODE_PRINTSCREEN,
	SCANCODE_LALT,
	SCANCODE_SPACE,
	SCANCODE_CAPSLOCK,
	SCANCODE_F1,
	SCANCODE_F2,
	SCANCODE_F3,
	SCANCODE_F4,
	SCANCODE_F5,
	SCANCODE_F6,
	SCANCODE_F7,
	SCANCODE_F8,
	SCANCODE_F9,
	SCANCODE_F10,
	SCANCODE_NUMLOCKCLEAR,
	SCANCODE_SCROLLLOCK,
	SCANCODE_HOME,
	SCANCODE_UP,
	SCANCODE_PAGEUP,
	SCANCODE_KP_MINUS,
	SCANCODE_LEFT,
	SCANCODE_KP_5,
	SCANCODE_RIGHT,
	SCANCODE_KP_PLUS,
	SCANCODE_END,
	SCANCODE_DOWN,
	SCANCODE_PAGEDOWN,
	SCANCODE_INSERT,
	SCANCODE_DELETE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONUSBACKSLASH,
	SCANCODE_F11,
	SCANCODE_F12,
	SCANCODE_PAUSE,
	SCANCODE_NONE,
	SCANCODE_LGUI,
	SCANCODE_RGUI,
	SCANCODE_APPLICATION,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_F13,
	SCANCODE_F14,
	SCANCODE_F15,
	SCANCODE_F16,
	SCANCODE_F17,
	SCANCODE_F18,
	SCANCODE_F19,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_INTERNATIONAL2,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_INTERNATIONAL1,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_NONE,
	SCANCODE_INTERNATIONAL4,
	SCANCODE_NONE,
	SCANCODE_INTERNATIONAL5,
	SCANCODE_NONE,
	SCANCODE_INTERNATIONAL3,
	SCANCODE_NONE,
	SCANCODE_NONE};

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
