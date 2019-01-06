#include <stdio.h>
#include <stdlib.h>
#include "FileToExeWriter.h"

static FILE_APPEND_RESULT append_file(FILE *dest, FILE *source) {
    //TODO: using larger buffer might optimize this

    char buffer;

    while (!feof(source)) {
        int read_count = fread(&buffer, sizeof(char), 1, source);
        if (read_count < 1)
            break;
        
        int wrote_count = fwrite(&buffer, sizeof(char), 1, dest);

        if (wrote_count != 1)
            return FILE_APPEND_WRITE_ERROR;
    }

    return FILE_APPEND_SUCCESS;
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
    FILE *out_file = fopen(dest_file, "a+b");
    if (out_file == NULL)
        return FILE_APPEND_OPEN_DEST_ERROR;

    for (size_t i = 0; i < write_file_count; i++) {
        FILE *in_file = fopen(files_to_write[i], "rb");
        if (in_file == NULL) {
            fclose(out_file);
            return FILE_APPEND_OPEN_SOURCE_ERROR;
        }

        FILE_APPEND_RESULT result = append_file(out_file, in_file);
        fclose(in_file);

        if (result != FILE_APPEND_SUCCESS)
            return result;
    }

    fclose(out_file);
    return FILE_APPEND_SUCCESS;
}

