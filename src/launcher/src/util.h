/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef UTIL_H
#define UTIL_H

#include "c2_hooks.h"

#include <windows.h>

#define ARRAY_SIZE(A) ((sizeof(A)/sizeof((A)[0])))

void CenterWindowOnScreen(HWND Wnd);

int ComboboxClearAllOptions(HWND Wnd, int nIDDlgItem);

void C2_HOOK_CDECL SendDialogMessageFormat(HWND hDlg, int idDlgItem, UINT Msg, WPARAM wParam, char *format, ...);

#endif /* UTIL_H */
