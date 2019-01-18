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
    if (arr == NULL) {
        #if DEBUG
        printf("DBG: failed to malloc for array.\n");
        #endif
        return NULL;
    }

    for (int c = -128; c <= 127; c++) {
        int i = c + 128;

        huff_tree_node* node = (huff_tree_node*) malloc(sizeof(huff_tree_node));
        if (node == NULL) {
            #if DEBUG
            printf("DBG: failed to malloc for array node.\n");
            #endif
            return NULL;
        }

        node->value = (char)c;
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

        if (w_result < 1) {
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

    return true;
}

huff_tree_node** read_occurrence_array(FILE* in_file) {
    const size_t ARR_SIZE = 256;
    fseek(in_file, -sizeof(uint32_t) * ARR_SIZE, SEEK_CUR);
    uint32_t read_buff = 0;
    int r_result;

    huff_tree_node** arr = (huff_tree_node**)malloc(sizeof(huff_tree_node*) * ARR_SIZE);
    if (arr == NULL) {
        #if DEBUG
        printf("DBG: error: failed to allocate memory for occurrence array!\n");
        #endif
        return NULL;
    }
    for (size_t i = 0; i < ARR_SIZE; i++) {
        r_result = fread(&read_buff, sizeof(uint32_t), 1, in_file);
        if (r_result < 1) {
            #if DEBUG
            printf("DBG: error: failed to read occurrence array.\n");
            #endif
            return NULL;
        }

        huff_tree_node* node = (huff_tree_node*)malloc(sizeof(huff_tree_node));
        if (node == NULL) {
            #if DEBUG
            printf("DBG: error: failed to allocate memory for array node!\n");
            #endif
            return NULL;
        }
        node->value = (char)(i - 128);
        node->count = read_buff;
        node->is_internal = false;
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;
        arr[i] = node;
    }

    fseek(in_file, -sizeof(uint32_t) * ARR_SIZE, SEEK_CUR);
    return arr;
}

char* read_and_decompress(const size_t uncompressed_size,
        huff_tree_node* root, BIT_READ_STREAM* in_stream) {

    char* buffer = (char*)malloc(uncompressed_size);
    if (buffer == NULL) {
        #if DEBUG
        printf("DBG: error: failed to allocate %u bytes\n", uncompressed_size);
        #endif
        return NULL;
    }

    printf("DBG: uncompressed_size: %u\n", uncompressed_size);

    huff_tree_node* cur_node = root;
    size_t i = 0;
    bool bit_buffer = 0;
    int r_result = 0;

    while (!in_stream->has_ended) {
        r_result = read_bit(in_stream, &bit_buffer);
        if (r_result != 0) {
            #if DEBUG
            printf("DBG: error: failed to read bit. Code: %d\n", r_result);
            #endif
            return NULL;
        }

        if (bit_buffer) {
            #if DEBUG
            if (cur_node->left == NULL) {
                printf("DBG: error: left node is null.\n");
                return NULL;
            }
            #endif
            cur_node = cur_node->left;
        }
        else {
            #if DEBUG
            if (cur_node->right == NULL) {
                printf("DBG: error: left node is null.\n");
                return NULL;
            }
            #endif
            cur_node = cur_node->right;
        }

        if (cur_node->left == NULL && cur_node->right == NULL) 
        {
            if (i >= uncompressed_size) {
                #if DEBUG
                printf("DBG: error: buffer overflow.\n");
                #endif
                return buffer;
            }

            if (cur_node == NULL) {
                printf("cur node is null!\n");
                return NULL;
            }

            buffer[i] = cur_node->value;
            i += 1;
            cur_node = root;
        }
    } //end while()

    printf("DBG: finish read_and_decompress\n");

    return buffer;
}
