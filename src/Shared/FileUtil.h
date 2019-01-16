#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include "ArrayList.h"

int is_directory(const char* path);
array_list* directory_entries(const char* dir_path);
char* path_combine(const char* dir, const char* file);

#endif