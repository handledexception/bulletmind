#ifndef _H_BITFLAGS
#define _H_BITFLAGS

#define SET_FLAG(flags, bit_idx) flags |= (1 << bit_idx)
#define CLEAR_FLAG(flags, bit_idx) flags &= ~(1 << bit_idx)
#define TOGGLE_FLAG(flags, bit_idx) flags ^= (1 << bit_idx)
#define IS_FLAG_SET(flags, bit_idx) (flags & (1 << bit_idx))

inline const char* byte_to_bin(u8 x)
{
	static char b[9];
	b[0] = '\0';

	u8 z;
	char* p = b;
	for (z = 128; z > 0; z >>= 1) {
		u8 result = (x & z);
		printf("(%d & %d) = %d\n", x, z, result);
		*p++ = result ? '1' : '0';
		/* if ((x & z) > 0) {
            *p++ = '1';
        } else {
            *p++ = '0';
        } */
	}

	return b;
}

#endif