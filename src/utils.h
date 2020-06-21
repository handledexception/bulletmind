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

/*
 * Example:
 * switch(enum_val) {
 *     ENUM_CASE_RETURN_STR(enum_val);
 * }
 */
#define ENUM_CASE_RETURN_STR(enum_name) case(enum_name): return #enum_name;

#endif