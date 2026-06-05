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

// FUNCTION: CARMA2_HW 0x004479c0
int C2_HOOK_FASTCALL StorageContainsPixelmap(tBrender_storage* pStorage, br_pixelmap* pMap) {
    int i;

    for (i = 0; i < pStorage->pixelmaps_count; i++) {

        if (pStorage->pixelmaps[i] == pMap) {
            break;
        }
    }
    return i != pStorage->pixelmaps_count;
}

// HideStoredOpaqueTextures

// FUNCTION: CARMA2_HW 0x00447b00
void C2_HOOK_FASTCALL RevealStoredTransparentTextures(tBrender_storage* pStorage) {
    int i;

    for (i = 0; i < pStorage->materials_count; i++) {
        br_pixelmap* colour_map;

        colour_map = pStorage->materialProps[i];

        if (colour_map != NULL && DRPixelmapHasZeros(colour_map)) {
            pStorage->materials[i]->colour_map = colour_map;
            pStorage->materialProps[i] = NULL;
            pStorage->materials[i]->flags |= BR_MATF_PRELIT;
            BrMaterialUpdate(pStorage->materials[i], BR_MATU_ALL);
        }
    }
}

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

