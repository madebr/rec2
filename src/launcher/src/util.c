#include "util.h"

#include <windows.h>

#include <stdarg.h>
#include <stdio.h>

void CenterWindowOnScreen(HWND Wnd) {
    int width;
    int height;
    RECT r;

    GetWindowRect(Wnd ,&r);
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(Wnd,
        HWND_TOPMOST,
        width / 2 - (r.right - r.left) / 2,
        height / 2 - (r.bottom - r.top) / 2,
        0,
        0,
        SWP_NOZORDER | SWP_NOSIZE);
}

int ComboboxClearAllOptions(HWND Wnd, int nIDDlgItem) {
    while (SendDlgItemMessageA(Wnd, nIDDlgItem, CB_DELETESTRING, 0,0) != -1) {
    }
    return -1;
}

void SendDialogMessageFormat(HWND hDlg, int idDlgItem, UINT Msg, WPARAM wParam, char *format, ...) {
    static char buffer[200];
    va_list va;

    va_start(va, format);
    vsprintf(buffer, format, va);
    SendDlgItemMessageA(hDlg, idDlgItem, Msg, wParam, (LPARAM)buffer);
    va_end(va);
    return;
}
