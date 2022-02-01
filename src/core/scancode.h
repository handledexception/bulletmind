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

// NOTE(paulh): This code was adopted from the SDL2 library

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
	kScancodeUnknown = 0,

	/**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
	/* @{ */

	kScancodeA = 4,
	kScancodeB = 5,
	kScancodeC = 6,
	kScancodeD = 7,
	kScancodeE = 8,
	kScancodeF = 9,
	kScancodeG = 10,
	kScancodeH = 11,
	kScancodeI = 12,
	kScancodeJ = 13,
	kScancodeK = 14,
	kScancodeL = 15,
	kScancodeM = 16,
	kScancodeN = 17,
	kScancodeO = 18,
	kScancodeP = 19,
	kScancodeQ = 20,
	kScancodeR = 21,
	kScancodeS = 22,
	kScancodeT = 23,
	kScancodeU = 24,
	kScancodeV = 25,
	kScancodeW = 26,
	kScancodeX = 27,
	kScancodeY = 28,
	kScancodeZ = 29,

	kScancode1 = 30,
	kScancode2 = 31,
	kScancode3 = 32,
	kScancode4 = 33,
	kScancode5 = 34,
	kScancode6 = 35,
	kScancode7 = 36,
	kScancode8 = 37,
	kScancode9 = 38,
	kScancode0 = 39,

	kScancodeReturn = 40,
	kScancodeEscape = 41,
	kScancodeBackspace = 42,
	kScancodeTab = 43,
	kScancodeSpace = 44,

	kScancodeMinus = 45,
	kScancodeEquals = 46,
	kScancodeLeftBracket = 47,
	kScancodeRightBracket = 48,
	kScancodeBackslash = 49, /**< Located at the lower left of the return
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
	kScancodeNonUSHash = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate kScancodeBACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
	kScancodeSemicolon = 51,
	kScancodeApostrophe = 52,
	kScancodeGrave = 53, /**< Located in the top left corner (on both ANSI
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
	kScancodeComma = 54,
	kScancodePeriod = 55,
	kScancodeSlash = 56,

	kScancodeCapsLock = 57,

	kScancodeF1 = 58,
	kScancodeF2 = 59,
	kScancodeF3 = 60,
	kScancodeF4 = 61,
	kScancodeF5 = 62,
	kScancodeF6 = 63,
	kScancodeF7 = 64,
	kScancodeF8 = 65,
	kScancodeF9 = 66,
	kScancodeF10 = 67,
	kScancodeF11 = 68,
	kScancodeF12 = 69,

	kScancodePrintScreen = 70,
	kScancodeScrollLock = 71,
	kScancodePause = 72,
	kScancodeInsert =
		73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
	kScancodeHome = 74,
	kScancodePageUp = 75,
	kScancodeDelete = 76,
	kScancodeEnd = 77,
	kScancodePageDown = 78,
	kScancodeRight = 79,
	kScancodeLeft = 80,
	kScancodeDown = 81,
	kScancodeUp = 82,

	kScancodeNumLockClear = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
	kScancodeKeypadDivide = 84,
	kScancodeKeypadMultiply = 85,
	kScancodeKeypadMinus = 86,
	kScancodeKeypadPlus = 87,
	kScancodeKeypadEnter = 88,
	kScancodeKeypad1 = 89,
	kScancodeKeypad2 = 90,
	kScancodeKeypad3 = 91,
	kScancodeKeypad4 = 92,
	kScancodeKeypad5 = 93,
	kScancodeKeypad6 = 94,
	kScancodeKeypad7 = 95,
	kScancodeKeypad8 = 96,
	kScancodeKeypad9 = 97,
	kScancodeKeypad0 = 98,
	kScancodeKeypadPeriod = 99,

	kScancodeNonUSBackslash = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
	kScancodeApplication = 101,    /**< windows contextual menu, compose */
	kScancodePower = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
	kScancodeKeypadEquals = 103,
	kScancodeF13 = 104,
	kScancodeF14 = 105,
	kScancodeF15 = 106,
	kScancodeF16 = 107,
	kScancodeF17 = 108,
	kScancodeF18 = 109,
	kScancodeF19 = 110,
	kScancodeF20 = 111,
	kScancodeF21 = 112,
	kScancodeF22 = 113,
	kScancodeF23 = 114,
	kScancodeF24 = 115,
	kScancodeExecute = 116,
	kScancodeHelp = 117,
	kScancodeMenu = 118,
	kScancodeSelect = 119,
	kScancodeStop = 120,
	kScancodeAgain = 121, /**< redo */
	kScancodeUndo = 122,
	kScancodeCut = 123,
	kScancodeCopt = 124,
	kScancodePaste = 125,
	kScancodeFind = 126,
	kScancodeMute = 127,
	kScancodeVolumeUp = 128,
	kScancodeVolumeDown = 129,
	/* not sure whether there's a reason to enable these */
	/*     kScancodeLOCKINGCAPSLOCK = 130,  */
	/*     kScancodeLOCKINGNUMLOCK = 131, */
	/*     kScancodeLOCKINGSCROLLLOCK = 132, */
	kScancodeKeypadComma = 133,
	kScancodeKeypadEqualsAs400 = 134,

	kScancodeInternational1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
	kScancodeInternational2 = 136,
	kScancodeInternational3 = 137, /**< Yen */
	kScancodeInternational4 = 138,
	kScancodeInternational5 = 139,
	kScancodeInternational6 = 140,
	kScancodeInternational7 = 141,
	kScancodeInternational8 = 142,
	kScancodeInternational9 = 143,
	kScancodeLang1 = 144, /**< Hangul/English toggle */
	kScancodeLang2 = 145, /**< Hanja conversion */
	kScancodeLang3 = 146, /**< Katakana */
	kScancodeLang4 = 147, /**< Hiragana */
	kScancodeLang5 = 148, /**< Zenkaku/Hankaku */
	kScancodeLang6 = 149, /**< reserved */
	kScancodeLang7 = 150, /**< reserved */
	kScancodeLang8 = 151, /**< reserved */
	kScancodeLang9 = 152, /**< reserved */

	kScancodeAltErase = 153, /**< Erase-Eaze */
	kScancodeSysReq = 154,
	kScancodeCancel = 155,
	kScancodeClear = 156,
	kScancodePrior = 157,
	kScancodeReturn2 = 158,
	kScancodeSeparator = 159,
	kScancodeOut = 160,
	kScancodeOper = 161,
	kScancodeClearAgain = 162,
	kScancodeCRSEL = 163,
	kScancodeEXSEL = 164,

	kScancodeKP_00 = 176,
	kScancodeKP_000 = 177,
	kScancodeThousandSeparator = 178,
	kScancodeDecimalSeparator = 179,
	kScancodeCurrencyUnit = 180,
	kScancodeCurrencySubUnit = 181,
	kScancodeKeypadLeftParen = 182,
	kScancodeKeypadRightParen = 183,
	kScancodeKeypadLeftBrace = 184,
	kScancodeKeypadRightBrace = 185,
	kScancodeKeypadTab = 186,
	kScancodeKeypadBackspace = 187,
	kScancodeKeypadA = 188,
	kScancodeKeypadB = 189,
	kScancodeKeypadC = 190,
	kScancodeKeypadD = 191,
	kScancodeKeypadE = 192,
	kScancodeKeypadF = 193,
	kScancodeKeypadXOR = 194,
	kScancodeKeypadPower = 195,
	kScancodeKeypadPercent = 196,
	kScancodeKeypadLess = 197,
	kScancodeKeypadGreater = 198,
	kScancodeKeypadAmpersand = 199,
	kScancodeKeypadDoubleAmpersand = 200,
	kScancodeKeypadVerticalBar = 201,
	kScancodeKeypadDoubleVerticalBar = 202,
	kScancodeKeypadColon = 203,
	kScancodeKeypadHash = 204,
	kScancodeKeypadSpace = 205,
	kScancodeKeypadAt = 206,
	kScancodeKeypadExclamation = 207,
	kScancodeKeypadMemStore = 208,
	kScancodeKeypadMemRecall = 209,
	kScancodeKeypadMemClear = 210,
	kScancodeKeypadMemAdd = 211,
	kScancodeKeypadMemSubtract = 212,
	kScancodeKeypadMemMultiply = 213,
	kScancodeKeypadMemDivide = 214,
	kScancodeKeypadPlusMinus = 215,
	kScancodeKeypadClear = 216,
	kScancodeKeypadClearEntry = 217,
	kScancodeKeypadBinary = 218,
	kScancodeKeypadOctal = 219,
	kScancodeKeypadDecimal = 220,
	kScancodeKeypadHexadecimal = 221,

	kScancodeLeftCtrl = 224,
	kScancodeLeftShift = 225,
	kScancodeLeftAlt = 226,  /**< alt, option */
	kScancodeLeftMeta = 227, /**< windows, command (apple), meta */
	kScancodeRightCtrl = 228,
	kScancodeRightShift = 229,
	kScancodeRightAlt = 230,  /**< alt gr, option */
	kScancodeRightMeta = 231, /**< windows, command (apple), meta */

	kScancodeMode = 257, /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

	/* @} */ /* Usage page 0x07 */

	/**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
	/* @{ */

	kScancodeAudioNext = 258,
	kScancodeAudioPrevious = 259,
	kScancodeAudioStop = 260,
	kScancodeAudioPlay = 261,
	kScancodeAudioMute = 262,
	kScancodeMediaSelect = 263,
	kScancodeWebBrowser = 264,
	kScancodeEMail = 265,
	kScancodeCalculator = 266,
	kScancodeComputer = 267,
	kScancodeACSearch = 268,
	kScancodeACHome = 269,
	kScancodeACBack = 270,
	kScancodeACForward = 271,
	kScancodeACStop = 272,
	kScancodeACRefresh = 273,
	kScancodeACBookmarks = 274,

	/* @} */ /* Usage page 0x0C */

	/**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
	/* @{ */

	kScancodeBrightnessDown = 275,
	kScancodeBrightnessUp = 276,
	kScancodeDisplaySwitch = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
	kScancodeKeyboardBacklightToggle = 278,
	kScancodeKeyboardBacklightDown = 279,
	kScancodeKeyboardBacklightUp = 280,
	kScancodeEject = 281,
	kScancodeSleep = 282,

	kScancodeApp1 = 283,
	kScancodeApp2 = 284,

	/* @} */ /* Walther keys */

	/**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
	/* @{ */

	kScancodeAudioRewind = 285,
	kScancodeAudioFastForward = 286,

	/* @} */ /* Usage page 0x0C (additional media keys) */

	/* Add any other keys here. */

	kScancodeMax = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} scancode_t;

#ifdef __cplusplus
}
#endif
