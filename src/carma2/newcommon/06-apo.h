/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_06_APO_H
#define GUARD_06_APO_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern void C2_HOOK_FASTCALL MaxOutAPO(void);

extern void C2_HOOK_FASTCALL ImprovePSPowerup(tCar_spec* pCar, int pIndex);

extern void C2_HOOK_FASTCALL BuyArmour(void);

extern void C2_HOOK_FASTCALL BuyPSPowerup(int pIndex);

extern void C2_HOOK_FASTCALL BuyPower(void);

extern void C2_HOOK_FASTCALL BuyOffense(void);

#endif // GUARD_06_APO_H
