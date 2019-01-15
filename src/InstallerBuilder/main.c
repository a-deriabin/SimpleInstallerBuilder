#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../Shared/StringUtil.h"

/*
    InstallerBuilder entry point.
    Input: list of files and configuration.
    Output: single installer file (.exe).

    Command-line arguments:
        -l <path_to_file>  --  specify path to files list file [optional]
        -u [Y/N]        --  include uninstaller? [optional, yes by default]

    List of files should be a text file, where each line
    represents a single file or directory.
    If it is a file, it will be copied to the root of destination directory.
    If it is a directory, the files will preserve their relative path.
    E.g. we write: '/mydir' to install_files.txt.
    mydir is a directory with subdirectory 'test' and file 'a.txt'.
    Resulting installer output will be: USER_SPECIFIED_DIR/test/a.txt
*/
int main(int argc, char** argv)
{
    char* list_file_name = "install_files.txt";
    bool include_uninstaller = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            list_file_name = argv[i + 1];
            i += 1;
            continue;
        }
        
        if (strcmp(argv[i], "-u") == 0) {
            if (*argv[i+1] == 'Y' || *argv[i+1] == 'y')
                include_uninstaller = true;
            else if (*argv[i+1] == 'N' || *argv[i+1] == 'n')
                include_uninstaller = false;
            else
                printf("Unknown option %s. Expected Y or N.\n", argv[i+1]);
            i += 1;
            continue;
        }
        
        printf("Unknown argument: %s\n", argv[i]);
    }

    FILE *list_file = fopen(list_file_name, "r");
    if (list_file == NULL) {
        printf("Failed to open file: %s", list_file_name);
        return 0;
    }

    char* line = (char*) malloc(sizeof(char) * 255);
    while (fgets(line, 255, list_file) != NULL) {
        char* trimmed = string_trim(line);
        printf("Retrieved line of text: %s\n", trimmed);
    }

    fclose(list_file);

    printf("TODO: generate installer file.\n");
    if (include_uninstaller)
        printf("TODO: include installer.\n");

    return 0;
}
