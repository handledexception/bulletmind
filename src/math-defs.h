#pragma once

#include "c99defs.h"
#include "types.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

#ifndef M_2PI
#define M_2PI (2.f * M_PI)
#endif

#define RAD_TO_DEG(rad) (180.f / M_PI) * rad
#define DEG_TO_RAD(deg) (M_PI / 180.f) * deg

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#ifdef __cplusplus
}
#endif
