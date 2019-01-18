#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <string.h>

#include "FileToExeWriter.h"
#include "StringUtil.h"
#include "FileUtil.h"
#include "ArrayList.h"
#include "BitUtil.h"
#include "HuffmanCompression.h"

#define DEBUG false

static FILE_APPEND_RESULT append_file(FILE *dest, FILE *source,
         char* dest_filename) {
    
    // Read file to buffer
    size_t buffer_size = get_file_size(source);
    char* buffer = (char*) malloc(sizeof(char) * buffer_size);
    int read_result = fread(buffer, buffer_size, 1, source);
    if (read_result < 1)
        return FILE_APPEND_READ_ERROR;

    #if DEBUG
    printf("DBG: read file of size: %u\n", buffer_size);
    #endif

    char did_use_compression;
    int w_result;

    // Compress only large enough files
    if (buffer_size > 4096) {
        #if DEBUG
        printf("DBG: start compression\n");
        #endif

        did_use_compression = true;
        
        huff_tree_node** occur_array = init_occurrence_array(buffer, buffer_size);
        if (occur_array == NULL)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done init occurrence array.\n");
        #endif

        huff_tree_node* tree = build_tree(occur_array);
        if (tree == NULL)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done building tree.\n");
        #endif

        BIT_WRITE_STREAM* w_stream = open_bit_write_stream(dest);
        if (w_stream == NULL)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done opening bit write stream.\n");
        #endif

        w_result = compress_and_write(buffer, buffer_size, occur_array, w_stream);
        if (!w_result)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done compress_and_write.\n");
        #endif

        // Align bit stream to 8 bits
        uint8_t offset_buffer = 0;
        w_result = flush_bit_write_stream(w_stream, &offset_buffer);
        if (w_result != 0)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done bit flush.\n");
        #endif

        // Write offset
        w_result = fwrite(&offset_buffer, sizeof(uint8_t), 1, dest);
        if (w_result < 1)
            return FILE_APPEND_WRITE_ERROR;

        #if DEBUG
        printf("DBG: done writing offset: %u bits\n", offset_buffer);
        #endif

        // Write byte count
        uint32_t byte_count = w_stream->wrote_bytes;
        w_result = fwrite(&byte_count, sizeof(uint32_t), 1, dest);
        if (w_result < 1)
            return FILE_APPEND_WRITE_ERROR;

        #if DEBUG
        printf("DBG: wrote compressed byte count: %u\n", byte_count);
        #endif

        // Write uncompressed byte count
        w_result = fwrite(&buffer_size, sizeof(size_t), 1, dest);
        if (w_result < 1)
            return FILE_APPEND_WRITE_ERROR;

        close_bit_write_stream(w_stream);

        // Write occurrence array
        w_result = write_occurrence_array(occur_array, dest);
        if (!w_result)
            return FILE_APPEND_COMPRESS_ERROR;

        #if DEBUG
        printf("DBG: done compression\n");
        #endif
    }
    else {
        did_use_compression = false;

        // Write raw file bytes
        w_result = fwrite(buffer, buffer_size, 1, dest);
        if (w_result < 1)
            return FILE_APPEND_WRITE_ERROR;

        #if DEBUG
        printf("DBG: wrote file content.\n");
        #endif

        // Append byte count
        w_result = fwrite(&buffer_size, sizeof(size_t), 1, dest);
        if (w_result < 1)
            return FILE_APPEND_WRITE_ERROR;

        #if DEBUG
        printf("DBG: wrote byte count: %u\n", buffer_size);
        #endif
    }

    // Write compression marker byte
    w_result = fwrite(&did_use_compression, sizeof(char), 1, dest);
    if (w_result < 1)
        return FILE_APPEND_WRITE_ERROR;

    // Append file name
    uint16_t name_length = (uint16_t) strlen(dest_filename) + 1;
    w_result = fwrite(dest_filename, sizeof(char) * name_length, 1, dest);
    if (w_result < 1)
        return FILE_APPEND_WRITE_ERROR;

    #if DEBUG
    printf("DBG: wrote file name: %s\n", dest_filename);
    #endif

    // Append file name length
    w_result = fwrite(&name_length, sizeof(uint16_t), 1, dest);
    if (w_result < 1)
        return FILE_APPEND_WRITE_ERROR;

    #if DEBUG
    printf("DBG: wrote file length: %u\n", name_length);
    #endif

    return FILE_APPEND_SUCCESS;
}

