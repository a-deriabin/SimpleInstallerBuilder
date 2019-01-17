#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "../Shared/StringUtil.h"
#include "../Shared/FileUtil.h"

#undef DEBUG

// this is the name of the temporary .bat file
static const char tempbatname[] = "_uninst_helper.bat" ;
static const char tempbatname2[] = "_uninst_helper_2.bat";

void selfdestruct() 
{
    // temporary .bat files' content
    static char templ[] = 
        "echo \"deletion started\"\r\n"
        ":Repeat\r\n"
        "del \"%s\"\r\n"
        "if exist \"%s\" goto Repeat\r\n"
        "%s" ;

    static char templ2[] = 
        "cd ..\r\n"
        ":Repeat2\r\n"
        "rmdir \"%s\"\r\n"
        "if exist \"%s\" goto Repeat2\r\n"
        "del \"%s\"\r\n"
        "del \"%s\"\r\n";


    char modulename[MAX_PATH + 1];    // absolute path of calling .exe file
    char temppath[MAX_PATH + 1];      // absolute path of temporary .bat file
    char temppath2[MAX_PATH + 1];     // same as temppath but for second .bat
    char folder[MAX_PATH + 1];

    // Get path for the first .bat
    GetTempPath(MAX_PATH, temppath);
    strcat(temppath, tempbatname);

    // Get path for the second .bat
    GetTempPath(MAX_PATH, temppath2);
    strcat(temppath2, tempbatname2);

    // Get current module directory
    GetModuleFileName(NULL, modulename, MAX_PATH + 1);
    strcpy (folder, modulename);

    // Get rid of the slash at the end of folder name
    char *pb = strrchr(folder, '\\');
    if (pb != NULL)
        *pb = 0;

    // Handle of files we're creating
    HANDLE hf;

    // Variables to be used during file creation
    DWORD len;
    char *bat;
  
    // Create second bat
    hf = CreateFile(temppath2, GENERIC_WRITE, 0, NULL,
              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hf == INVALID_HANDLE_VALUE) {
        #if DEBUG
        printf("DBG: could not create secondary .bat file\n");
        #endif
        return;
    }
    
    bat = (char*)malloc(strlen(templ2) + 
               strlen(folder) * 2 + strlen(temppath) +
               strlen(temppath2) + 20);

    wsprintf(bat, templ2, folder, folder, temppath, temppath2);

    WriteFile(hf, bat, strlen(bat), &len, NULL);
    CloseHandle(hf);

    // Create first bat
    hf = CreateFile(temppath, GENERIC_WRITE, 0, NULL, 
              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  
    if (hf == INVALID_HANDLE_VALUE) {
        #if DEBUG
        printf("could not create primary .bat file\n");
        #endif
        return;
    }

    bat = (char*)malloc(strlen(templ) + 
               strlen(modulename) * 2 + strlen(temppath2) + 20);

    wsprintf(bat, templ, modulename, modulename, temppath2);

    WriteFile(hf, bat, strlen(bat), &len, NULL);
    CloseHandle(hf);

    // Execute primary bat file
    ShellExecute(NULL, "open", temppath, NULL, NULL, SW_HIDE);

    #if DEBUG
    printf("DBG: Executed shell command.\n");
    #endif
}

void perform_delete() {
    // Get current executable name
    TCHAR cur_file[MAX_PATH + 1];
    GetModuleFileName(NULL, cur_file, MAX_PATH + 1);

    // Extract directory name
    char* cur_dir_name = dirname(cur_file);

    // Remove everything inside directory (including uninstaller)
    delete_recursively(cur_dir_name);
    selfdestruct();
}

/*
    Uninstaller entry point.
    Deletes all the files in the current directory.
    After that, deletes itself and deletes current folder.
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    
    const int result = MessageBox(
        NULL,
        "You are going to uninstall application. Are you sure?",
        "Confirm uninstall",
        MB_YESNO);

    switch (result)
    {
        case IDYES:
            perform_delete();
            break;
        case IDNO:
            break;
    }

    return 0;
}
