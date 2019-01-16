#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include <stdint.h>

#define INITIAL_CAPACITY 16

typedef struct {
    int32_t capacity;
    int32_t size;
    int32_t entry_size;
    void* array;
} array_list;

array_list* list_new(const int32_t element_size);
void list_add(array_list* list, void* p);
void* list_get(array_list* list, int32_t i);
void list_set(array_list* list, int32_t i, void* value_p);
void* list_to_array(const array_list* list);


#endif