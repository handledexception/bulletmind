#ifndef H_BM_CORE_EXPORT
#define H_BM_CORE_EXPORT

#ifdef _MSC_VER
#define BM_DLL_EXPORT __declspec(dllexport)
#define BM_FORCE_INLINE __forceinline
#else
#define BM_DLL_EXPORT
#define BM_FORCE_INLINE inline __attribute__((always_inline))
#endif

#endif
