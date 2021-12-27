#include "core/path.h"

#include <string.h>

const char* path_get_extension(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}
