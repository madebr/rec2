/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_OIL_H
#define REC2_OIL_H

#include "rec2_types.h"

#include "c2_hooks.h"

extern int gNext_oil_pixie;

void C2_HOOK_FASTCALL InitOilSpills(void);

void C2_HOOK_FASTCALL ResetOilSpills(void);

void C2_HOOK_FASTCALL SetInitialOilStuff(tOil_spill_info* pOil, br_model* pModel);

void C2_HOOK_FASTCALL MungeOilsHeightAeGround(tOil_spill_info* pThe_spill);

int C2_HOOK_FASTCALL OKToSpillOil(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL MungeOilsHeightAboveGround(tOil_spill_info* pOil);

void C2_HOOK_FASTCALL ProcessOilSpills(tU32 pFrame_period);

int C2_HOOK_FASTCALL GetOilSpillCount(void);

void C2_HOOK_FASTCALL GetOilSpillDetails(int pIndex, br_actor** pActor, br_scalar* pSize);

void C2_HOOK_FASTCALL MungeIndexedOilsHeightAboveGround(int pIndex);

#endif //REC2_OIL_H