#ifndef _H_UTILS
#define _H_UTILS

#include "c99defs.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define PI 3.141592654f
#define PI_2 (2.f * PI)
#define RAD_TO_DEG(rad) (180.f/PI) * rad
#define DEG_TO_RAD(deg) (PI/180.f) * deg

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

inline const char* file_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

inline uint32_t pack_version(uint8_t maj, uint8_t min, uint8_t rev) {
    return (uint32_t)((maj << 16) + (min << 8) + rev);
}

inline void version_string(const uint32_t version, const char* ver_str) {
    char str_tmp[12];
    const uint8_t ver_maj = (version & 0xff0000) >> 16;
    const uint8_t ver_min = (version & 0xff00) >> 8;
    const uint8_t ver_rev = (version & 0xff);
    sprintf(str_tmp, "%d.%d.%d", ver_maj, ver_min, ver_rev);
    memcpy(ver_str, str_tmp, 12);
}
/*
 * Example:
 * switch(enum_val) {
 *     ENUM_CASE_RETURN_STR(enum_val);
 * }
 */
#define ENUM_CASE_RETURN_STR(enum_name) case(enum_name): return #enum_name;

#endif