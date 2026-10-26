/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_OPTIONS_H
#define REC2_OPTIONS_H

#include "c2_hooks.h"

extern char* gKey_names[153];
extern int gOrig_key_map_index;


void C2_HOOK_FASTCALL DoOptions(void);

void C2_HOOK_FASTCALL StripControls(char* pStr);

void C2_HOOK_FASTCALL LoadKeyNames(void);

void C2_HOOK_FASTCALL DisposeKeyNames(void);

void C2_HOOK_FASTCALL BackupKeyMappings(void);

void C2_HOOK_FASTCALL Joystick_BackupSettings(void);

#endif //REC2_OPTIONS_H