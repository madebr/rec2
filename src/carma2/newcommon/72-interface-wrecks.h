/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_72_WRECKS_H
#define GUARD_72_WRECKS_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern tFrontend_spec gFrontend_WRECKS;

extern int C2_HOOK_FASTCALL BuyCurrentCar(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL WrecksInFunc(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL WrecksUpdateFunc(tFrontend_spec* pFrontend);

#endif /* GUARD_72_WRECKS_H */
