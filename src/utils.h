#ifndef H_UTILS
#define H_UTILS

#include "c99defs.h"

#define PI 3.141592654f
#define PI_2 (2.f * PI)
#define RAD_TO_DEG(rad) (180.f/PI) * rad
#define DEG_TO_RAD(deg) (PI/180.f) * deg

inline const char* file_extension(const char* filename)
{
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";
    return dot + 1;
}

#endif