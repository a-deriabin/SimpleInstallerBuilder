#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shlobj.h>
#include "../Shared/FileExtracter.h"
#include "../Shared/StringUtil.h"


static char* get_result_msg(FILE_EXTRACT_RESULT result) {
    switch (result) {
        case EXTRACT_SUCCESS:
            return "Installed successfully.\n";

        case EXTRACT_OPEN_SOURCE_ERROR:
            return "Error: failed to open source file.\n";

        case EXTRACT_OPEN_DEST_ERROR:
            return "Error: failed to open destination file.\n";

        case EXTRACT_READ_ERROR:
            return "Error: failed to read from file.\n";

        case EXTRACT_WRITE_ERROR:
            return "Error: failed to write to file.\n";

        default:
            return "Unknown result.\n";
    }
}

HWND hEdit;

static void browse_btn_clicked(HWND hwnd) {
    LPITEMIDLIST pidlBrowse;
    BROWSEINFO BRinfo;
    BRinfo.hwndOwner = hwnd;
    BRinfo.pidlRoot = NULL;
    BRinfo.pszDisplayName = NULL;
    BRinfo.lpszTitle = "Select Folder";
    BRinfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    BRinfo.lpfn = NULL;
    BRinfo.lParam = (LPARAM)NULL;

    pidlBrowse = SHBrowseForFolder(&BRinfo); 

    if (pidlBrowse != NULL)
    {
        char install_path[255];
        SHGetPathFromIDList(pidlBrowse, install_path);

        SetWindowText(hEdit, install_path);
    }
}

static void install_btn_clicked(HWND hwnd) {
    // Get destination path
    TCHAR dest_path[1024];
    GetWindowText(hEdit, dest_path, 1024);

    if (string_is_null_or_whitespace(dest_path)) {
        MessageBox(hwnd, "Please enter a valid directory path.", "Install error", MB_OK);
        return;
    }

    // Get current file path
    TCHAR cur_file[MAX_PATH + 1];
    GetModuleFileName(NULL, cur_file, MAX_PATH + 1);

    // Extract files from current file into destination folder
    FILE_EXTRACT_RESULT result = extract_files(cur_file, dest_path);
    char* result_msg = get_result_msg(result);

    // Done
    MessageBox(hwnd, result_msg, "Install result", MB_OK);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg)
    {
        // Form created
        case WM_CREATE:
        {
            HWND hwnd_st_u = CreateWindow("static", "ST_U",
                              WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                              50, 90, 400, 30,
                              hwnd, (HMENU)(501),
                              (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);
            SetWindowText(hwnd_st_u, "Enter destination directory:");

            HFONT hfDefault;

            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 
                50, 110, 300, 30, hwnd, (HMENU)101, GetModuleHandle(NULL), NULL);
            if (hEdit == NULL)
                MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

            hfDefault = GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

            HWND hButton = CreateWindow("BUTTON", "..", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                360, 110, 50, 30, hwnd, (HMENU)10000, (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);
            if (hButton == NULL)
                MessageBox(hwnd, "Could now create browse button.", "Error", MB_OK | MB_ICONERROR);

            HWND hInstallButton = CreateWindow("BUTTON", "Install", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                150, 150, 100, 30, hwnd, (HMENU)10001, (HINSTANCE) GetWindowLong (hwnd, GWL_HINSTANCE), NULL);
            if (hInstallButton == NULL)
                MessageBox(hwnd, "Could now create install button.", "Error", MB_OK | MB_ICONERROR);
        }
        break;

        // Some action on form
        case WM_COMMAND:
            if (LOWORD(wParam) == 10000)
                browse_btn_clicked(hwnd);

            if (LOWORD(wParam) == 10001)
                install_btn_clicked(hwnd);
            break;

        // Close form
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        // Destroy form
        case WM_DESTROY:
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/*
    Installer entry point.
    Opens GUI form, which handles user input and calls installer core functions.
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {

    const char g_szClassName[] = "myWindowClass";
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    // Register the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Installer application",
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}
