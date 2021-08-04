#ifndef H_AJA_TIMECODE
#define H_AJA_TIMECODE

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fps_s {
    u32 numerator;
    u32 denominator;
} fps_t;

/**

╔═════════════════════════════════╗
║ SMPTE 12m Timecode payload bits ║
╚═════════════════════════════════╝
╔ lo word ══════════════════════════════════════════════════════════════════════════════════╗
║ 0..3  ║ 4..7   ║ 8..9 ║  10  ║  11   ║ 12..15 ║ 16..19 ║ 20..23 ║ 24..26 ║ 27    ║ 28..31 ║
║ frame ║ user   ║frame ║drop  ║color  ║ user   ║ secs   ║ user   ║ secs   ║ bpmpc ║ user   ║
║ units ║ bits 1 ║tens  ║frame ║frame  ║ bits 2 ║ units  ║ bits 3 ║ tens   ║       ║ bits 4 ║
╚═══════════════════════════════════════════════════════════════════════════════════════════╝
╔ hi word ══════════════════════════════════════════════════════════════════════════════════╗
║ 32..35 ║ 36..39 ║ 40..42 ║  43  ║ 44..47 ║ 48..51 ║ 52..55 ║ 56..57 ║ 58..59   ║ 60..63   ║
║ mins   ║ user   ║ mins   ║ bgfb ║ user   ║ hours  ║ user   ║ hours  ║ reserved ║ user     ║
║ units  ║ bits 5 ║ tens   ║      ║ bits 6 ║ units  ║ bits 7 ║ tens   ║ bgfb     ║ bits 8   ║
╚═══════════════════════════════════════════════════════════════════════════════════════════╝
**/

typedef enum {
    kFrameUnitsMask     = 0xf0000000,
    kUserBits1Mask      = 0xf000000,
    kFrameTensMask      = 0xc00000,
    kDropFrameBitMask   = 0x200000,
    kColorFrameBitMask  = 0x100000,
    kUserBits2Mask      = 0xf0000,
    kSecsUnitsMask      = 0xf000,
    kUserBits3Mask      = 0xf00,
    kSecsTensMask       = 0xe0,
    kBPMPCMask          = 0x10,
    kUserBits4Mask      = 0xf
} smpte_12m_lo_word_mask;

typedef enum {
    kMinsUnitsMask      = 0xf0000000,
    kUserBits5Mask      = 0xf000000,
    kMinsTensMask       = 0xe00000,
    kBGFBMask           = 0x100000,
    kUserBits6Mask      = 0xf0000,
    kHoursUnitsMask     = 0xf000,
    kUserBits7Mask      = 0xf00,
    kHoursTensMask      = 0xc0,
    kReservedBGFBMask   = 0x30,
    kUserBits8Mask      = 0xf
} smpte_12m_hi_word_mask;

typedef enum {
    kFrameUnitsShift    = 28,
    kUserBits1Shift     = 24,
    kFrameTensShift     = 23,
    kDropFrameBitShift  = 20,
    kColorFrameBitShift = 16,
    kUserBits2Shift     = 12,
    kSecsUnitsShift     = 8,
    kUserBits3Shift     = 7,
    kSecsTensShift      = 6,
    kBPMPCShift         = 4,
    kUserBits4Shift     = 0
} smpte_12m_lo_word_shift;

typedef enum {
    kMinsUnitsShift     = 28,
    kUserBits5Shift     = 24,
    kMinsTensShift      = 22,
    kBGFBShift          = 20,
    kUserBits6Shift     = 16,
    kHoursUnitsShift    = 12,
    kUserBits7Shift     = 8,
    kHoursTensShift     = 7,
    kReservedBGFBShift  = 4,
    kUserBits8Shift     = 0
} smpte_12m_hi_word_shift;

typedef struct smpte_tc {
    u32 hi_word;
    u32 lo_word;
    u16 sync_word;
} smpte_tc_t;

extern void smpte_set_lo_word_value(smpte_tc_t* st, u8 val, u32 mask, u32 shift);
extern u8 smpte_get_lo_word_value(smpte_tc_t* st, u32 mask, u32 shift);

extern void smpte_set_hi_word_value(smpte_tc_t* st, u8 val, u32 mask, u32 shift);
extern u8 smpte_get_hi_word_value(smpte_tc_t* st, u32 mask, u32 shift);

