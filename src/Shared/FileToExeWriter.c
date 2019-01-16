#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <string.h>

#include "FileToExeWriter.h"
#include "StringUtil.h"
#include "FileUtil.h"
#include "ArrayList.h"

typedef char buffer_t;

static FILE_APPEND_RESULT append_file(FILE *dest, FILE *source,
         char* dest_filename) {
    //TODO: using larger buffer might speed this up

    buffer_t buffer;
    uint32_t total_bytes = 0;
    int wrote_count;

    while (!feof(source)) {
        // Read to buffer
        int read_count = fread(&buffer, sizeof(buffer_t), 1, source);
        if (read_count < 1)
            break;
        
        //TODO: read to big buffer, compress, then append

        // Append what we've read to destination file
        wrote_count = fwrite(&buffer, sizeof(buffer_t), 1, dest);

        if (wrote_count < 1)
            return FILE_APPEND_WRITE_ERROR;

        // Count bytes
        total_bytes += wrote_count * sizeof(buffer_t);
    }

    // Append byte count
    wrote_count = fwrite(&total_bytes, sizeof(uint32_t), 1, dest);
    if (wrote_count < 1)
        return FILE_APPEND_WRITE_ERROR;

    printf("DBG: wrote byte count: %u\n", total_bytes);

    // Append file name
    uint16_t name_length = (uint16_t) strlen(dest_filename) + 1;
    wrote_count = fwrite(dest_filename, sizeof(char) * name_length, 1, dest);
    if (wrote_count < 1)
        return FILE_APPEND_WRITE_ERROR;

    printf("DBG: wrote file name: %s\n", dest_filename);

    // Append file name length
    wrote_count = fwrite(&name_length, sizeof(uint16_t), 1, dest);
    if (wrote_count < 1)
        return FILE_APPEND_WRITE_ERROR;

    printf("DBG: wrote file length: %u\n", name_length);

    return FILE_APPEND_SUCCESS;
}

static FILE_APPEND_RESULT read_and_append(FILE* dest_file, char* source_file,
         char* rel_path) {
    FILE *in_file = fopen(source_file, "rb");
    if (in_file == NULL)
        return FILE_APPEND_OPEN_SOURCE_ERROR;

    // Extract file name from file path.
    // Copy string because basename() might modify the argument
    char* source_path_copy = string_copy(source_file);
    char* file_name = basename(source_path_copy);

    // Determine resulting file path (where to extract it)
    char* rel_file_name = string_concat(rel_path, file_name);

    // Append file
    FILE_APPEND_RESULT result = append_file(dest_file, in_file, rel_file_name);
    fclose(in_file);

    if (result != FILE_APPEND_SUCCESS)
        return result;
    return FILE_APPEND_SUCCESS;
}

static FILE_APPEND_RESULT read_and_append_recursive(
        FILE* dest_file, char* source, char* prev_path) {

    if (is_directory(source)) {
        char* source_path_copy = string_copy(source);
        char* cur_dir_name = basename(source_path_copy);
        char* new_prev_path = string_concat(
                prev_path,
                string_concat(cur_dir_name, "/"));

        array_list* inner_entries = directory_entries(source);
        for (int32_t i = 0; i < inner_entries->size; i++) {
            char* entry_path = (char*) list_get(inner_entries, i);
            printf("Found: %s\n", entry_path);
            read_and_append_recursive(dest_file, entry_path, new_prev_path);
        }

        return FILE_APPEND_SUCCESS;
    }
    else {
        printf("Found simple file.\n");
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
    for (size_t i = 0; i < write_file_count; i++) {
        FILE_APPEND_RESULT result = read_and_append_recursive(out_file, files_to_write[i], "");
        if (result != FILE_APPEND_SUCCESS) {
            fclose(out_file);
            return result;
        }
    }

    // Append file count
    uint32_t file_count = (uint32_t)write_file_count;
    int wrote_count = fwrite(&file_count, sizeof(uint32_t), 1, out_file);
    if (wrote_count < 1) {
        fclose(out_file);
        return FILE_APPEND_WRITE_ERROR;
    }

    printf("DBG: wrote file count: %u\n", file_count);

    // We're done
    fclose(out_file);
    return FILE_APPEND_SUCCESS;
}

