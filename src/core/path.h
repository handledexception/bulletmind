#ifndef BM_CORE_PATH
#define BM_CORE_PATH

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BM_WINDOWS)
#define PATH_SEP '\\'
#define WPATH_SEP L'\\'
#else
#define PATH_SEP '/'
#define WPATH_SEP L'/'
#endif

const char* path_get_basename(const char* path);
const char* path_get_extension(const char* filename);

#ifdef __cplusplus
}
#endif

#endif
