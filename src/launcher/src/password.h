/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef PASSWORD_H
#define PASSWORD_H

#include <windows.h>

extern int g_parental_lock;

int AskPassword(HWND hWnd);

#endif /* PASSWORD_H */
