#ifndef HUFFMAN_COMPRESSION_H
#define HUFFMAN_COMPRESSION_H

#include <stdbool.h>
#include <stdint.h>
#include "BitUtil.h"

typedef struct huff_tree_node {
    char value;
    uint32_t count;
    bool is_internal;
    struct huff_tree_node* left;
    struct huff_tree_node* right;
    struct huff_tree_node* parent;
} huff_tree_node;

huff_tree_node** init_occurrence_array(const char* data, const size_t data_size);
huff_tree_node* build_tree(huff_tree_node** init_array);
bool write_occurrence_array(huff_tree_node** arr, FILE* out_file);
bool compress_and_write(const char* data, const size_t data_size,
     huff_tree_node** init_array, BIT_WRITE_STREAM* out_stream);
huff_tree_node** read_occurrence_array(FILE* in_file);
char* read_and_decompress(const size_t uncompressed_size, 
        huff_tree_node** init_array, BIT_READ_STREAM* in_stream);

#endif