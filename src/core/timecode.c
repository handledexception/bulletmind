#include "timecode.h"

#include <math.h>
#include <string.h>

void smpte_set_lo_word_value(smpte_tc_t* st, u8 val, u32 mask, u32 shift) {
    st->lo_word &= ~mask;
    st->lo_word |= ((val << shift) & mask);
}

u8 smpte_get_lo_word_value(smpte_tc_t* st, u32 mask, u32 shift) {
    return (st->lo_word & mask) >> shift;
}

void smpte_set_hi_word_value(smpte_tc_t* st, u8 val, u32 mask, u32 shift) {
    st->hi_word &= ~mask;
    st->hi_word |= ((val << shift) & mask);
}

u8 smpte_get_hi_word_value(smpte_tc_t* st, u32 mask, u32 shift) {
    return (st->hi_word & mask) >> shift;
}

void smpte_set_frame_units(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kFrameUnitsMask, kFrameUnitsShift);
}
u8 smpte_get_frame_units(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kFrameUnitsMask, kFrameUnitsShift);
}

void smpte_set_user_bits_1(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kUserBits1Mask, kUserBits1Shift);
}
u8 smpte_get_user_bits_1(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kUserBits1Mask, kUserBits1Shift);
}

void smpte_set_frame_tens(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kFrameTensMask, kFrameTensShift);
}
u8 smpte_get_frame_tens(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kFrameTensMask, kFrameTensShift);
}

void smpte_set_drop_frame_bit(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kDropFrameBitMask, kDropFrameBitShift);
}
u8 smpte_get_drop_frame_bit(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kDropFrameBitMask, kDropFrameBitShift);
}

void smpte_set_color_frame_bit(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kColorFrameBitMask, kColorFrameBitShift);
}
u8 smpte_get_color_frame_bit(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kColorFrameBitMask, kColorFrameBitShift);
}

void smpte_set_user_bits_2(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kUserBits2Mask, kUserBits2Shift);
}
u8 smpte_get_user_bits_2(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kUserBits2Mask, kUserBits2Shift);
}

void smpte_set_secs_units(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kSecsUnitsMask, kSecsUnitsShift);
}
u8 smpte_get_secs_units(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kSecsUnitsMask, kSecsUnitsShift);
}

void smpte_set_user_bits_3(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kUserBits3Mask, kUserBits3Shift);
}
u8 smpte_get_user_bits_3(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kUserBits3Mask, kUserBits3Shift);
}

void smpte_set_secs_tens(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kSecsTensMask, kSecsTensShift);
}
u8 smpte_get_secs_tens(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kSecsTensMask, kSecsTensShift);
}

void smpte_set_bpmpc(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kBPMPCMask, kBPMPCShift);
}
u8 smpte_get_bpmpc(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kBPMPCMask, kBPMPCShift);
}

void smpte_set_user_bits_4(smpte_tc_t* st, u8 val) {
    smpte_set_lo_word_value(st, val, kUserBits4Mask, kUserBits4Shift);
}
u8 smpte_get_user_bits_4(smpte_tc_t* st) {
    return smpte_get_lo_word_value(st, kUserBits4Mask, kUserBits4Shift);
}

bool is_high_framerate(fps_t fps) {
    if (fps.numerator % 47980 == 0 ||
        fps.numerator % 48000 == 0 ||
        fps.numerator % 50000 == 0 ||
        fps.numerator % 60000 == 0)
        return true;
    return false;
}

u32 round_fps_rational(double fps) {
    return (u32)(fps + 0.5);
}

void frames_to_timecode(u32 frames, fps_t fps, bool drop, timecode_t* tc) {
    bool is_hfr = is_high_framerate(fps);

    s64 frame = (s64)frames;

    f64 fps_num = fps.numerator;

    if (is_hfr) {
        frame /= 2;
        fps_num /= 2;
    }

    f64 fps_rational = fps_num / (double)fps.denominator;

    u32 frames_per_sec = round_fps_rational(fps_rational);
    u32 frames_per_min = frames_per_sec * 60;
    u32 frames_per_hr = frames_per_min * 60;
    u32 frames_per_day = frames_per_hr * 24;
    if (drop) {
        u32 dropped_frames = round_fps_rational(fps_rational * 0.066666);
        u32 dropped_frames_per_sec = frames_per_sec - dropped_frames;
        u32 dropped_frames_per_min = (59 * frames_per_sec) + dropped_frames_per_sec;
        u32 dropped_frames_per_ten_min = (9 * dropped_frames_per_min) + frames_per_min;
        u32 dropped_frames_per_hr = dropped_frames_per_ten_min * 6;
        u32 dropped_frames_per_day = dropped_frames_per_hr * 24;

        frame = frame % dropped_frames_per_day;

        tc->hours = (u32)(frame / dropped_frames_per_hr);
        frame = frame % dropped_frames_per_hr;

        tc->mins = (u32)(10 * (frame / dropped_frames_per_ten_min));
        frame = frame % dropped_frames_per_ten_min;

        if (frame >= frames_per_min) {
            tc->mins += 1;
            frame = frame - frames_per_min;

            tc->mins += (u32)(frame / dropped_frames_per_min);
            frame = frame % dropped_frames_per_min;
        }

        tc->secs = 0;
        if (tc->mins % 10 == 0) {
            tc->secs = (u32)(frame / frames_per_sec);
            frame = frame % frames_per_sec;
        }
        else {
            if (frame >= dropped_frames_per_sec) {
                tc->secs += 1;
                frame = frame - dropped_frames_per_sec;

                tc->secs += (u32)(frame / frames_per_sec);
                frame = frame % frames_per_sec;
            }
        }

        tc->frame = (u32)(frame);

        if ((tc->secs == 0) && (tc->mins % 10 != 0))
            tc->frame += dropped_frames;
    }
    else {
        frame = frame % frames_per_day;

        tc->hours = (u32)(frame / frames_per_hr);
        frame = frame % frames_per_hr;

        tc->mins = (u32)(frame / frames_per_min);
        frame = frame % frames_per_min;

        tc->secs = (u32)(frame / frames_per_sec);

        tc->frame = (u32)(frame % frames_per_sec);
    }
}

