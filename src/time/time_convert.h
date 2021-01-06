#pragma once

#include "types.h"

static inline f64 nsec_to_sec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000000000ULL);
}

static inline f64 nsec_to_msec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000000);
}

static inline f64 nsec_to_usec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000);
}

static inline u32 nsec_to_msec_u32(const u64 nanos)
{
	return (u32)(nanos / 1000000);
}
