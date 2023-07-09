#ifndef UTIL_H
#define UTIL_H

#include <windows.h>

#define ARRAY_SIZE(A) ((sizeof(A)/sizeof((A)[0])))

void CenterWindowOnScreen(HWND Wnd);

int ComboboxClearAllOptions(HWND Wnd, int nIDDlgItem);

void SendDialogMessageFormat(HWND hDlg, int idDlgItem, UINT Msg, WPARAM wParam, char *format, ...);

#endif /* UTIL_H */
