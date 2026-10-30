/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef REC2_EXPLOSIONS_H
#define REC2_EXPLOSIONS_H

#include "rec2_types.h"

#include "c2_hooks.h"
#include "c2_stdio.h"

extern tSmash_explosion gSmash_explosions[20];

void C2_HOOK_FASTCALL InitExplosions(void);

void C2_HOOK_FASTCALL ResetExplosions(void);

void C2_HOOK_FASTCALL ReadExplosionInfo(FILE* pF, int* pChance_explosion, int* pExplosion_sound_id, tExplosion_animation* pExplosion_groups);

int C2_HOOK_FASTCALL PointOutOfSightNotAR(const br_vector3* pPoint, float pMax_distance);

void C2_HOOK_FASTCALL MungeExplosions(void);

void C2_HOOK_FASTCALL Explode(tExplosion_animation* pExplosion_animation, br_actor* pActor, br_bounds3* pBounds, br_vector3* pPos);

#endif // REC2_EXPLOSIONS_H