u32 timecode_to_frames(timecode_t* tc) {
    s64 frame_rate = 0;
    s64 frame_duration = 0;

    u32 frame = 0;

    bool is_hfr = is_high_framerate(tc->fps);
    if (is_hfr)
        tc->fps.numerator /= 2;

    f64 fps_rational = (double)tc->fps.numerator / (double)tc->fps.denominator;
    u32 tb = round_fps_rational(fps_rational);
    u32 minute_frames = tb * 60;
    u32 hour_frames = minute_frames * 60;
    if (tc->drop) {
        u32 drop_frames = round_fps_rational(fps_rational * 0.066666);

        u32 total_minutes = (60 * tc->hours) + tc->mins;

        u32 frm = tc->frame;
        if ((tc->secs == 0) && ((tc->mins % 10) > 0) && ((tc->frame & ~1) == 0))
            frm = 2;
        frame = ((hour_frames * tc->hours) + (minute_frames * tc->mins) + (tb * tc->secs) + frm) - (drop_frames * (total_minutes - (total_minutes / 10)));
    }
    else {
        frame = ((hour_frames * tc->hours) + (minute_frames * tc->mins) + (tb + tc->secs) + tc->frame);
    }

    if (is_hfr)
        frame *= 2;

    return frame;
}

void smpte_init_frame(smpte_t* st) {
    memset(&st, 0, sizeof(smpte_t));
}

void smpte_drop_frames(smpte_t* st) {
	if ((st->mins_units != 0)
		&& (st->secs_units == 0)
		&& (st->secs_tens == 0)
		&& (st->frame_units == 0)
		&& (st->frame_tens == 0)
		) {
		st->frame_units += 2;
	}
}

void smpte_increment(smpte_t* st, fps_t fps) {
    st->frame_units++;

    if (st->frame_units == 10) {
        st->frame_units = 0;
        st->frame_tens++;
    }

    if (fps.numerator == st->frame_units + st->frame_tens * 10) {
        st->frame_units = 0;
        st->frame_tens = 0;
        st->secs_units++;
        if (st->secs_units == 10) {
            st->secs_units = 0;
            st->secs_tens++;
            if (st->secs_tens == 6) {
                st->secs_tens = 0;
                st->mins_units++;
                if (st->mins_units == 10) {
                    st->mins_units = 0;
                    st->mins_tens++;
                    if (st->mins_tens == 6) {
                        st->mins_tens = 0;
                        st->hours_units++;
                        if (st->hours_units == 10) {
                            st->hours_units = 0;
                            st->hours_tens++;
                        }
                        if (st->hours_units == 4 && st->hours_tens == 2) {
                            st->hours_tens = 0;
                            st->hours_units = 0;
                        }
                    }
                }
            }
        }
    }

    if (st->drop_frame)
        smpte_drop_frames(st);
}

void timecode_to_smpte(timecode_t* tc, smpte_t* st) {
	st->hours_tens  = tc->hours / 10;
	st->hours_units = tc->hours - st->hours_tens * 10;
	st->mins_tens   = tc->mins / 10;
	st->mins_units  = tc->mins - st->mins_tens * 10;
	st->secs_tens   = tc->secs / 10;
	st->secs_units  = tc->secs - st->secs_tens * 10;
	st->frame_tens  = tc->frame / 10;
	st->frame_units = tc->frame - st->frame_tens * 10;

    if (st->drop_frame)
        smpte_drop_frames(st);
}

void smpte_to_timecode(smpte_t* st, timecode_t* tc) {
    tc->hours = st->hours_units + st->hours_tens * 10;
    tc->mins = st->mins_units + st->mins_tens * 10;
    tc->secs = st->secs_units + st->secs_tens * 10;
    tc->frame = st->frame_units + st->frame_tens * 10;
}
