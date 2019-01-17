#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct huff_tree_node {
    char value;
    uint32_t count;
    struct huff_tree_node* left;
    struct huff_tree_node* right;
} huff_tree_node;

int huff_node_comp(const void* el_left, const void* el_right) {
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
        node->left = NULL;
        node->right = NULL;
        arr[i] = node;
    }

    for (size_t i = 0; i < data_size; i++) {
        char c = data[i];
        arr[c + 128]->count += 1;
    }

    return arr;
}

huff_tree_node* build_tree(const char* data, const size_t data_size) {
    huff_tree_node** arr = init_occurrence_array(data, data_size);
    size_t arr_size = 256;

    while (arr_size > 1) {
        // Sort array
        qsort(arr, arr_size, sizeof(huff_tree_node*), huff_node_comp);

        // Merge two smallest nodes
        huff_tree_node* node = (huff_tree_node*)malloc(sizeof(huff_tree_node));
        node->value = 0;
        node->count = arr[0]->count + arr[1]->count;
        node->left = arr[0];
        node->right = arr[1];
        arr[0] = node;

        // Put the last node to a free slot
        arr[1] = arr[arr_size - 1];

        // Shrink array
        arr_size -= 1;
    }

    //TODO: free array?

    return arr[0];
}



