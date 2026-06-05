#include "17-world2.h"

// InitialiseExtraRenders

// AddExtraRender

// PointOutOfSight

// CancelificateClipulatingPlaneyThings

// FixificateClipulatingPlaneyThings

// ProcessModelFaceMaterials

// ProcessModelFaceMaterials2

// ProcessFaceMaterials

// FUNCTION: CARMA2_HW 0x004475c0
int C2_HOOK_FASTCALL DRPixelmapHasZeros(br_pixelmap* pm) {
    int x;
    int y;
    tU8* row_ptr;
    tU8* pp;

    if (pm->flags & BR_PMF_NO_ACCESS) {
        return 1;
    }
    row_ptr = (tU8*)pm->pixels + pm->base_x + pm->base_y * pm->row_bytes;
    for (y = 0; y < pm->height; y++) {
        pp = row_ptr;
        for (x = 0; x < pm->width; x++, pp++) {
            if (*pp == '\0') {
                return 1;
            }
        }
        row_ptr += pm->row_bytes;
    }
    return 0;
}

// StorageContainsPixelmap

// HideStoredOpaqueTextures

// RevealStoredTransparentTextures

// HideStoredTextures

// RevealStoredTextures

// SetCarStorageTexturingLevel

// GetCarTexturingLevel

// STUB: CARMA2_HW 0x00447650
void C2_HOOK_FASTCALL SetCarTexturingLevel(tCar_texturing_level pLevel) {
    NOT_IMPLEMENTED();
}

// HasThisSuffix

// UnsuffixedMaterial

// RoadUntexToPersp

// WallUntexToPersp

// WallLinearToPersp

// GetRoadTexturingLevel

// STUB: CARMA2_HW 0x00448780
void C2_HOOK_FASTCALL SetRoadTexturingLevel(tRoad_texturing_level pLevel) {
    NOT_IMPLEMENTED();
}

// GetWallTexturingLevel

// STUB: CARMA2_HW 0x00448900
void C2_HOOK_FASTCALL SetWallTexturingLevel(tWall_texturing_level pLevel) {
    NOT_IMPLEMENTED();
}

// DisposeSuffixedMaterials

// DisposeTexturingMaterials

// SetAccessoryRenderingCB

// STUB: CARMA2_HW 0x00448ec0
void C2_HOOK_FASTCALL SetAccessoryRendering(int pOn) {
    NOT_IMPLEMENTED();
}

// GetAccessoryRendering

// STUB: CARMA2_HW 0x00448f10
void C2_HOOK_FASTCALL SetCarSimplificationLevel(int pLevel) {
    NOT_IMPLEMENTED();
}

// GetCarSimplificationLevel

// ProcessMaterials

// BuildColourTable

