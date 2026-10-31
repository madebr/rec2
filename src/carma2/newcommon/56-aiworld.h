/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_56_AIWORLD_H
#define GUARD_56_AIWORLD_H

#include "c2_hooks.h"
#include "rec2_types.h"

#include "brender/brender.h"

#include <stdio.h>

// MakeVertexAndOffsetIt

extern void C2_HOOK_FASTCALL MakeFaceAndTextureIt(br_model* pModel, int pFace_num, br_uint_16 pV0, br_uint_16 pV1, br_uint_16 pV2, br_material* pMaterial);

extern void C2_HOOK_FASTCALL InitPanGameAIWorld(void);

extern void C2_HOOK_FASTCALL LoadAIWorldTrackInfo(FILE* pF);

extern void C2_HOOK_FASTCALL LoadAIWorldForRace(tRace_info* pRace_info);

// InitialiseAIWorldForRace

extern void C2_HOOK_FASTCALL DisposeAIWorldRaceStuff(void);

// MungeAIWorld

extern void C2_HOOK_FASTCALL PointActorAlongThisBloodyVector(br_actor* pThe_actor, br_vector3* pThe_vector);

extern void C2_HOOK_FASTCALL PointActorAlongVectorWithUp(br_actor* pThe_actor, br_vector3* pLook, br_vector3* pUp);

extern br_material* C2_HOOK_FASTCALL CreateSimpleMaterial(int pIndex);

// MakeAIEditCubeHere

// CalcNegativeXVector

// MakeAISimpleEditSectionHere

#endif // GUARD_56_AIWORLD_H
