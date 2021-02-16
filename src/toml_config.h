#pragma once

#include "c99defs.h"
#include "types.h"

#include <toml.h>

bool read_toml_config(const char* path, toml_table_t** toml);
bool read_table_string(toml_table_t* table, const char* key, char** val);
bool read_table_int32(toml_table_t* table, const char* key, i32* val);
bool read_table_f64(toml_table_t* table, const char* key, f64* val);
