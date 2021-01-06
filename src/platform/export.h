#pragma once

#define UNUSED_PARAMETER(param) (void)param

#ifdef _MSC_VER
#define BM_PLATFORM_EXPORT __declspec(dllexport)
#define BM_FORCE_INLINE __forceinline
#else
#define BM_PLATFORM_EXPORT
#define BM_FORCE_INLINE inline __attribute__((always_inline))
#endif
