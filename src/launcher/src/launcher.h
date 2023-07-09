#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <windows.h>

void Direct3DRefreshDeviceCombobox(void);
int CALLBACK MainWindowDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void EnableD3DOptions(int enable);

#endif /* LAUNCHER_H */
