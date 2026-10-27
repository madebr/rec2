/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_43_PRATCAM_H
#define GUARD_43_PRATCAM_H

#include "c2_hooks.h"

extern void C2_HOOK_FASTCALL ChangeAmbientPratcam(int pIndex);

extern void C2_HOOK_FASTCALL PratcamEvent(int pIndex);

extern void C2_HOOK_FASTCALL InitPratcam(void);

extern void C2_HOOK_FASTCALL DisposePratcam(void);

#endif // GUARD_43_PRATCAM_H
