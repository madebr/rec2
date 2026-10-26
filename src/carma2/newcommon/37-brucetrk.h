/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_37_BRUCETRK_H
#define GUARD_37_BRUCETRK_H

#include "c2_hooks.h"
#include "brender/brender.h"
#include "rec2_types.h"

extern br_material* gDefault_track_material;
extern int gCount_track_non_cars;
extern br_vector3 gAverage_grid_position;

// AssertNoncars

// AllocateActorMatrix

// DisposeActorMatrix

// DisposeRuntimeBuiltModels

extern void C2_HOOK_FASTCALL DisposeColumns(tTrack_spec* pTrack_spec);

extern void C2_HOOK_FASTCALL XZToColumnXZ(tU8* pColumn_x, tU8* pColumn_z, br_scalar pX, br_scalar pZ, tTrack_spec* pTrack_spec);

// FindNonCarsCB

// MungeFaces

// ProcessModelsCB

// ProcessModels

// ExtractColumns

// RenderTrack

extern br_scalar C2_HOOK_STDCALL GetYonFactor(void);

extern void C2_HOOK_STDCALL SetYonFactor(br_scalar pNew);

extern intptr_t C2_HOOK_CDECL FoundAnActor(br_actor* pActor, void* pContext);

// ProcessNearbyActors

#endif // GUARD_37_BRUCETRK_H
