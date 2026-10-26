/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_CREDITS_H
#define FRONTEND_CREDITS_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_CREDITS;


int C2_HOOK_FASTCALL CreditsScreenInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL CreditsScreenOutfunc(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CREDITS_H */