#pragma once

#include "core/export.h"

#ifdef __cplusplus
extern "C" {
#endif

BM_EXPORT void gfx_activate_d3d12_debug(void);
BM_EXPORT void gfx_com_release_d3d12(void);

#ifdef __cplusplus
}
#endif
