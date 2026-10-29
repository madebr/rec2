/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_20_SKIDMARK_H
#define GUARD_20_SKIDMARK_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern char* gBoring_material_names[2];
extern char* gMaterial_names[2];
extern const char* gExplosion_rotate_names[2];
extern br_material* gMaterial[2];
extern tSkid gSkids[100];
extern int gCurrent_skid;

// StretchMark

// MaterialFromIndex

extern void C2_HOOK_FASTCALL AdjustSkid(int pSkid_num, br_matrix34* pMatrix, br_material* pMaterial);

// FarFromLine2D

// Reflex2D

extern void C2_HOOK_FASTCALL InitSkids(void);

extern void C2_HOOK_FASTCALL HideSkid(int pSkid_num);

extern void C2_HOOK_FASTCALL HideSkids(void);

// SkidLen

// UglyWidthChange

// GenericSkidSection

extern int C2_HOOK_FASTCALL SkidSection(tS16* pSkid_id, br_vector3* pSkid_start, br_vector3* pSkid_end, br_material* pMaterial, br_vector3* pPos, br_vector3* pNorm, br_vector3* pPrev_pos, br_vector3* pPrev_norm, br_scalar pTexture_start, br_scalar pTexture_step);

extern void C2_HOOK_FASTCALL SkidMark(tCar_spec* pCar_spec, int pWheel_num);

extern void C2_HOOK_FASTCALL InitCarSkidStuff(tCar_spec* pCar);

// SkidsPerFrame

#endif // GUARD_20_SKIDMARK_H
