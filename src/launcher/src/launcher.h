/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <windows.h>

void Direct3DRefreshDeviceCombobox(void);
int CALLBACK MainWindowDlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
void EnableD3DOptions(int enable);

#endif /* LAUNCHER_H */
