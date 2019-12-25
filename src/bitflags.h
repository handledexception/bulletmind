#ifndef H_BITFLAGS
#define H_BITFLAGS

#define SET_FLAG(flags, bit_idx) flags |= (1 << bit_idx)
#define CLEAR_FLAG(flags, bit_idx) flags &= ~(1 << bit_idx)
#define TOGGLE_FLAG(flags, bit_idx) flags ^= (1 << bit_idx)
#define IS_FLAG_SET(flags, bit_idx) (flags & (1 << bit_idx))

#endif