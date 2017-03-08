#ifndef H_BITWISE
#define H_BITWISE

#include "c99defs.h"

// http://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit-in-c-c

// set bit n
static void bit_set_uint16(uint16_t *dest, uint32_t bitnum)
{
	*dest |= 1 << bitnum;
}

// set bit n
static void bit_set_uint32(uint32_t *dest, uint32_t bitnum)
{
	*dest |= 1 << bitnum;	
}
//clear bit n
static void bit_clear_uint16(uint16_t *dest, uint32_t bitnum)
{
	*dest &= ~(1 << bitnum);
}
// clear bit n
static void bit_clear_uint32(uint32_t *dest, uint32_t bitnum)
{
	*dest &= ~(1 << bitnum);
}
// toggle bit n
static void bit_toggle_uint16(uint16_t *dest, uint32_t bitnum)
{
	*dest ^= 1 << bitnum;
}
// toggle bit n
static void bit_toggle_uint32(uint32_t *dest, uint32_t bitnum)
{
	*dest ^= 1 << bitnum;
}
// check value of bit n
static uint16_t bit_check_uint16(uint16_t src, uint32_t bitnum)
{
	return (src >> bitnum) & 1;
}
// check value of bit n
static uint32_t bit_check_uint32(uint32_t src, uint32_t bitnum)
{
	return (src >> bitnum) & 1;
}
#endif