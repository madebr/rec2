/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright 2022-2026 rec2 developers
 */

#ifndef GUARD_17_WORLD2_H
#define GUARD_17_WORLD2_H

#include "c2_hooks.h"
#include "rec2_types.h"

extern int gCar_simplification_level;

extern tU32 gWild_start;

extern tU32 gQuite_wild_start;

extern tU32 gQuite_wild_end;

extern tU32 gOn_me_wheels_start;

extern tU32 gWoz_upside_down_at_all;

extern void C2_HOOK_FASTCALL InitialiseExtraRenders(void);

extern void C2_HOOK_FASTCALL AddExtraRender(br_actor* pActor, br_material* pMaterial);

extern int C2_HOOK_FASTCALL PointOutOfSight(const br_vector3* pPoint, undefined4 pArg2, br_scalar pMax_distance);

extern void C2_HOOK_FASTCALL CancelificateClipulatingPlaneyThings(void);

extern void C2_HOOK_FASTCALL FixificateClipulatingPlaneyThings(void);

extern void C2_HOOK_FASTCALL ProcessModelFaceMaterials(br_model* pModel, tPMFMCB* pCallback);

extern void C2_HOOK_FASTCALL ProcessModelFaceMaterials2(br_model* pModel, material_cbfn* pCallback);

extern intptr_t C2_HOOK_CDECL ProcessFaceMaterials(br_actor* pActor, void* pData);

extern int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm);

extern int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap);

extern void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);

extern tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

extern int C2_HOOK_FASTCALL HasThisSuffix(const char* pIdent, const char* pSuffix);

extern br_material* C2_HOOK_FASTCALL UnsuffixedMaterial(const char* pOld_ident, const char* pSuffix);

extern br_material* C2_HOOK_FASTCALL RoadUntexToPersp(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallUntexToPersp(br_model* pModel, tU16 pFace);

extern br_material* C2_HOOK_FASTCALL WallLinearToPersp(br_model* pModel, tU16 pFace);

extern tRoad_texturing_level C2_HOOK_FASTCALL GetRoadTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

extern tWall_texturing_level C2_HOOK_FASTCALL GetWallTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

extern br_material* C2_HOOK_FASTCALL DisposeSuffixedMaterials(br_model* pModel, tU16 pFace);

// DisposeTexturingMaterials

extern intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag);

extern void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn);

extern int C2_HOOK_FASTCALL GetAccessoryRendering(void);

extern void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

extern int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);

extern intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, void* pContext);

extern void C2_HOOK_FASTCALL BuildColourTable(br_pixelmap* pPalette);

#endif // GUARD_17_WORLD2_H
