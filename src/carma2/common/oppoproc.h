/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_OPPOPROC_H
#define REC2_OPPOPROC_H

#include <brender/brender.h>
#include "rec2_types.h"

#include "c2_hooks.h"


void C2_HOOK_CDECL DoNotDprintf_oppoproc(const char* pMessage, ...);

br_scalar C2_HOOK_FASTCALL Vector2Cross(const br_vector2* pV1, const br_vector2* pV2);

tFollow_path_result FollowCheatyPath(tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL GetStraight(br_vector2* pStart, br_vector2* pFinish, float* pWidth, tS16 pSection, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcCorners(tCorner* pCorners, int pSection, float pWidth, br_vector2* pStart, tOpponent_spec* pOpponent_spec);

int C2_HOOK_FASTCALL CalcSOCs(int pNext_section, int pCount_corners, tCorner *pCorners, tOpponent_spec *pOpponent_spec, tSOC* pSocs, tCar_spec* pCar);

#endif //REC2_OPPONENT_H