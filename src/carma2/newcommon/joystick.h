/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_JOYSTICK_H
#define REC2_JOYSTICK_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern int gJoystick_index;
extern float gJoystick_x_steering;
extern float gJoystick_y_throttle;
extern int gINT_00595f84;
extern int gINT_00596310;
extern int gJoystick_preferences_backup_valid;
extern tButtonJoystickInfo gJoystick_preferences_backup;

extern int C2_HOOK_FASTCALL FindJoystickPreferences(FILE* pF, const char* pJoystick_name);

extern int C2_HOOK_FASTCALL ReadJoystickPreferences(FILE* pF, tButtonJoystickInfo* pInfo);

extern void C2_HOOK_FASTCALL EnableJoysticks(void);

extern void C2_HOOK_FASTCALL DisableJoysticks(void);

extern int C2_HOOK_FASTCALL LoadJoystickPreferences(void);
extern int C2_HOOK_FASTCALL SaveJoystickPreferences(void);
extern void C2_HOOK_FASTCALL BackupJoystickPreferences(void);
extern void C2_HOOK_FASTCALL RestoreJoystickPreferences(void);

extern int C2_HOOK_FASTCALL HasCurrentJoystick(void);

extern int C2_HOOK_FASTCALL ReadUntilCharacter(FILE* pF, char* pBuffer, int pBuffer_size, char pLimit_char);

extern int C2_HOOK_FASTCALL WriteJoystickPreferences(FILE* pF, const tButtonJoystickInfo* pInfo);

extern int C2_HOOK_FASTCALL CountJoystickPreferences(FILE* pF);

extern int C2_HOOK_FASTCALL ToggleJoystickThrottle(void);

extern void C2_HOOK_FASTCALL SetJoystickDPadEnabled(int pEnabled);

extern int C2_HOOK_FASTCALL CycleJoystickFFBGain(void);

extern void C2_HOOK_FASTCALL SetJoystickFFBGain(int pValue);

extern int C2_HOOK_FASTCALL GetJoystickFBBGain(void);

extern void C2_HOOK_FASTCALL SetJoystickX(float pValue);

extern void C2_HOOK_FASTCALL SetJoystickY(float pValue);

extern float C2_HOOK_FASTCALL GetJoystickX(void);

extern float C2_HOOK_FASTCALL GetJoystickY(void);

#endif /* REC2_JOYSTICK_H */
