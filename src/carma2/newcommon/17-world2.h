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

// HideStoredOpaqueTextures

// RevealStoredTransparentTextures

// HideStoredTextures

// RevealStoredTextures

// SetCarStorageTexturingLevel

// GetCarTexturingLevel

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

// SetAccessoryRenderingCB

extern void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn);

// GetAccessoryRendering

extern void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel);

// GetCarSimplificationLevel

// ProcessMaterials

// BuildColourTable

#endif // GUARD_17_WORLD2_H
