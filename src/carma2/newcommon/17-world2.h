#ifndef GUARD_17_WORLD2_H
#define GUARD_17_WORLD2_H

#include "c2_hooks.h"
#include "rec2_types.h"

// InitialiseExtraRenders

// AddExtraRender

// PointOutOfSight

// CancelificateClipulatingPlaneyThings

// FixificateClipulatingPlaneyThings

// ProcessModelFaceMaterials

// ProcessModelFaceMaterials2

// ProcessFaceMaterials

extern int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm);

extern int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap);

extern void C2_HOOK_FASTCALL HideStoredOpaqueTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL HideStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL RevealStoredTextures(tBrender_storage* pStorage);

extern void C2_HOOK_FASTCALL SetCarStorageTexturingLevel(tBrender_storage* pStorage, tCar_texturing_level pNew, tCar_texturing_level pOld);

extern tCar_texturing_level C2_HOOK_FASTCALL GetCarTexturingLevel(void);

extern void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel);

// HasThisSuffix

// UnsuffixedMaterial

// RoadUntexToPersp

// WallUntexToPersp

// WallLinearToPersp

// GetRoadTexturingLevel

extern void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel);

// GetWallTexturingLevel

extern void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel);

// DisposeSuffixedMaterials

// DisposeTexturingMaterials

extern intptr_t C2_HOOK_CDECL SetAccessoryRenderingCB(br_actor* pActor, void* pFlag);

extern void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn);

// GetAccessoryRendering

extern void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

// GetCarSimplificationLevel

extern intptr_t C2_HOOK_CDECL ProcessMaterials(br_actor* pActor, material_cbfn* pCallback);

// BuildColourTable

#endif // GUARD_17_WORLD2_H
