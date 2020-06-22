#ifndef _H_CONFIG
#define _H_CONFIG

#include "c99defs.h"

#include <toml.h>

bool read_toml_config(const char* path, toml_table_t** toml);
bool read_table_string(toml_table_t* table, const char* key, char** string);

#endif
