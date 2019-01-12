#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
    InstallerBuilder entry point.
    Input: list of files and configuration.
    Output: single installer file (.exe).

    Command-line arguments:
        -l <path_to_file>  --  specify path to files list file [optional]
        -u [Y/N]        --  include uninstaller? [optional, yes by default]
*/
int main(int argc, char** argv)
{
    char* file_list = "install_files.txt";
    bool include_uninstaller = true;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            file_list = argv[i + 1];
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

    printf("TODO: read file list from %s.\n", file_list);
    printf("TODO: generate installer file.\n");
    if (include_uninstaller)
        printf("TODO: include installer.\n");

    return 0;
}
