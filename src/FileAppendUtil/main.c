#include <stdio.h>
#include <stdlib.h>
#include "../Shared/FileToExeWriter.h"

/*
    FileAppendUtil entry point
    Input: destination file, source files
    Result: source files are appended to the destination file
    and are able to be extracted later.
*/
int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("A destination file and at least one source file should be specified.\n");
        return 0;
    }

    char *dest = argv[1];
    char **source = (char**) malloc(sizeof(char*) * argc - 2);

    for (int i = 0; i < argc - 2; i++)
        source[i] = argv[i + 2];

    FILE_APPEND_RESULT result = write_files(dest, source, argc - 2);

    switch (result) {
        case FILE_APPEND_SUCCESS:
            printf("Finished successfully.\n");
            break;
        case FILE_APPEND_OPEN_DEST_ERROR:
            printf("Failed to open destination file.\n");
            break;
        case FILE_APPEND_OPEN_SOURCE_ERROR:
            printf("Failed to open one of the source files.\n");
            break;
        case FILE_APPEND_WRITE_ERROR:
            printf("Failed to write to a destination file.\n");
            break;
    }

    return 0;
}