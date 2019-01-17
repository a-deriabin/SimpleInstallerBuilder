#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "HuffmanCompression.h"
#include "BitUtil.h"

#define DEBUG true

static int huff_node_comp(const void* el_left, const void* el_right) {
    huff_tree_node* left = *((huff_tree_node**)el_left);
    huff_tree_node* right = *((huff_tree_node**)el_right);
    if (left->count > right->count)
        return 1;
    if (left->count < right->count)
        return -1;
    return 0;
}

huff_tree_node** init_occurrence_array(const char* data, const size_t data_size) {
    const size_t ARR_SIZE = 256;
    huff_tree_node** arr = (huff_tree_node**)malloc(sizeof(huff_tree_node*) * ARR_SIZE);

    for (char c = -128; c <= 127; c++) {
        int i = c + 128;
        huff_tree_node* node = (huff_tree_node*) malloc(sizeof(huff_tree_node));
        node->value = c;
        node->count = 0;
        node->is_internal = false;
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        arr[i] = node;
    }

    for (size_t i = 0; i < data_size; i++) {
        char c = data[i];
        arr[c + 128]->count += 1;
    }

    return arr;
}

huff_tree_node* build_tree(huff_tree_node** init_array) {
    size_t arr_size = 256;

    // Copy array
    huff_tree_node** arr = (huff_tree_node**) malloc(sizeof(huff_tree_node*) * arr_size);
    memcpy(arr, init_array, arr_size * sizeof(huff_tree_node*));
    
    while (arr_size > 1) {
        // Sort array
        qsort(arr, arr_size, sizeof(huff_tree_node*), huff_node_comp);

        // Merge two smallest nodes
        huff_tree_node* node = (huff_tree_node*)malloc(sizeof(huff_tree_node));
        node->value = 0;
        node->count = arr[0]->count + arr[1]->count;
        node->is_internal = true;
        node->left = arr[0];
        node->right = arr[1];
        node->parent = NULL;
        arr[0]->parent = node;
        arr[1]->parent = node;

        // Remove these two smallest nodes from array.
        // First one is replaced by freshly created.
        arr[0] = node;

        // Put the last node to a second free slot
        arr[1] = arr[arr_size - 1];

        // Shrink array
        arr_size -= 1;
    }

    //TODO: free copied array?

    return arr[0];
}

bool write_occurrence_array(huff_tree_node** arr, FILE* out_file) {
    const size_t ARR_SIZE = 256;
    int w_result = 0;

    for (size_t i = 0; i < ARR_SIZE; i++) {
        w_result = fwrite(&(arr[i]->count), sizeof(uint32_t), 1, out_file);

        if (w_result != 0) {
            #if DEBUG
            printf("Failed to write %d-th element of occurrence array.\n", i);
            #endif
            return false;
        }
    }
    return true;
}

bool compress_and_write(const char* data, const size_t data_size,
     huff_tree_node** init_array, BIT_WRITE_STREAM* out_stream) {

    int w_result;
    for (size_t i = 0; i < data_size; i++) {
        char c = data[i];
        huff_tree_node* node = init_array[c + 128];
        if (node == NULL) {
            #if DEBUG
            printf("DBG: error: node from occurrence array is null!\n");
            #endif
            return false;
        }

        while (node->parent != NULL) {
            if (node->parent->left == node)
                w_result = write_bit(out_stream, 1);
            else if (node->parent->right == node)
                w_result = write_bit(out_stream, 0);
            else {
                #if DEBUG
                printf("DBG: error: parent node doesn't reference to its child.\n");
                #endif
                return false;
            }

            if (w_result != 0) {
                #if DEBUG
                printf("DBG: Failed to write to a bit stream.\n");
                #endif

                return false;
            }

            node = node->parent;
        }
    } // end for()

    // // Align bit stream to 8 bits
    // uint8_t offset_buffer = 0;
    // flush_bit_write_stream(stream, &offset_buffer);

    // // Write bit stream offset
    // fwrite(&offset_buffer, sizeof(uint8_t), 1, out_file);
    return true;
}

