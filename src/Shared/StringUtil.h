#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <stdbool.h>

char* string_copy(char* str_to_copy);
char* string_concat(char* str_a, char* str_b);
bool string_is_empty(char* str);
char* string_trim(char* str);

#endif