/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_PRATCAM_H
#define REC2_PRATCAM_H

#include "c2_hooks.h"

void C2_HOOK_FASTCALL InitPratcam(void);

void C2_HOOK_FASTCALL DisposePratcam(void);

void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

#endif //REC2_PRATCAM_H