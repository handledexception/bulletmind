#include "core/path.h"

#include <string.h>

const char* path_get_basename(const char* path)
{
	const char* slash = strrchr(path, PATH_SEP);
	if (!slash || slash == path)
		return "";
	return slash + 1;
}

const char* path_get_extension(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}
