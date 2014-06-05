
#ifndef DEF_CONFIG
#define DEF_CONFIG

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int load_config();
void free_config();
int has_entry(const char* name);
const char* get_string(const char* name);
int32_t get_int(const char* name);
int get_bool(const char* name);
/* For debug */
void dump_to_stdout();

#endif

