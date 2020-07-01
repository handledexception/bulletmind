#ifndef _H_CONFIG
#define _H_CONFIG

#include "c99defs.h"

#include <toml.h>

bool read_toml_config(const char* path, toml_table_t** toml);
bool read_table_string(toml_table_t* table, const char* key, char** string);
bool read_table_int32(toml_table_t* table, const char* key, int32_t* i32);
bool read_table_double(toml_table_t* table, const char* key, double* dbl);

#endif
