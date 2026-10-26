/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_MAIN_H
#define FRONTEND_MAIN_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_MAIN;
extern int gFrontend_car_image_outdated;
extern int gFrontend_net_initialized;


int C2_HOOK_FASTCALL MainMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL MainMenuOutfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL NewGameToggleTyping(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_MainMenuHandler(tFrontend_spec* pFrontend);

void C2_HOOK_FASTCALL UpdateCarInfo(tFrontend_spec *pFrontend);

int C2_HOOK_FASTCALL OnePlayerSetup(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL MultiplayerSetup(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL testUp(tFrontend_spec *pFrontend);

int C2_HOOK_FASTCALL testDn(tFrontend_spec *pFrontend);

int C2_HOOK_FASTCALL MainMenuSelectRace(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL StartFudge(tFrontend_spec* pFrontend);

#endif /* FRONTEND_MAIN_H */