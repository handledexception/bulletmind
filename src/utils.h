#pragma once

#include "c99defs.h"
#include "types.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

inline const char* file_extension(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}

inline u32 pack_version(u8 maj, u8 min, u8 rev)
{
	return (u32)((maj << 16) + (min << 8) + rev);
}

inline void version_string(const u32 version, char* ver_str)
{
	char str_tmp[12];
	const u8 ver_maj = (version & 0xff0000) >> 16;
	const u8 ver_min = (version & 0xff00) >> 8;
	const u8 ver_rev = (version & 0xff);
	sprintf(str_tmp, "%d.%d.%d", ver_maj, ver_min, ver_rev);
	memcpy(ver_str, str_tmp, 12);
}
/*
 * Example:
 * switch(enum_val) {
 *     ENUM_CASE_RETURN_STR(enum_val);
 * }
 */
#define ENUM_CASE_RETURN_STR(enum_name) \
	case (enum_name):               \
		return #enum_name;
