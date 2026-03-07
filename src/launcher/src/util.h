#ifndef UTIL_H
#define UTIL_H

#include "c2_hooks.h"

#include <windows.h>

#define ARRAY_SIZE(A) ((sizeof(A)/sizeof((A)[0])))

void CenterWindowOnScreen(HWND Wnd);

int ComboboxClearAllOptions(HWND Wnd, int nIDDlgItem);

void C2_HOOK_CDECL SendDialogMessageFormat(HWND hDlg, int idDlgItem, UINT Msg, WPARAM wParam, char *format, ...);

#endif /* UTIL_H */