extern void smpte_set_sync_word(smpte_tc_t* st, u16 val);
extern u16 smpte_get_sync_word(smpte_tc_t* st);

// hi word setters
extern void smpte_set_mins_units(smpte_tc_t* st, u8 val);
extern void smpte_set_user_bits_5(smpte_tc_t* st, u8 val);
extern void smpte_set_mins_tens(smpte_tc_t* st, u8 val);
extern void smpte_set_bgfb(smpte_tc_t* st, u8 val);
extern void smpte_set_user_bits_6(smpte_tc_t* st, u8 val);
extern void smpte_set_hours_units(smpte_tc_t* st, u8 val);
extern void smpte_set_user_bits_7(smpte_tc_t* st, u8 val);
extern void smpte_set_hours_tens(smpte_tc_t* st, u8 val);
extern void smpte_set_reserved_bgfb(smpte_tc_t* st, u8 val);
extern void smpte_set_user_bits_8(smpte_tc_t* st, u8 val);

// lo word setters
extern void smpte_set_frame_units(smpte_tc_t* st, u8 val);
extern u8 smpte_get_frame_units(smpte_tc_t* st);

extern void smpte_set_user_bits_1(smpte_tc_t* st, u8 val);
extern u8 smpte_get_user_bits_1(smpte_tc_t* st);

extern void smpte_set_frame_tens(smpte_tc_t* st, u8 val);
extern u8 smpte_get_frame_tens(smpte_tc_t* st);

extern void smpte_set_drop_frame_bit(smpte_tc_t* st, u8 val);
extern u8 smpte_get_drop_frame_bit(smpte_tc_t* st);

extern void smpte_set_color_frame_bit(smpte_tc_t* st, u8 val);
extern u8 smpte_get_color_frame_bit(smpte_tc_t* st);

extern void smpte_set_user_bits_2(smpte_tc_t* st, u8 val);
extern u8 smpte_get_user_bits_2(smpte_tc_t* st);

extern void smpte_set_secs_units(smpte_tc_t* st, u8 val);
extern u8 smpte_get_secs_units(smpte_tc_t* st);

extern void smpte_set_user_bits_3(smpte_tc_t* st, u8 val);
extern u8 smpte_get_user_bits_3(smpte_tc_t* st);

extern void smpte_set_secs_tens(smpte_tc_t* st, u8 val);
extern u8 smpte_get_secs_tens(smpte_tc_t* st);

extern void smpte_set_bpmpc(smpte_tc_t* st, u8 val);
extern u8 smpte_get_bpmpc(smpte_tc_t* st);

extern void smpte_set_user_bits_4(smpte_tc_t* st, u8 val);
extern u8 smpte_get_user_bits_4(smpte_tc_t* st);

// SMPTE 12M Timecode (little-endian)
typedef struct smpte_s {
    u32 frame_units:4;
    u32 user1:4;

    u32 frame_tens:2;
    u32 drop_frame:1;
    u32 color_frame:1;
    u32 user2:4;

    u32 secs_units:4;
    u32 user3:4;

    u32 secs_tens:3;
    u32 biphase_mark_phase_correction:1;
    u32 user4:4;

    u32 mins_units:4;
    u32 user5:4;

    u32 mins_tens:3;
    u32 binary_group_flag_bit0:1;
    u32 user6:4;

    u32 hours_units:4;
    u32 user7:4;

    u32 hours_tens:2;
    u32 binary_group_flag_bit1:1;
    u32 binary_group_flag_bit2:1;
    u32 user8:4;

    u16 sync_word:16;
} smpte_t;


// human-readable timecode
typedef struct timecode {
    // char timezone[6];
    // u8 years;
    // u8 months;
    // u8 days;

    u8 hours; // hour 0..23
    u8 mins;  // minute 0..60
    u8 secs;  // second 0..60
    u8 frame; // sub-second frame 0..(FPS - 1)
    fps_t fps;
    bool drop;
} timecode_t;

extern void frames_to_timecode(u32 frames, fps_t fps, bool drop, timecode_t* tc);
extern u32 timecode_to_frames(timecode_t* tc);

extern void smpte_init_frame(smpte_t* st);
extern void smpte_drop_frames(smpte_t* st);
extern void smpte_increment(smpte_t* st, fps_t fps);
extern void smpte_to_timecode(smpte_t* st, timecode_t* tc);
extern void timecode_to_smpte(timecode_t* tc, smpte_t* st);

#ifdef __cplusplus
}
#endif

#endif
