/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef FRONTEND_CHANGECAR_H
#define FRONTEND_CHANGECAR_H

#include "c2_hooks.h"

#include "rec2_types.h"

extern tFrontend_spec gFrontend_CHANGE_CAR;
extern int gFrontend_change_car_selected_car;


int C2_HOOK_FASTCALL ChangeCarMenuInfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL ChangeCarMenuOutfunc(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL FRONTEND_ChangeCarMenuHandler(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL CarClickPrev(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL CarClickNext(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL ChangeCarOK(tFrontend_spec* pFrontend);

int C2_HOOK_FASTCALL ChangeCarCancel(tFrontend_spec* pFrontend);

#endif /* FRONTEND_CHANGECAR_H */