static FILE_APPEND_RESULT read_and_append(FILE* dest_file, char* source_file,
         char* rel_path) {

    #if DEBUG
    printf("DBG: Opening file: %s\n", source_file);
    #endif

    FILE *in_file = fopen(source_file, "rb");
    if (in_file == NULL)
        return FILE_APPEND_OPEN_SOURCE_ERROR;

    #if DEBUG
    printf("DBG: done open file.\n");
    #endif

    // Extract file name from file path.
    // Copy string because basename() might modify the argument
    char* source_path_copy = string_copy(source_file);

    #if DEBUG
    printf("DBG: done copy source path.\n");
    #endif

    char* file_name = basename(source_path_copy);

    #if DEBUG
    printf("DBG: basename: %s\n", file_name);
    #endif

    // Determine resulting file path (where to extract it)
    char* rel_file_name = string_concat(rel_path, file_name);

    #if DEBUG
    printf("DBG: appending file with relative name: %s\n", rel_file_name);
    #endif

    // Append file
    FILE_APPEND_RESULT result = append_file(dest_file, in_file, rel_file_name);
    fclose(in_file);

    // Free memory
    free(source_path_copy);
    free(file_name);
    free(rel_file_name);

    if (result != FILE_APPEND_SUCCESS)
        return result;
    return FILE_APPEND_SUCCESS;
}

static FILE_APPEND_RESULT read_and_append_recursive(
        FILE* dest_file, char* source, char* prev_path,
        uint32_t* file_count) {

    #if DEBUG
    printf("DBG: cur relative dir: %s\n", prev_path);
    printf("DBG: cur file path: %s\n", source);
    #endif

    if (is_directory(source)) {
        #if DEBUG
        printf("DBG: found directory.\n");
        #endif
        char* source_path_copy = string_copy(source);
        char* cur_dir_name = basename(source_path_copy);
        char* new_prev_path = string_concat(
                prev_path,
                string_concat(cur_dir_name, "/"));

        array_list* inner_entries = directory_entries(source);
        for (int32_t i = 0; i < inner_entries->size; i++) {
            char* entry_path = *((char**) list_get(inner_entries, i));
            #if DEBUG
            printf("DBG: Found inner entry: %s\n", entry_path);
            #endif
            if (strcmp(entry_path, ".") == 0)
                continue;
            if (strcmp(entry_path, "..") == 0)
                continue;
            
            char* full_entry_path = path_combine(source, entry_path);
            read_and_append_recursive(dest_file, full_entry_path, new_prev_path, file_count);
            free(full_entry_path);
        }

        free(inner_entries);
        free(new_prev_path);
        free(cur_dir_name);
        free(source_path_copy);
        return FILE_APPEND_SUCCESS;
    }
    else {
        #if DEBUG
        printf("DBG: Found file.\n");
        #endif
        *file_count += 1;
        return read_and_append(dest_file, source, prev_path);
    }
}

/*
    Appends specified files to the end of .exe (or any other binary) file.
    dest_file: null-terminated string - name of the file to write to
    files_to_write: array of null-terminated strings - names of files to append
    write_file_count: size of files_to_write array
    Returns 1 if operation was successful, 0 otherwise.
*/
FILE_APPEND_RESULT write_files(const char* const dest_file,
                 char** files_to_write,
                const size_t write_file_count)
{
    // Open destination file
    FILE *out_file = fopen(dest_file, "a+b");
    if (out_file == NULL)
        return FILE_APPEND_OPEN_DEST_ERROR;

    // Open source files, read them and append to the dest file
    uint32_t file_count = 0;
    for (size_t i = 0; i < write_file_count; i++) {
        FILE_APPEND_RESULT result = read_and_append_recursive(out_file, files_to_write[i], "", &file_count);
        if (result != FILE_APPEND_SUCCESS) {
            fclose(out_file);
            return result;
        }
    }

    // Append file count
    int wrote_count = fwrite(&file_count, sizeof(uint32_t), 1, out_file);
    if (wrote_count < 1) {
        fclose(out_file);
        return FILE_APPEND_WRITE_ERROR;
    }

    #if DEBUG
    printf("DBG: wrote file count: %u\n", file_count);
    #endif

    // We're done
    fclose(out_file);
    return FILE_APPEND_SUCCESS;
}

