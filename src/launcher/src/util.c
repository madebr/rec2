/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

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

void C2_HOOK_CDECL SendDialogMessageFormat(HWND hDlg, int idDlgItem, UINT Msg, WPARAM wParam, char *format, ...) {
    static char buffer[200];
    va_list va;

    va_start(va, format);
    vsprintf(buffer, format, va);
    SendDlgItemMessageA(hDlg, idDlgItem, Msg, wParam, (LPARAM)buffer);
    va_end(va);
    return;
}
