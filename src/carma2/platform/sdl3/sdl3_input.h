/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_SDL3_INPUT_H
#define REC2_SDL3_INPUT_H

#include "rec2_types.h"

extern int gASCII_table[151];
extern int gASCII_shift_table[151];

extern void C2_HOOK_FASTCALL Win32InitInputDevice(void);

extern void KeyBegin(void);

extern void C2_HOOK_FASTCALL CloseDirectInput(void);

extern void PDSetKeyArray(int *pKeys, int pMark);

extern void PDReadJoysticks(void);

extern int GetDirectInputJoy1X(void);

extern int GetDirectInputJoy1Y(void);

extern tU32 PDGetJoy1Button1(void);

extern tU32 PDGetJoy1Button2(void);

extern tU32 PDGetJoy1Button3(void);

extern tU32 PDGetJoy1Button4(void);

extern int C2_HOOK_FASTCALL PDGetJoy1X(void);

extern int PDGetJoy1Y(void);

extern tU32 PDGetJoy2Button1(void);

extern tU32 PDGetJoy2Button2(void);

extern tU32 PDGetJoy2Button3(void);

extern tU32 PDGetJoy2Button4(void);

extern int PDGetJoy2X(void);

extern int PDGetJoy2Y(void);

extern void PDInitJoysticks(void);

#endif /* REC2_SDL3_INPUT_H */
